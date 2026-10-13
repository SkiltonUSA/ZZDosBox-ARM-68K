/* GPL-3.0-or-later. Completed-frame/P96 lock and XACP deferred PAN. */
#include <exec/memory.h>
#include <devices/timer.h>
#include <graphics/gfxbase.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <libraries/configvars.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/timer.h>
#include <proto/expansion.h>
#include <proto/Picasso96.h>
#include <string.h>
#include <stdio.h>
#include "display.h"
#include "pc_wire.h"
#include "direct_plan.h"
#include "present_plan.h"
extern struct Device *TimerBase;
struct GfxBase *GfxBase;
static volatile uint8_t *shared;static const struct ad_io *syncio;
static struct Window *window;
static struct BitMap *direct_bitmap;
static LONG direct_lock;
static int direct_locked;
static ULONG direct_format,direct_native_format,active,complete,frames;
/* Host-observed grant-to-completion latency includes polling delay. */
static ULONG copy_start,copy_us,copy_max;
static struct pc_flow flow;
/* Opt-in: current game workloads did not produce frames early enough to overlap. */
static int pipeline=0,console_shown;
static UBYTE *locked_memory;
static ULONG overlaps,releases;
static UBYTE *direct_readback;
static ULONG us(void){struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
#include "fullscreen.h"
static void publish(unsigned offset,ULONG n){ad_put(shared,offset,n);syncio->push(shared+offset,4,NULL);syncio->barrier(NULL);}
void pc_display_pipeline(int enabled){pipeline=enabled!=0;}
int pc_display_open(volatile uint8_t *b,const struct ad_io *io){
 shared=b;syncio=io;active=complete=frames=0;
 copy_start=copy_us=copy_max=0;
 overlaps=releases=0;locked_memory=NULL;pc_flow_init(&flow,pipeline?2:1);
 GfxBase=(struct GfxBase*)OpenLibrary("graphics.library",39);if(!GfxBase)return 0;
 console_shown=0;return 1;
}
int pc_display_locked(void){return direct_locked;}
int pc_display_pending(void){return active||flow.ready||full_slots.pending>=0;}
int pc_display_service(void){
 ULONG seq;struct RenderInfo ri;ULONG span,addr,nonce;UBYTE *target;
 if(!full_poll())return 0;
 syncio->pull(shared+PC_VIDEO,44,NULL);seq=ad_get(shared,PC_VIDEO);
 if(active && ad_get(shared,PC_VIDEO+36)==active){
  ULONG elapsed=us()-copy_start;copy_us+=elapsed;
  if(elapsed>copy_max)copy_max=elapsed;
  if(ad_get(shared,PC_VIDEO+40))return 0;
  if(!pc_flow_complete(&flow))return 0;
  complete=active;active=0;frames++;
 }
 if(flow.ready&&full_slots.pending<0&&!full_queue())return 0;
 if(direct_locked&&pc_flow_idle(&flow,&full_slots)){
  if(!pc_flow_release(&flow,&full_slots))return 0;
  full_unlock();locked_memory=NULL;releases++;
 }
 if(!direct_locked && window){struct IntuiMessage *msg;
  while((msg=(struct IntuiMessage *)GetMsg(window->UserPort))){ULONG cl=msg->Class;UWORD raw=msg->Code;ReplyMsg((struct Message *)msg);
   if(cl==IDCMP_RAWKEY){ULONG w=ad_get(shared,PC_KEY_WRITE),r;syncio->pull(shared+PC_KEY_READ,4,NULL);r=ad_get(shared,PC_KEY_READ);
    if(raw==0x59)return -1;
    if(w-r>=PC_KEY_SIZE)return 0;
    shared[PC_KEY_DATA+(w&(PC_KEY_SIZE-1))]=(UBYTE)raw;syncio->push(shared+PC_KEY_DATA,PC_KEY_SIZE,NULL);publish(PC_KEY_WRITE,w+1);
   }
  }
 }
 if(seq==complete||active||flow.ready)return 1;
 if(seq!=complete+1)return 0;
 if(!full_attached){if(!full_open())return 0;}
 if(console_shown){ScreenToFront(full_screen);ActivateWindow(window);console_shown=0;}
 if(!full_front())return 0;
 if(!pc_flow_can_grant(&flow,&full_slots))return 1;
 if(!direct_locked){
 memset(&ri,0,sizeof(ri));direct_lock=p96LockBitMap(direct_bitmap,(UBYTE*)&ri,sizeof(ri));direct_locked=1;full_origin_known=0;
 full_pitch=ri.BytesPerRow;
 if(!ri.Memory||ri.BytesPerRow<2560||ri.BytesPerRow>4096||(ri.BytesPerRow&3)||(ULONG)ri.RGBFormat!=direct_native_format||
    (ULONG)ri.Memory<(ULONG)full_board+0x10000u||(ULONG)ri.Memory>(ULONG)full_board+0x4000000u-1440*full_pitch)return 0;
 full_origin=(ULONG)ri.Memory-(ULONG)full_board-0x10000u;full_origin_known=1;
 locked_memory=(UBYTE*)ri.Memory;
 }
 if(!locked_memory||!pc_flow_grant(&flow,&full_slots))return 0;
 if(full_slots.pending>=0)overlaps++;
 target=locked_memory+full_slots.render*480*full_pitch;
 addr=(ULONG)target-(ULONG)full_board+0x1f0000u;span=pc_span(addr,full_pitch,direct_format);if(!span)return 0;
 nonce=ad_get(shared,ZZ_CONTROL)^seq^0x973b102du;
 ad_put(target,0,nonce);ad_put(target,span-4,nonce^0x31415926u);syncio->push(target,4,NULL);syncio->push(target+span-4,4,NULL);
 publish(PC_VIDEO+20,addr);publish(PC_VIDEO+24,full_pitch);publish(PC_VIDEO+28,direct_format);publish(PC_VIDEO+32,nonce);
 copy_start=us();publish(PC_VIDEO+16,seq);active=seq;return 1;
}
extern void pc_console_cleanup(void);
void ff_cleanup(void){
 /* Launcher calls this only after synchronous Core1 reset. */
 full_cleanup();
 if(direct_bitmap){p96FreeBitMap(direct_bitmap);direct_bitmap=NULL;}
 if(direct_readback){FreeVec(direct_readback);direct_readback=NULL;}
 pc_console_cleanup();
 printf("DISPLAY frames=%u flips=%u wait_us=%u\n",frames,full_slots.flips,full_wait_us);
 printf("COPY observed_us=%u max_us=%u\n",copy_us,copy_max);
 printf("PIPELINE enabled=%u overlaps=%u releases=%u\n",pipeline,overlaps,releases);
}

void pc_display_console(int waiting){
 if(waiting&&full_screen&&!direct_locked&&!pc_display_pending()&&!console_shown){ScreenToBack(full_screen);console_shown=1;}
}
