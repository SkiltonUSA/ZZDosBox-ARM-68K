/* GPL-3.0-or-later. Amiga console host for the application-owned Core1 worker. */
#include <exec/memory.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <stdio.h>
#include <string.h>
#include "wire.h"
#include "stream.h"
#include "window.h"
#include "fs_host.h"
#include "mmu.h"
struct Device *TimerBase;
static volatile uint8_t *shared;
static const struct ad_io *syncio;
static ULONG sequence;
static const char *drive="PROGDIR:CDrive";
static int testmode;
static char auto_command[160];
static const char commands[]="dir /b\rtype readme.txt\recho saved text > SAVED.TXT\rtype saved.txt\rfiletest\rexetest\rcopy LARGE.BIN LARGE2.BIN\rren LARGE2.BIN RENAMED.BIN\rdir /b\rexit\r";
static char transcript[8192];
int db_arguments(int argc,char **argv){
 int i;if(argc==0)return 0;
 for(i=1;i<argc;i++){
  if(!strcmp(argv[i],"--selftest"))testmode=1;
  else if(!strcmp(argv[i],"--cancel-read"))testmode=2;
  else if(!strcmp(argv[i],"--command")&&i+1<argc){
   const char *s=argv[++i];if(strlen(s)>140)return -1;
   snprintf(auto_command,sizeof(auto_command),"%s\rexit\r",s);
  }
  else if(!strcmp(argv[i],"--drive")&&i+1<argc)drive=argv[++i];
  else {puts("Usage: ZZDosBox [--drive drawer] [--command command] [--selftest|--cancel-read]");return -1;}
 }
 return 1;
}
static ULONG us(void){struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
static ULONG value(unsigned field){return ad_get(shared+MB_REPLY,MB_RESULT+field*4);}
static void publish(unsigned offset,ULONG n){ad_put(shared,offset,n);syncio->push(shared+offset,4,NULL);syncio->barrier(NULL);}
static ULONG receive(unsigned offset){syncio->pull(shared+offset,4,NULL);return ad_get(shared,offset);}
static int input(const UBYTE *p,unsigned n){
 ULONG w=ad_get(shared,DB_RX_WRITE),r=receive(DB_RX_READ),used=w-r;unsigned i;
 if(used>DB_RX_SIZE||n>DB_RX_SIZE-used)return 0;
 for(i=0;i<n;i++)shared[DB_RX_DATA+((w+i)&(DB_RX_SIZE-1))]=p[i];
 syncio->push(shared+DB_RX_DATA,DB_RX_SIZE,NULL);publish(DB_RX_WRITE,w+n);return 1;
}
static ULONG received,hash,mismatch;
static int drain(struct DbWindow *console){
 ULONG r=ad_get(shared,DB_TX_READ),w=receive(DB_TX_WRITE),n=w-r,i;UBYTE data[DB_TX_SIZE];
 if(n>DB_TX_SIZE)return 0;
 syncio->pull(shared+DB_TX_DATA,DB_TX_SIZE,NULL);
 for(i=0;i<n;i++){
  UBYTE c=shared[DB_TX_DATA+((r+i)&(DB_TX_SIZE-1))];data[i]=c;
  if(received<sizeof(transcript)-1)transcript[received]=c;
  received++;hash=(hash^c)*16777619u;
 }
 if(n&&console&&!db_window_write(console,data,n))return 0;
 publish(DB_TX_READ,r+n);return 1;
}
static int run(struct DbWindow *console){
 ULONG start=us(),stop_time=0;int sent=0,stopping=0,forced=0,transport_ok=1;
 ULONG token=(sequence+1)^ad_get(shared,ZZ_CONTROL);UBYTE keys[64];
 memset(transcript,0,sizeof(transcript));received=mismatch=0;hash=2166136261u;
 /* Deliberately cross uint32 wrap in automated tests. */
 publish(DB_RX_WRITE,testmode?0xfffffffcu:0);publish(DB_RX_READ,testmode?0xfffffffcu:0);
 publish(DB_TX_WRITE,testmode?0xfffffffcu:0);publish(DB_TX_READ,testmode?0xfffffffcu:0);publish(DB_RX_EOF,0);
 publish(MB_HOST+8,token);publish(MB_HOST+MB_OP,MB_VERIFY);publish(MB_HOST+MB_SEQ,++sequence);
 for(;;){
  ULONG ack,error;int want_stop=0;
  if(!db_fs_service()){transport_ok=0;want_stop=1;}
  if(!drain(console)){transport_ok=0;want_stop=1;}
  if((testmode==1||auto_command[0])&&received>=7&&!sent){
   const UBYTE *testkeys=(const UBYTE *)(auto_command[0]?auto_command:commands);
   if(!input(testkeys,strlen((const char *)testkeys))){transport_ok=0;want_stop=1;}sent=1;
  }
  if(!testmode&&!stopping){
   ULONG w=ad_get(shared,DB_RX_WRITE),r=receive(DB_RX_READ),space;
   unsigned count=0;
   if((ULONG)(w-r)>DB_RX_SIZE){transport_ok=0;want_stop=1;space=0;}else space=DB_RX_SIZE-(w-r);
   if(db_window_closed(console))want_stop=1;
   while(!want_stop&&count<sizeof(keys)&&count<space){
    int next=db_window_read(console);UBYTE c;
    if(next==-1)break;
    if(next<0||next==3){want_stop=1;break;}
    c=(UBYTE)next;
    if(c==10)c=13;
    if(c==127)c=8;
    if(c==13||c==8||c==9||(c>=32&&c<127))keys[count++]=c;
   }
   if(count&&!input(keys,count)){transport_ok=0;want_stop=1;}
  }
  syncio->pull(shared+MB_REPLY,320,NULL);ack=ad_get(shared+MB_REPLY,MB_ACK);error=ad_get(shared+MB_REPLY,MB_ERROR);
  if(ack==sequence){
   int ok=transport_ok&&ad_get(shared+MB_REPLY,68)==(token^ZZ_XOR);
   if(!drain(console))ok=0;
   if(error&&error!=2)printf("DOS error: %.*s\n",159,(const char *)(shared+MB_REPLY+128));
   printf("SHELL result=%u guest_exit=%u slices=%u bytes=%u hash=%08x mismatches=%u\n",error,value(7),value(2),received,hash,mismatch);
   if(testmode==1)ok=ok&&!error&&strstr(transcript,"ZZDOS files/handles/seek PASS")&&strstr(transcript,"ZZDOS MZ relocation/memory PASS")&&strstr(transcript,"saved text\r\n")&&strstr(transcript,"RENAMED.BIN");
   else if(testmode>=2)ok=ok&&forced&&stopping&&error==2;
   else ok=ok&&(!error||(stopping&&error==2));
   printf("%s\nSHELL %s mode=%d\n",transcript,ok?"PASS":"FAIL",testmode);fflush(stdout);return ok?0:20;
  }
  if(testmode==2&&received>=7&&(ULONG)(us()-start)>250000u){want_stop=1;forced=1;}
  if(SetSignal(0,SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C)want_stop=1;
  if((testmode||auto_command[0])&&(ULONG)(us()-start)>90000000u){want_stop=1;transport_ok=0;}
  if(want_stop&&!stopping){stopping=1;stop_time=us();publish(ZZ_STOP,1);}
  if(stopping&&(ULONG)(us()-stop_time)>3000000u){puts("Console stop timeout");return 20;}
  Delay(1);
 }
}
int ff_window(volatile uint8_t *base,const struct ad_io *io,int connected){
 struct MsgPort *port=NULL;struct timerequest *timer=NULL;int opened=0,rc=20;struct DbWindow *console=NULL;
 (void)connected;shared=base;syncio=io;
 if(!db_fs_open(base,io,drive)){puts("Cannot open DOS drive drawer");goto done;}
 if(!testmode){
  console=db_window_open();if(!console){puts("Cannot open console window");goto done;}
 }
 port=CreateMsgPort();if(!port)goto done;
 timer=(struct timerequest *)CreateIORequest(port,sizeof(*timer));if(!timer)goto done;
 if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)timer,0))goto done;
 opened=1;TimerBase=timer->tr_node.io_Device;rc=run(console);
