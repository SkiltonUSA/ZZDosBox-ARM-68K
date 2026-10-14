/* GPL-2.0-or-later. Frontend for upstream DOSBox CPU, DOS, BIOS and EGA/VGA.
 * Sound and serial devices are not connected in this milestone. */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include "dosbox.h"
#include "cpu.h"
#include "paging.h"
#include "callback.h"
#include "pic.h"
#include "lazyflags.h"
#include "adapter.h"
#include "console_io.h"
#include "shell_api.h"
#include "pc.h"
#include "timer.h"
#include "bios.h"
#include "int10.h"
#include "profile.h"
#include "adapters.h"
#ifndef DB_NATIVE
#include "pc_wire.h"
extern "C" volatile uint8_t *db_shared();
static void console_wait(int on){ad_put(db_shared(),PC_CONSOLE_WAIT,on);__asm__ volatile("dsb sy":::"memory");}
#else
static void console_wait(int){}
#endif
static uint8_t ram[8*1024*1024];
unsigned db_memory_mb=2,db_machine_option=0,db_cpu_cycles=3000,db_monochrome=0;
bool db_vga_console=false;unsigned db_text_attr=7;
static unsigned ansi_bytes;static int extended_key=-1;
HostPt MemBase=ram;
MachineType machine=MCH_VGA;
SVGACards svgaCard=SVGA_None;
bool SDLNetInited=false;
Bit32u ticksDone=0,ticksScheduled=0;
Bitu cycle_count=0;


static jmp_buf escape;
static DosboxResult *result;
static bool finished,finishing;
bool db_shell_finishing(){return finishing;}
static int (*stop_fn)();
static const DbConsoleIO *console_io;
static void cancelled(){result->fault=3;longjmp(escape,2);}
static void check_stop(){if(stop_fn&&stop_fn())cancelled();}
void db_shell_poll(){check_stop();}
void E_Exit(const char *msg,...) { result->fault=1; va_list a;va_start(a,msg);vsnprintf(result->text,sizeof(result->text),msg,a);va_end(a);longjmp(escape,1); }
void GFX_ShowMsg(const char *,...) {}
void GFX_SetTitle(Bit32s,int,bool) {}

void CPU_Reset_AutoAdjust();
HostPt GetMemBase(){return ram;}
Bitu MEM_TotalPages(){return db_memory_mb*256;}
class RamPage:public PageHandler {
public:
 RamPage(){flags=PFLAG_READABLE|PFLAG_WRITEABLE;}
 HostPt GetHostReadPt(Bitu page) override {if(page>=MEM_TotalPages())E_Exit("Guest RAM bound");return ram+page*4096;}
 HostPt GetHostWritePt(Bitu page) override {return GetHostReadPt(page);}
};
static RamPage ram_page;
class RomPage:public RamPage {
public:
 RomPage(){flags=PFLAG_READABLE|PFLAG_HASROM;}
 void writeb(PhysPt,Bitu) override {}
 void writew(PhysPt,Bitu) override {}
 void writed(PhysPt,Bitu) override {}
};
static RomPage rom_page;
static PageHandler *handlers[2048];
static Bitu lfb_page,lfb_pages;
static PageHandler *lfb_handler,*lfb_mmio;
PageHandler *MEM_GetPageHandler(Bitu page){
 if(page>=2048&&lfb_pages){
  if(page>=lfb_page&&page-lfb_page<lfb_pages)return lfb_handler;
  if(page>=lfb_page+4096&&page-(lfb_page+4096)<16)return lfb_mmio;
 }
 if(page>=2048)E_Exit("Guest page bound %x at %x:%x ss:sp=%x:%x",(unsigned)page,SegValue(cs),reg_eip,SegValue(ss),reg_esp);return handlers[page]?handlers[page]:&ram_page;}
