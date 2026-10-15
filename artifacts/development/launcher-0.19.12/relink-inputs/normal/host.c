/* GPL-3.0-or-later. Amiga console host for the application-owned Core1 worker. */
#include <exec/memory.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <stdio.h>
#include <string.h>
#include "wire.h"
#include "stream_host.h"
#include "window.h"
#include "fs_host.h"
#include "fs_wire.h"
#include "display.h"
#include "cd_host.h"
#include "midi_host.h"
#include "pc_wire.h"
#include "mmu.h"
#include "profile_plan.h"
#include "options.h"
#include "../launcher/session.h"
#ifdef DB_NETWORK
#include "../network/host.h"
#include "../network/control_host.h"
static int network_enabled;
static const char *network_dns;
static struct dn_config network_config;
#endif
static struct dl_session settings;
static int configured;
static char boot_name[FS_NAME_SIZE],hdd_name[FS_NAME_SIZE];
static unsigned memory_override;
static int machine_override=-1;
static unsigned monochrome;
struct Device *TimerBase;
static volatile uint8_t *shared;
static const struct ad_io *syncio;
static ULONG sequence;
static const char *drive="PROGDIR:CDrive";
static struct DbWindow *deferred_console;
void pc_console_cleanup(void){db_window_close(deferred_console);deferred_console=NULL;}
static int testmode;
static int profiling;
#ifdef DB_VIDEO_TRACE
#include "../launcher/developer.h"
static unsigned trace_enabled,break_phase,break_frame;
#endif
#ifdef DB_PRESENT_TEST
static unsigned present_mode,present_hz=60;
#endif
static ULONG duration=8000,wall_limit_us=90000000u;
/* Keep the original tick wait available for controlled A/B measurements. */
static ULONG poll_ms=1;
static struct timerequest *poll_timer;
static ULONG wait_calls,wait_us,wait_max;
static char auto_command[176];
static const char commands[]="dir /b\rtype readme.txt\recho saved text > SAVED.TXT\rtype saved.txt\rfiletest\rexetest\rcopy LARGE.BIN LARGE2.BIN\rren LARGE2.BIN RENAMED.BIN\rdir /b\rexit\r";
static char transcript[8192];
int db_arguments(int argc,char **argv){
 unsigned test_ms=0;int i;if(argc==0)return 0;dl_defaults(&settings);settings.display=1;
 for(i=1;i<argc;i++){
  if(!strcmp(argv[i],"--session")&&i+1<argc){
   if(!dl_session_read(argv[++i],&settings)){puts("Invalid launcher session");return -1;}
   configured=1;drive=settings.drive;duration=settings.live?0:8000;
   if(settings.system){strcpy(hdd_name,settings.system==2?settings.image:"DOS622.IMG");duration=0;}
   if(*settings.command&&!settings.system)snprintf(auto_command,sizeof(auto_command),"%s\r%s",settings.command,settings.terminal?"":"exit\r");
  }
  else if(!strcmp(argv[i],"--snapshot")&&i+1<argc){if(strlen(argv[i+1])>=256)return -1;pc_display_screenshot_path(argv[++i]);}
  else if(!strcmp(argv[i],"--window"))settings.display=0;
#ifdef DB_NETWORK
  else if(!strcmp(argv[i],"--network"))network_enabled=1;
  else if(!strcmp(argv[i],"--network-config")&&i+1<argc){
   if(!dn_file_read(argv[++i],&network_config)||!zz_net_configure(&network_config)){puts("Invalid network configuration");return -1;}
   network_enabled=network_config.enabled;
  }
  else if(!strcmp(argv[i],"--dns")&&i+1<argc)network_dns=argv[++i];
#endif
  else if(!strcmp(argv[i],"--fullscreen"))settings.display=1;
  else if(!strcmp(argv[i],"--window-size")&&i+1<argc){const char *n=argv[++i];if(strlen(n)!=1||!strchr("012",*n))return -1;settings.size=*n-'0';}
  else if(!strcmp(argv[i],"--fps"))settings.fps=1;
  else if(!strcmp(argv[i],"--memory")&&i+1<argc){const char *m=argv[++i];if(strlen(m)!=1||!strchr("1248",*m))return -1;memory_override=*m-'0';}
  else if(!strcmp(argv[i],"--monochrome")&&i+1<argc){static const char*names[]={"white","amber","green","paperwhite"};const char*m=argv[++i];for(monochrome=0;monochrome<4&&strcmp(m,names[monochrome]);monochrome++){}if(monochrome==4)return -1;}
  else if(!strcmp(argv[i],"--machine")&&i+1<argc){
   static const char*names[]={"vga","ega","cga","s3","tandy","pcjr","hercules"};unsigned n;const char*m=argv[++i];
   for(n=0;n<PC_ADAPTER_COUNT&&strcmp(m,names[n]);n++){}if(n==PC_ADAPTER_COUNT)return -1;machine_override=n;
  }
  else if(!strcmp(argv[i],"--hdd")&&i+1<argc){unsigned j;const char *name=argv[++i];if(strlen(name)>=sizeof(hdd_name))return -1;strcpy(hdd_name,name);for(j=0;hdd_name[j];j++)if(hdd_name[j]>='a'&&hdd_name[j]<='z')hdd_name[j]-=32;if(!fs_name_valid(hdd_name))return -1;duration=0;}
  else if(!strcmp(argv[i],"--boot")&&i+1<argc){unsigned j;const char *name=argv[++i];if(strlen(name)>=sizeof(boot_name))return -1;strcpy(boot_name,name);for(j=0;boot_name[j];j++)if(boot_name[j]>='a'&&boot_name[j]<='z')boot_name[j]-=32;if(!fs_name_valid(boot_name))return -1;duration=0;}
  else if(!strcmp(argv[i],"--selftest"))testmode=1;
  else if(!strcmp(argv[i],"--cancel-read"))testmode=2;
  else if(!strcmp(argv[i],"--serial-video"))pc_display_pipeline(0);
  else if(!strcmp(argv[i],"--pipeline-video"))pc_display_pipeline(1);
#ifdef DB_PRESENT_TEST
  else if(!strcmp(argv[i],"--presentation")&&i+1<argc){
   const char*m=argv[++i];
   if(!strcmp(m,"off"))present_mode=0;
   else if(!strcmp(m,"adaptive"))present_mode=1;
   else if(strlen(m)==1&&strchr("234",*m))present_mode=*m-'0';
   else return -1;
  }
  else if(!strcmp(argv[i],"--present-hz")&&i+1<argc){
   const char*m=argv[++i];
   if(!strcmp(m,"30"))present_hz=30;
   else if(!strcmp(m,"50"))present_hz=50;
   else if(!strcmp(m,"60"))present_hz=60;
   else return -1;
  }
#endif
#ifdef DB_SCANLINE_TEST
  else if(!strcmp(argv[i],"--scanlines")&&i+1<argc){
   const char*m=argv[++i];
   if(!strcmp(m,"off"))pc_display_scanlines(0);
   else if(!strcmp(m,"classic"))pc_display_scanlines(2);
   else if(!strcmp(m,"soft"))pc_display_scanlines(3);
   else if(!strcmp(m,"gradient"))pc_display_scanlines(4);
   else return -1;
  }
  else if(!strcmp(argv[i],"--scanline-copy")&&i+1<argc){
   const char *m=argv[++i];
   if(!strcmp(m,"off"))pc_display_scanlines(0);
   else if(!strcmp(m,"full"))pc_display_scanlines(1);
   else if(!strcmp(m,"skip"))pc_display_scanlines(2);
   else return -1;
  }
#endif
#ifdef DB_VIDEO_TRACE
  else if(!strcmp(argv[i],"--trace-video"))trace_enabled=1;
  else if((!strcmp(argv[i],"--break-before")||!strcmp(argv[i],"--break-after"))&&i+1<argc){
   if(break_phase)return -1;
   break_phase=!strcmp(argv[i],"--break-before")?1:2;
   if(!dl_dev_frame(argv[++i],&break_frame))return -1;
  }
#endif
  else if(!strcmp(argv[i],"--test-ms")&&i+1<argc){
   const char *v=argv[++i];char *end;unsigned long n=strtoul(v,&end,10);
   if(!*v||*end||n<1||n>60000)return -1;
   test_ms=(unsigned)n;
  }
  else if(!strcmp(argv[i],"--profile"))profiling=1;
  else if(!strcmp(argv[i],"--command")&&i+1<argc){
   const char *s=argv[++i];if(strlen(s)>140)return -1;
   snprintf(auto_command,sizeof(auto_command),"%s\rexit\r",s);
  }
  else if(!strcmp(argv[i],"--drive")&&i+1<argc)drive=argv[++i];
  else if(!strcmp(argv[i],"--poll-ms")&&i+1<argc){
   const char *s=argv[++i];
   if(!strcmp(s,"1"))poll_ms=1;
   else if(!strcmp(s,"4"))poll_ms=4;
   else if(!strcmp(s,"20"))poll_ms=20;
   else {puts("--poll-ms requires 1, 4 or 20");return -1;}
  }
  else {puts("Usage: ZZDosBox [--drive drawer] [--boot FLOPPY.IMG] [--hdd HDD.IMG] [--command command] [--poll-ms 1|4|20] [--serial-video|--pipeline-video] [--profile] [--test-ms 1..60000] [--selftest|--cancel-read]");return -1;}
 }
 if(test_ms){duration=test_ms;if(test_ms>4500)wall_limit_us=test_ms*20000u;}
 if(machine_override>=0){settings.machine=machine_override;configured=1;}
 if(memory_override){settings.memory=memory_override;configured=1;}
 if(!configured&&!testmode&&!auto_command[0]&&!boot_name[0]&&!hdd_name[0]){puts("PC prototype requires --command; use the Shell build for an interactive prompt.");return -1;}
 return 1;
}
static ULONG us(void){struct timeval t;GetSysTime(&t);return t.tv_secs*1000000u+t.tv_micro;}
static int poll_wait(void){
 ULONG start=us(),elapsed;LONG error=0;
 if(poll_ms==20)Delay(1);
 else {
  poll_timer->tr_node.io_Command=TR_ADDREQUEST;
  poll_timer->tr_time.tv_secs=0;poll_timer->tr_time.tv_micro=poll_ms*1000;
  error=DoIO((struct IORequest *)poll_timer);
 }
 elapsed=us()-start;wait_calls++;wait_us+=elapsed;
 if(elapsed>wait_max)wait_max=elapsed;
 return !error;
}
static ULONG value(unsigned field){return ad_get(shared+MB_REPLY,MB_RESULT+field*4);}
static void publish(unsigned offset,ULONG n){ad_put(shared,offset,n);syncio->push(shared+offset,4,NULL);syncio->barrier(NULL);}
static ULONG receive(unsigned offset){syncio->pull(shared+offset,4,NULL);return ad_get(shared,offset);}
static int input(const UBYTE *p,unsigned n){
 return db_stream_input(shared,syncio,p,n);
}
static ULONG received,hash,mismatch;
static int drain(struct DbWindow *console){
 UBYTE data[DB_TX_SIZE];unsigned i;
 int n=db_stream_peek(shared,syncio,data);
 if(n<0)return 0;
 for(i=0;i<(unsigned)n;i++){
  UBYTE c=data[i];
  if(received<sizeof(transcript)-1)transcript[received]=c;
  received++;hash=(hash^c)*16777619u;
 }
 if(n&&console&&!db_window_write(console,data,n))return 0;
 db_stream_consume(shared,syncio,(unsigned)n);return 1;
}
#ifdef DB_VIDEO_TRACE
#include "video_trace_host.h"
#include "video_debug_host.h"
#endif
#ifdef DB_MEMORY_DEBUG
#include "memory_host.h"
#endif
/* Bounded session I/O timing: saturated totals rather than wrap after 71 min. */
static ULONG fs_read_us,fs_write_us,fs_other_us,fs_max_us,fs_timed_calls;
static ULONG add_us(ULONG a,ULONG b){return b>~(ULONG)0-a?~(ULONG)0:a+b;}
static int timed_fs_service(void){
 ULONG op,start,elapsed;int ok;
 /* Requests arriving after this check are serviced on the next poll.
  * Only service inside the timed region, so no transfers escape accounting. */
 if(receive(FS_CMD)==receive(FS_REPLY))return 1;
 op=receive(FS_CMD+4);start=us();ok=db_fs_service();elapsed=us()-start;
 if(op==FS_DISK_READ||op==FS_READ)fs_read_us=add_us(fs_read_us,elapsed);
 else if(op==FS_DISK_WRITE||op==FS_WRITE)fs_write_us=add_us(fs_write_us,elapsed);
 else fs_other_us=add_us(fs_other_us,elapsed);
 if(elapsed>fs_max_us){fs_max_us=elapsed;}
 fs_timed_calls++;
 return ok;
}
static int run(struct DbWindow *console){
 ULONG start=us(),stop_time=0;int sent=0,stopping=0,forced=0,transport_ok=1;
 fs_read_us=fs_write_us=fs_other_us=fs_max_us=fs_timed_calls=0;
 ULONG token=(sequence+1)^ad_get(shared,ZZ_CONTROL);UBYTE keys[64];
#ifdef DB_VIDEO_TRACE
 trace_seen=0;puts("VIDEO_TRACE diagnostic build; timings are not benchmarks");fflush(stdout);
#endif
 memset(transcript,0,sizeof(transcript));received=mismatch=0;hash=2166136261u;
 /* Deliberately cross uint32 wrap in automated tests. */
 publish(DB_RX_WRITE,testmode?0xfffffffcu:0);publish(DB_RX_READ,testmode?0xfffffffcu:0);
 publish(DB_TX_WRITE,testmode?0xfffffffcu:0);publish(DB_TX_READ,testmode?0xfffffffcu:0);publish(DB_RX_EOF,0);
 publish(MB_HOST+8,token);publish(MB_HOST+MB_OP,MB_VERIFY);publish(MB_HOST+MB_SEQ,++sequence);
 for(;;){
  ULONG ack,error;int want_stop=0,midi_result;
#ifdef DB_MEMORY_DEBUG
  memory_host_service(stopping);if(memory_stop_requested)want_stop=1;
#endif
#ifdef DB_VIDEO_TRACE
  if(trace_enabled&&!pc_display_locked()&&!pc_display_pending())video_trace_poll();
#endif
  if(receive(PC_CONSOLE_WAIT)&&!pc_display_locked()&&!pc_display_pending())pc_display_console(1);
  if(!stopping){int display_result=pc_display_service();if(display_result<=0){if(!display_result)transport_ok=0;want_stop=1;}}
#ifdef DB_VIDEO_TRACE
  {int debug_result=video_debug_service(stopping);
   if(debug_result<=0){want_stop=1;if(debug_result<0)transport_ok=0;}}
#endif
  if(!(midi_result=db_midi_host_service())){transport_ok=0;want_stop=1;}
#ifdef DB_NETWORK
  if(!db_net_host_service()){transport_ok=0;want_stop=1;}
  if(dn_control_poll(0))want_stop=1;
#endif
  if(!timed_fs_service()){transport_ok=0;want_stop=1;}
  if(!drain(pc_display_locked()?NULL:console)){transport_ok=0;want_stop=1;}
  if((testmode==1||auto_command[0])&&received>=7&&!sent){
   const UBYTE *testkeys=(const UBYTE *)(auto_command[0]?auto_command:commands);
   if(!input(testkeys,strlen((const char *)testkeys))){transport_ok=0;want_stop=1;}sent=1;
  }
  if(console&&!testmode&&!stopping&&!pc_display_locked()){
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
  if(ack==sequence&&(midi_result!=2||stopping||!transport_ok)&&(!pc_display_pending()||stopping||!transport_ok)){
   int ok=transport_ok&&ad_get(shared+MB_REPLY,68)==(token^ZZ_XOR);
#ifdef DB_VIDEO_TRACE
   if(trace_enabled&&!pc_display_locked()&&!pc_display_pending())video_trace_poll();
#endif
   if(!drain(pc_display_locked()?NULL:console))ok=0;
   if(*boot_name||*hdd_name){syncio->pull(shared+PC_BOOT_TEXT,2004,NULL);if(ad_get(shared,PC_BOOT_TEXT+2000)==0x424f4f54){unsigned row,col;char text[81];puts("BOOT TEXT:");for(row=0;row<25;row++){for(col=0;col<80;col++)text[col]=shared[PC_BOOT_TEXT+row*80+col];text[80]=0;puts(text);}}}
   if(error&&error!=2)printf("DOS error: %.*s\n",159,(const char *)(shared+MB_REPLY+128));
#ifdef DB_OPENGL
   {unsigned o=PC_VIDEO+0xde0u;syncio->pull(shared+o,28,NULL);printf("OPENGL magic=%08x frames=%u ticks_hi=%u ticks_lo=%u closed=%u direct=%u fallback=%u\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24));}
#endif
   printf("VIDEO adapter=%u source=%ux%u monochrome=%u\n",settings.machine,receive(PC_VIDEO+4),receive(PC_VIDEO+8),monochrome);
   {unsigned o=PC_VIDEO+0xd40;syncio->pull(shared+o,28,NULL);printf("JIT magic=%08x commits=%u witness=%u dispatches=%u cache_bytes=%u heap_peak=%u paged_fallbacks=%u\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24));}
   {unsigned o=PC_VIDEO+0xd60;syncio->pull(shared+o,28,NULL);printf("NEON magic=%08x witness=%u context_equal=%u cpacr=%08x fpexc=%08x mvfr0=%08x mvfr1=%08x\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24));ok=ok&&ad_get(shared,o)==0x4e454f31&&ad_get(shared,o+4)==42&&ad_get(shared,o+8)==1;}
   {unsigned o=PC_VIDEO+0xd80;syncio->pull(shared+o,24,NULL);printf("SCANLINES magic=%08x frames=%u rendered_rows=%u cleared_rows=%u skipped_rows=%u nonce_repairs=%u\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20));}
   printf("CPU_CYCLES applied=%u\n",receive(PC_CYCLE_APPLIED));
#ifdef DB_PRESENT_TEST
   {unsigned o=PC_PRESENT_REPORT;syncio->pull(shared+o,PC_PRESENT_SIZE,NULL);
    printf("PRESENT magic=%08x source=%u shown=%u skipped=%u divisor=%u changes=%u mode=%u hz=%u work_ticks=%u copy_ticks=%u total_copy=%08x%08x\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24),ad_get(shared,o+28),ad_get(shared,o+32),ad_get(shared,o+36),ad_get(shared,o+40),ad_get(shared,o+44));}
#endif
   printf("ARM_FPS tenths=%u\n",receive(PC_FPS_TENTHS));
   printf("PC frames=%u emulated_ms=%u wall_ms=%u timed_done=%u\n",value(9),value(10),(ULONG)(us()-start)/1000u,receive(PC_DONE));
   if(profiling){
    static const char *names[]={"other","cpu","events","capture","grant","copy"};unsigned i;
    volatile uint8_t *p=shared+PC_PROFILE;syncio->pull(p,PC_PROFILE_SIZE,NULL);
    if(ad_get(p,4)!=PC_PROFILE_MAGIC||ad_get(p,20)){puts("PROFILE invalid");ok=0;}
    else{
     printf("PROFILE total=%08x%08x unwound=%u\n",ad_get(p,8),ad_get(p,12),ad_get(p,16));
     for(i=0;i<PP_COUNT;i++)printf("PROFILE bucket=%s ticks=%08x%08x calls=%u\n",names[i],ad_get(p,24+i*16),ad_get(p,28+i*16),ad_get(p,32+i*16));
    }
   }
   printf("POLL ms=%u calls=%u wait_us=%u max_us=%u\n",poll_ms,wait_calls,wait_us,wait_max);
   printf("SHELL result=%u guest_exit=%u slices=%u bytes=%u hash=%08x mismatches=%u\n",error,value(7),value(2),received,hash,mismatch);
   if(testmode==1)ok=ok&&!error&&strstr(transcript,"ZZDOS files/handles/seek PASS")&&strstr(transcript,"ZZDOS MZ relocation/memory PASS")&&strstr(transcript,"saved text\r\n")&&strstr(transcript,"RENAMED.BIN");
   else if(testmode>=2)ok=ok&&forced&&stopping&&error==2;
   else ok=ok&&(!error||((stopping||receive(PC_DONE))&&error==2));
   printf("%s\nSHELL %s mode=%d\n",transcript,ok?"PASS":"FAIL",testmode);fflush(stdout);return ok?0:20;
  }
  if(testmode==2&&received>=7&&(ULONG)(us()-start)>250000u){want_stop=1;forced=1;}
  if(SetSignal(0,SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C)want_stop=1;
  if(duration&&(testmode||auto_command[0]||boot_name[0]||hdd_name[0])
#ifdef DB_VIDEO_TRACE
     &&!break_phase
#endif
     &&(ULONG)(us()-start)>wall_limit_us){want_stop=1;transport_ok=0;}
  if(want_stop&&!stopping){stopping=1;stop_time=us();publish(ZZ_STOP,1);}
  if(stopping&&(ULONG)(us()-stop_time)>3000000u){printf("Console stop timeout fs_seq=%u fs_ack=%u video_req=%u video_done=%u\n",receive(FS_CMD),receive(FS_REPLY),receive(PC_VIDEO),receive(PC_VIDEO+36));return 20;}
  if(!poll_wait()){
   puts("Poll timer failed");transport_ok=0;
   if(!stopping){stopping=1;stop_time=us();publish(ZZ_STOP,1);}
   Delay(1);
  }
 }
}
int ff_window(volatile uint8_t *base,const struct ad_io *io,int connected){
 struct MsgPort *port=NULL;struct timerequest *timer=NULL;int opened=0,rc=20;struct DbWindow *console=NULL;
 (void)connected;shared=base;syncio=io;
 if(!db_fs_open(base,io,drive)){puts("Cannot open DOS drive drawer");goto done;}
 port=CreateMsgPort();if(!port)goto done;
 timer=(struct timerequest *)CreateIORequest(port,sizeof(*timer));if(!timer)goto done;
 if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest *)timer,0))goto done;
 opened=1;TimerBase=timer->tr_node.io_Device;
 poll_timer=timer;wait_calls=wait_us=wait_max=0;
 {unsigned j;for(j=0;j<FS_NAME_SIZE;j++)shared[PC_BOOT_NAME+j]=boot_name[j];syncio->push(shared+PC_BOOT_NAME,FS_NAME_SIZE,NULL);}
 {unsigned j;for(j=0;j<FS_NAME_SIZE;j++)shared[PC_HDD_NAME+j]=hdd_name[j];syncio->push(shared+PC_HDD_NAME,FS_NAME_SIZE,NULL);}
 publish(PC_BOOT_TEXT+2000,0);
 publish(PC_UI_FLAGS,(testmode?0:2)|(settings.fps?1:0));publish(PC_FPS_TENTHS,0);
 {static const unsigned ansi_to_vga[]={0,4,2,6,1,5,3,7};publish(PC_TEXT_COLOURS,ansi_to_vga[settings.fg]|(ansi_to_vga[settings.bg]<<4));}
 {unsigned j;for(j=0;j<160;j++)shared[PC_BOOT_COMMAND+j]=settings.system?settings.command[j]:0;syncio->push(shared+PC_BOOT_COMMAND,160,NULL);}
 pc_display_mode(settings.display);pc_display_size(settings.size);
 printf("DISPLAY_CONFIG mode=%s fps=%u system=%s\n",settings.display?"fullscreen":"window",settings.fps,(*hdd_name||*boot_name)?"booted DOS":"embedded DOS 5");
 publish(PC_OPTIONS,0);publish(PC_KEY_WRITE,0);publish(PC_KEY_READ,0);publish(PC_MOUSE_WRITE,0);publish(PC_MOUSE_READ,0);publish(PC_CONSOLE_WAIT,0);
 if(configured){publish(PC_OPTIONS+4,settings.memory);publish(PC_OPTIONS+8,settings.machine);publish(PC_OPTIONS+12,settings.cycles);publish(PC_OPTIONS+16,monochrome);publish(PC_OPTIONS,PC_OPTIONS_MAGIC);printf("SETTINGS memory_mb=%u machine=%u cycles=%u live=%u\n",settings.memory,settings.machine,settings.cycles,settings.live);}
 publish(PC_CYCLE_REQUEST,configured?settings.cycles:3000);publish(PC_CYCLE_APPLIED,0);
 publish(PC_DURATION,duration);publish(PC_DONE,0);
 publish(PC_PROFILE,profiling);publish(PC_PROFILE+4,0);
#ifdef DB_VIDEO_TRACE
 publish(PC_TRACE,0);publish(PC_DEV_TRACE,trace_enabled);
 publish(PC_DEV_PHASE,break_phase);publish(PC_DEV_FRAME,break_frame);
 publish(PC_DEV_RESUME,0);publish(PC_DEV_PAUSED,0);
#endif
#ifdef DB_PRESENT_TEST
 publish(PC_PRESENT_MODE,present_mode);publish(PC_PRESENT_HZ,present_hz);publish(PC_PRESENT_REPORT,0);
#endif
 publish(PC_VIDEO+944,0);
 if(settings.cd_mode){ULONG count=db_cd_host_open(settings.cd_mode,settings.cd_path);if(!count){puts("Cannot mount CD: select a readable ISO9660 image or mounted CD device");goto done;}publish(PC_VIDEO+944,count);}
 if(!db_midi_host_open(base,io,settings.midi,settings.midi_port)){puts("Cannot connect selected CAMD MIDI output");goto done;}
#ifdef DB_NETWORK
 if(!db_net_host_open(base,io,network_enabled,network_dns)){puts("Cannot open Roadshow network sharing");goto done;}
 if(network_enabled&&!dn_control_open()){puts("Network control port unavailable");goto done;}
 if(network_enabled){struct dn_config c;zz_net_configuration(&c);puts(c.protocol?"NETWORK IPX/UDP client active":"NETWORK NE2000 io=300 irq=3 active (private IPv4 NAT)");}
#endif
 if(!pc_display_open(base,io)){puts("Cannot open PC display");goto done;}
#ifdef DB_MEMORY_DEBUG
 if(!memory_host_open()){puts("Guest memory debug port unavailable");goto done;}
#endif
 rc=run(console);
done:
#ifdef DB_NETWORK
 dn_control_close();db_net_host_close();
#endif
#ifdef DB_MEMORY_DEBUG
 memory_host_close();
#endif
#ifdef DB_VIDEO_TRACE
 video_debug_close();
#endif
 db_midi_host_close();db_cd_host_close();printf("FILES_TIME calls=%u read_us=%u write_us=%u other_us=%u max_us=%u (totals saturate at 4294967295)\n",fs_timed_calls,fs_read_us,fs_write_us,fs_other_us,fs_max_us);
 db_fs_close();
 if(pc_display_locked()){puts("Display lock retained for reset");}
 deferred_console=console;
 poll_timer=NULL;
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