done:
 db_fs_close();
 db_window_close(console);
 if(opened)CloseDevice((struct IORequest *)timer);
 if(timer)DeleteIORequest((struct IORequest *)timer);
 if(port)DeleteMsgPort(port);
 return rc;
}

#ifdef MB_MMU
/* Invoked only after RET1 and synchronous idle reset, before FreeMem. */
int mb_after_stop(volatile uint8_t *base,const struct ad_io *io) {
    ULONG i,bad=0,phase,registers,mask;
    io->pull(base+MB_MMU_REPORT,160,NULL);
    phase=ad_get(base,MB_MMU_REPORT);registers=ad_get(base,MB_MMU_REPORT+4);
    mask=ad_get(base,MB_MMU_REPORT+136);
    io->pull(base+MB_WITNESS-32,1088,NULL);
    for(i=0;i<256;i++)if(ad_get(base,MB_WITNESS+i*4)!=(ad_get(base,ZZ_CONTROL)^i*0x01010101u))bad++;
    for(i=0;i<32;i+=4)if(ad_get(base,MB_WITNESS-32+i) || ad_get(base,MB_WITNESS+1024+i))bad++;
    printf("MMU_RESTORE phase=%u register_mismatches=%08x cpsr_control=%03x expected=%03x ttbr=%08x ccsidr=%08x witness_bad=%u mailbox_rounds=%u\n",
        phase,registers,mask,ad_get(base,MB_MMU_REPORT+16+6*4)&511u,
        ad_get(base,MB_MMU_REPORT+8),ad_get(base,MB_MMU_REPORT+12),bad,sequence);
    for(i=0;i<14;i++)printf("REGISTER n=%u original=%08x restored=%08x\n",i,
        ad_get(base,MB_MMU_REPORT+16+i*4),ad_get(base,MB_MMU_REPORT+80+i*4));
    return phase==3 && !registers && !bad && mask==(ad_get(base,MB_MMU_REPORT+16+6*4)&511u);
}
#endif