void MEM_SetPageHandler(Bitu page,Bitu pages,PageHandler *h){if(page>2048||pages>2048-page)E_Exit("Page handler bound");for(Bitu i=0;i<pages;i++)handlers[page+i]=h;PAGING_ClearTLB();}
void MEM_ResetPageHandler(Bitu p,Bitu n){MEM_SetPageHandler(p,n,nullptr);}
void MEM_SetLFB(Bitu page,Bitu pages,PageHandler*h,PageHandler*mmio){
 if(pages>PC_S3_VRAM_BYTES/4096||page>0x100000u-4096u-16u||!h||!mmio)E_Exit("SVGA mapping bound");
 lfb_page=page;lfb_pages=pages;lfb_handler=h;lfb_mmio=mmio;PAGING_ClearTLB();
}
static bool a20;
bool MEM_A20_Enabled(){return a20;}
void MEM_A20_Enable(bool on){a20=on;for(unsigned i=0;i<16;i++)PAGING_MapPage(0x100+i,on?0x100+i:i);}
Bits CPU_Core_Simple_Run(){E_Exit("Unexpected simple decoder");}
#include "dos_inc.h"
#include "drive.h"
static unsigned slice_limit;
void DOSBOX_RunMachine(){
 for(;;){
  check_stop();if(result->slices++>=slice_limit&&slice_limit!=0xffffffffu)E_Exit("Instruction budget exceeded");
  pc_prof_enter(PP_EVENTS);
  bool queued=PIC_RunQueue();
  if(!queued){TIMER_AddTick();db_pc_tick();}
  pc_prof_leave();if(!queued)continue;
  pc_prof_enter(PP_CPU);Bits n=(*cpudecoder)();pc_prof_leave();
  // The upstream page-fault decoder returns -1 when its nested fault has
  // completed. Unwind only this RunMachine invocation so paging can resume.
  if(n<0)return;
  if(n){if((Bitu)n>=CB_MAX||!CallBack_Handlers[n])E_Exit("Invalid callback");if(CallBack_Handlers[n]()==CBRET_STOP)return;}
 }
}
void DOSBOX_SetLoop(LoopHandler*){E_Exit("Unexpected loop change");}
void DOSBOX_SetNormalLoop(){}
extern void zz_dos_init();
static void vga_text(const uint8_t *p,unsigned n){
 for(unsigned i=0;i<n;i++){
  uint8_t c=p[i];
  if(ansi_bytes||c==27){
   if(!ansi_bytes){ansi_bytes=1;continue;}
   if((ansi_bytes==1&&c!='[')||(ansi_bytes>1&&c>=0x40&&c<=0x7e)||++ansi_bytes>=32)ansi_bytes=0;
   continue;
  }
  INT10_TeletypeOutputAttr(c,db_text_attr,true);
 }
}
static int bios_ready(){return extended_key>=0||mem_readw(BIOS_KEYBOARD_BUFFER_HEAD)!=mem_readw(BIOS_KEYBOARD_BUFFER_TAIL);}
static int bios_read(){
 if(extended_key>=0){int c=extended_key;extended_key=-1;return c;}
 unsigned head=mem_readw(BIOS_KEYBOARD_BUFFER_HEAD),tail=mem_readw(BIOS_KEYBOARD_BUFFER_TAIL);
 if(head==tail)return -1;
 unsigned start=mem_readw(BIOS_KEYBOARD_BUFFER_START),end=mem_readw(BIOS_KEYBOARD_BUFFER_END);
 if(start<0x1e||end>0x100||start>=end||head<start||head>=end||(head&1))E_Exit("BIOS keyboard bounds");
 unsigned key=real_readw(0x40,head);head+=2;if(head>=end)head=start;mem_writew(BIOS_KEYBOARD_BUFFER_HEAD,head);
 if(!(key&255))extended_key=(key>>8)&255;
 return key&255;
}
void db_console(const uint8_t *data,unsigned size){
 if(db_vga_console)vga_text(data,size);
 if(console_io){
  check_stop();int rc=console_io->write(console_io->opaque,data,size);
  if(rc<0)cancelled();if(!rc)E_Exit("Console transport failed");
 }
 for(unsigned i=0;i<size;i++){
  if(result->bytes+1<sizeof(result->text))result->text[result->bytes]=data[i];
  else if(!console_io)E_Exit("Console output bound");
  result->bytes++;
 }
 result->text[result->bytes<sizeof(result->text)?result->bytes:sizeof(result->text)-1]=0;
}
int db_console_ready(){return (db_vga_console&&bios_ready())||(console_io&&console_io->ready(console_io->opaque));}
unsigned db_console_read(uint8_t *data,unsigned size,bool echo){
 unsigned used=0;if(!console_io)return 0;
 while(used<size){
  check_stop();console_wait(!db_vga_console&&!console_io->ready(console_io->opaque));
  int c=console_io->read(console_io->opaque);
  if(c==-1&&db_vga_console){c=bios_read();if(c==-1)CALLBACK_Idle();}
  if(c==-1)continue;if(c==-2)break;
  console_wait(0);
  if(c<0||c>255)E_Exit("Console input failed");
  if(c==8&&size>1){
   if(used){used--;const uint8_t bs[]={8,32,8};db_console(bs,3);}continue;
  }
  data[used++]=(uint8_t)c;
  if(echo)db_console(data+used-1,1);
  if(c==13){if(used<size)data[used++]=10;if(echo){const uint8_t lf=10;db_console(&lf,1);}break;}
 }
 return used;
}
static Bitu returned(){if(GETFLAG(CF))E_Exit("DOS EXEC failed %u",reg_ax);result->exit_code=dos.return_code;finished=true;return CBRET_STOP;}
extern void zz_dosbox_paging_init();
extern void CPU_Core_Normal_Init();
extern void CALLBACK_Init(Section*);
extern void db_cd_setup(),db_cd_mount(),db_cd_close();
#ifdef DB_NEON
extern "C" void db_neon_begin(),db_neon_finish();
#endif
#ifdef DB_DYNREC
extern void CPU_Core_Dynrec_Cache_Init(bool),CPU_Core_Dynrec_Cache_Close();
extern "C" void db_dynrec_preflight(),db_dynrec_report();
#endif
static int run_guest(const uint8_t *program,unsigned bytes,unsigned limit,DosboxResult *out,int (*stop)()){
 result=out;memset(out,0,sizeof(*out));finished=false;stop_fn=stop;
 if(bytes>0xff00||!limit){out->fault=2;return 1;}
 slice_limit=limit;
 int jumped=setjmp(escape);if(jumped)return jumped==2?2:1;
#ifdef DB_NEON
 db_neon_begin();
#endif
 if(db_machine_option>=PC_ADAPTER_COUNT)E_Exit("Unknown video adapter");
 const MachineType adapters[]={MCH_VGA,MCH_EGA,MCH_CGA,MCH_VGA,MCH_TANDY,MCH_PCJR,MCH_HERC};
 machine=adapters[db_machine_option];svgaCard=db_machine_option==PC_ADAPTER_S3?SVGA_S3Trio:SVGA_None;
 lfb_page=lfb_pages=0;lfb_handler=lfb_mmio=nullptr;
 memset(ram,0,sizeof(ram));memset(&cpu_regs,0,sizeof(cpu_regs));memset(&cpu,0,sizeof(cpu));memset(&Segs,0,sizeof(Segs));
 zz_dosbox_paging_init();CPU_ArchitectureType=CPU_ARCHTYPE_386SLOW;CPU_Core_Normal_Init();cpudecoder=CPU_Core_Normal_Run;
 for(unsigned seg=0;seg<6;seg++)SegSet16(seg,0x1000);
 cpu.stack.mask=0xffff;cpu.stack.notmask=0xffff0000;cpu.idt.SetBase(0);cpu.idt.SetLimit(1023);CPU_SetFlags(FLAG_IF,FMASK_ALL);
 reg_sp=0xfffe;reg_eip=0x100;
 memset(handlers,0,sizeof(handlers));a20=false;
 // Preserve the PC BIOS and VGA ROM against guest RAM probes.
 for(unsigned page=0xc0;page<0xc8;page++)handlers[page]=&rom_page;
 for(unsigned page=0xf0;page<0x100;page++)handlers[page]=&rom_page;
#ifdef DB_DYNREC
 db_dynrec_preflight();CPU_Core_Dynrec_Cache_Init(true);cpudecoder=CPU_Core_Dynrec_Run;
#endif
 CALLBACK_Init(nullptr);db_pc_init();db_cd_setup();ansi_bytes=0;extended_key=-1;
 if(db_vga_console)INT10_ScrollWindow(0,0,24,79,0,db_text_attr,0);
 if(*db_boot_name||*db_hdd_name){db_boot_start();DOSBOX_RunMachine();return 0;}
 zz_dos_init();dos.echo=false;db_mount(program,bytes);db_cd_mount();
 // Parent PSP lives in DOSBox's reserved shell region. EXEC is invoked
 // through the real INT21 entry, so its RETF/IRET and parent restoration run.
 const Bit16u pspseg=DOS_FIRST_SHELL;
 dos.psp(pspseg);DOS_PSP parent(pspseg);parent.MakeNew(0);parent.SetParent(pspseg);parent.SetEnvironment(0);
 dos.dta(RealMake(pspseg,0x80));db_stdio(parent);
 Bit16u scratch=DOS_GetMemory(256);SegSet16(ss,scratch);reg_sp=4094;
 SegSet16(ds,scratch);SegSet16(es,scratch);SegSet16(cs,scratch);reg_ip=0;
 if(!program){zz_shell_run();return 0;}
 Bitu cb=CALLBACK_Allocate();CALLBACK_Setup(cb,returned,CB_IRET,"host return");
 RealPt ret=CALLBACK_RealPointer(cb);
 const uint8_t boot[]={0xb8,0x00,0x4b,0xba,0x00,0x01,0xbb,0x00,0x02,0xcd,0x21,0xea,
  (uint8_t)ret,(uint8_t)(ret>>8),(uint8_t)(ret>>16),(uint8_t)(ret>>24)};
 MEM_BlockWrite(PhysMake(scratch,0),boot,sizeof(boot));
 const char name[]="C:\\TEST.COM";MEM_BlockWrite(PhysMake(scratch,0x100),name,sizeof(name));
 DOS_ParamBlock block(PhysMake(scratch,0x200));block.Clear();block.exec.cmdtail=RealMake(pspseg,0x80);block.SaveData();

 while(!finished&&out->slices<limit){
  if(stop_fn&&stop_fn()){out->fault=3;return 2;}
  CPU_Cycles=1000;Bits n=CPU_Core_Normal_Run();out->slices++;
  if(n<0)E_Exit("Decoder error");
  if(n){if((Bitu)n>=CB_MAX||!CallBack_Handlers[n])E_Exit("Invalid callback");CallBack_Handlers[n]();}
 }
 if(!finished)E_Exit("Instruction budget exceeded");
 out->hash=0;
 return out->exit_code?1:0;
}

extern char *CallBack_Description[CB_MAX];
extern "C" int db_run_io(const uint8_t *program,unsigned bytes,unsigned limit,DosboxResult *out,int (*stop)(),const DbConsoleIO *io){
 if(!out)return 1;
 if(io&&(!io->read||!io->ready||!io->write))return 1;
 console_io=io;finishing=false;
 int rc=run_guest(program,bytes,limit,out,stop);
 pc_prof_unwind();
#ifdef DB_DYNREC
 CPU_Core_Dynrec_Cache_Close();db_dynrec_report();
#endif
 finishing=true;
 zz_shell_cleanup();
 db_boot_close();
 db_pc_close();
 db_unmount();db_cd_close();
 for(unsigned i=0;i<CB_MAX;i++){delete[] CallBack_Description[i];CallBack_Description[i]=nullptr;}
#ifdef DB_OPENGL
 extern void db_gl_shutdown();db_gl_shutdown();
#endif
#ifdef DB_NEON
 db_neon_finish();
#endif
 result=nullptr;console_io=nullptr;
 return rc;
}

extern "C" int db_run(const uint8_t *program,unsigned bytes,unsigned limit,DosboxResult *out,int (*stop)()){
 return db_run_io(program,bytes,limit,out,stop,nullptr);
}
