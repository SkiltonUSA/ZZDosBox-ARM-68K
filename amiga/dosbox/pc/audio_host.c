/* GPL-3.0-or-later. Reuses the emulator AHI worker; no firmware audio calls. */
#include <exec/memory.h>
#include <devices/ahi.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <stdio.h>
#include <string.h>
#include "audio_host.h"
extern struct Device *TimerBase;
static ULONG us(void){struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
static ULONG audio_gaps,audio_samples;
static volatile uint8_t *shared;
static const struct ad_io *syncio;
/* GPL-3.0-or-later. 68k-only producer/consumer; no PicoDrive code reused.
 * The worker exclusively owns AHI ports, requests and DMA buffers. It never
 * touches ARM shared memory, graphics, libc or the main task's timer.
 * It is a DOS Process because device initialization may use DOS internally.
 * Ring slots remain immutable until copied by the worker. On this single 68k
 * CPU, short Forbid/Permit sections publish indices and guard task lifetime. */
#include <dos/dostags.h>
#define AUDIO_SLOTS 4u
#define AUDIO_PREBUFFER 2u
struct audio_block { ULONG count; UBYTE data[4096]; };
static struct audio_block *audio_queue;
static struct Task *audio_task,*audio_parent;
static volatile ULONG audio_write,audio_read,audio_worker_mask;
static volatile ULONG audio_started,audio_done,audio_stop,audio_error,audio_force;
static volatile ULONG audio_highwater,audio_submitted,audio_completed;
static LONG audio_parent_signal=-1;
static int audio_opened;

static void audio_wake(void){
 Forbid();
 if(audio_task&&!audio_done&&audio_worker_mask)Signal(audio_task,audio_worker_mask);
 Permit();
}
static void audio_worker(void){
 struct MsgPort *port=NULL;
 struct AHIRequest *req[2]={NULL,NULL};
 UBYTE *data[2]={NULL,NULL};
 int active[2]={0,0},opened=0,slot=0,primed=0;
 ULONG submitted_count[2]={0,0};
 LONG signal=AllocSignal(-1);ULONG mask=0,i;
 if(signal<0)goto finish;
 mask=1UL<<signal;audio_worker_mask=mask;
 port=CreateMsgPort();if(!port)goto finish;
 for(i=0;i<2;i++){
  req[i]=(struct AHIRequest *)CreateIORequest(port,sizeof(struct AHIRequest));
  data[i]=AllocVec(4096,MEMF_PUBLIC);
  if(!req[i]||!data[i])goto finish;
 }
 req[0]->ahir_Version=4;
 if(OpenDevice(AHINAME,0,(struct IORequest *)req[0],0))goto finish;
 opened=1;
 req[1]->ahir_Std.io_Device=req[0]->ahir_Std.io_Device;
 req[1]->ahir_Std.io_Unit=req[0]->ahir_Std.io_Unit;req[1]->ahir_Version=4;
 audio_opened=1;audio_started=1;Signal(audio_parent,1UL<<audio_parent_signal);
 for(;;){
  ULONG available,n,index;int other=slot^1;
  if(audio_stop==1)break;
  Forbid();available=audio_write-audio_read;Permit();
  if(!primed&&(available>=AUDIO_PREBUFFER||audio_stop==2||audio_force))primed=1;
  /* Keep the previous request intact until its linked successor has been
   * submitted. Reap a slot only when it is about to be reused. */
  if(primed&&available&&(!active[slot]||CheckIO((struct IORequest *)req[slot]))){
   if(active[slot]){
    WaitIO((struct IORequest *)req[slot]);active[slot]=0;
    if(req[slot]->ahir_Std.io_Error){audio_error=1;break;}
    audio_completed+=submitted_count[slot];
   }
   index=audio_read%AUDIO_SLOTS;n=audio_queue[index].count;
   CopyMem(audio_queue[index].data,data[slot],n*4);
   Forbid();audio_read++;Permit();
   req[slot]->ahir_Std.io_Command=CMD_WRITE;
   req[slot]->ahir_Std.io_Data=data[slot];req[slot]->ahir_Std.io_Length=n*4;
   req[slot]->ahir_Std.io_Offset=0;req[slot]->ahir_Frequency=32000;
   req[slot]->ahir_Type=AHIST_S16S;req[slot]->ahir_Volume=0x10000;
   req[slot]->ahir_Position=0x8000;
   if(active[other]&&CheckIO((struct IORequest *)req[other]))audio_gaps++;
   req[slot]->ahir_Link=active[other]?req[other]:NULL;
   /* AHI can rewrite io_Length from bytes to samples; retain our count. */
   submitted_count[slot]=n;SendIO((struct IORequest *)req[slot]);
   active[slot]=1;audio_submitted+=n;slot=other;
   continue;
  }
  if(audio_stop==2&&!available){
   int busy=0;
   for(i=0;i<2;i++)if(active[i]){
    if(!CheckIO((struct IORequest *)req[i]))busy=1;
   }
   if(!busy)break;
  }
  Wait(mask|(1UL<<port->mp_SigBit));
 }
finish:
 for(i=0;i<2;i++)if(active[i]){
  int aborted=!CheckIO((struct IORequest *)req[i]);
  if(aborted)AbortIO((struct IORequest *)req[i]);
  WaitIO((struct IORequest *)req[i]);
  if(!aborted&&!req[i]->ahir_Std.io_Error)audio_completed+=submitted_count[i];
  else if(!aborted)audio_error=1;
 }
 if(opened)CloseDevice((struct IORequest *)req[0]);
 for(i=0;i<2;i++){
  if(req[i])DeleteIORequest((struct IORequest *)req[i]);
  if(data[i])FreeVec(data[i]);
 }
 if(port)DeleteMsgPort(port);
 if(signal>=0)FreeSignal(signal);
 /* Return to the DOS process wrapper while still forbidden, so the parent
  * cannot unload our entry code before it returns. DOS owns process teardown. */
 Forbid();audio_worker_mask=0;audio_done=1;audio_started=1;
 Signal(audio_parent,1UL<<audio_parent_signal);
}
static void player_audio_close(void){
 if(audio_task){
  ULONG start=us();
  audio_stop=2;audio_wake(); /* Flush even a short stream below prebuffer. */
  while(!audio_done){
   if((ULONG)(us()-start)>2000000u){audio_stop=1;audio_wake();}
   /* Never unload live worker code or free buffers on a timeout. */
   Delay(1);
  }
  audio_task=NULL;
 }
 audio_opened=0;
 if(audio_parent_signal>=0){SetSignal(0,1UL<<audio_parent_signal);FreeSignal(audio_parent_signal);audio_parent_signal=-1;}
 if(audio_queue){FreeVec(audio_queue);audio_queue=NULL;}
 printf("AUDIO_TASK queued=%u submitted=%u completed=%u highwater=%u error=%u stopped=%u gaps=%u\n",
  audio_samples,audio_submitted,audio_completed,audio_highwater,audio_error,audio_done,audio_gaps);
}
static int player_audio_open(void){
 audio_parent=FindTask(NULL);audio_parent_signal=AllocSignal(-1);
 if(audio_parent_signal<0)goto fail;
 audio_queue=AllocVec(sizeof(*audio_queue)*AUDIO_SLOTS,MEMF_PUBLIC|MEMF_CLEAR);
 if(!audio_queue)goto fail;
 /* AHI may load preferences/drivers using DOS: use a Process, not an Exec
  * Task. No shell file handles or CLI are inherited by the child. */
 audio_task=(struct Task *)CreateNewProcTags(NP_Entry,(ULONG)audio_worker,
  NP_Name,(ULONG)"ZZDosBox audio",NP_Priority,0,NP_StackSize,32768,
  NP_Input,0,NP_Output,0,NP_CloseInput,FALSE,NP_CloseOutput,FALSE,TAG_DONE);
 if(!audio_task)goto fail;
 while(!audio_started)Wait(1UL<<audio_parent_signal);
 if(!audio_opened)goto fail;
 puts("AUDIO task: AHI unit 0, 32000 Hz stereo; 4 queue slots, 64 ms prebuffer");return 1;
fail:
 player_audio_close();puts("AHI unit 0 unavailable: select Sound card Off to launch without audio");return 0;
}

int db_audio_host_open(volatile uint8_t*b,const struct ad_io*io,unsigned enabled){
 shared=b;syncio=io;
 ad_put(b,PC_AUDIO_WRITE,0);ad_put(b,PC_AUDIO_READ,0);
 io->push(b+PC_AUDIO_WRITE,4,NULL);io->push(b+PC_AUDIO_READ,4,NULL);
 if(!enabled)return 1;
 return player_audio_open();
}
int db_audio_host_service(void){
 ULONG r,w,n,index;volatile uint8_t*b=shared;
 if(!audio_opened)return 1;
 if(audio_error||audio_done)return 0;
 r=ad_get(b,PC_AUDIO_READ);syncio->pull(b+PC_AUDIO_WRITE,4,NULL);w=ad_get(b,PC_AUDIO_WRITE);syncio->barrier(NULL);
 if(w-r>PC_AUDIO_SLOTS)return 0;
 if(r==w)return 1;
 Forbid();n=audio_write-audio_read;Permit();
 if(n==AUDIO_SLOTS)return 1;
 index=audio_write%AUDIO_SLOTS;
 syncio->pull(b+PC_AUDIO_DATA+(r&(PC_AUDIO_SLOTS-1))*PC_AUDIO_STRIDE,PC_AUDIO_STRIDE,NULL);
 CopyMem((APTR)(b+PC_AUDIO_DATA+(r&(PC_AUDIO_SLOTS-1))*PC_AUDIO_STRIDE),audio_queue[index].data,PC_AUDIO_STRIDE);
 audio_queue[index].count=PC_AUDIO_FRAMES;
 Forbid();audio_write++;if(n+1>audio_highwater)audio_highwater=n+1;Permit();
 audio_samples+=PC_AUDIO_FRAMES;audio_wake();
 ad_put(b,PC_AUDIO_READ,r+1);syncio->push(b+PC_AUDIO_READ,4,NULL);syncio->barrier(NULL);
 return 1;
}
int db_audio_host_pending(void){
 if(!audio_opened)return 0;
 syncio->pull(shared+PC_AUDIO_WRITE,4,NULL);return ad_get(shared,PC_AUDIO_WRITE)!=ad_get(shared,PC_AUDIO_READ);
}
void db_audio_host_close(void){if(audio_task||audio_queue)player_audio_close();}
