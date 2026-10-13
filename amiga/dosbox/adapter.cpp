/* GPL-2.0-or-later. Original host adapter; DOS calls here are diagnostic stubs,
 * not DOSBox's complete DOS kernel. Guest x86 execution uses upstream code. */
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
static uint8_t ram[2*1024*1024];
HostPt MemBase=ram;
MachineType machine=MCH_VGA;
SVGACards svgaCard=SVGA_None;
bool SDLNetInited=false;
Bit32u ticksDone=0,ticksScheduled=0;
Bitu cycle_count=0;
Bitu PIC_Ticks=0;
Bitu PIC_IRQCheck=0;
static jmp_buf escape;
static DosboxResult *result;
static bool finished;
static int (*stop_fn)();
void E_Exit(const char *msg,...) { result->fault=1; va_list a;va_start(a,msg);vsnprintf(result->text,sizeof(result->text),msg,a);va_end(a);longjmp(escape,1); }
void GFX_ShowMsg(const char *,...) {}
void GFX_SetTitle(Bit32s,int,bool) {}
void PIC_RunIRQs() {}
void CPU_Reset_AutoAdjust();
HostPt GetMemBase(){return ram;}
Bitu MEM_TotalPages(){return sizeof(ram)/4096;}
class RamPage:public PageHandler {
public:
 RamPage(){flags=PFLAG_READABLE|PFLAG_WRITEABLE;}
 HostPt GetHostReadPt(Bitu page) override {if(page>=MEM_TotalPages())E_Exit("Guest RAM bound");return ram+page*4096;}
 HostPt GetHostWritePt(Bitu page) override {return GetHostReadPt(page);}
};
static RamPage ram_page;
PageHandler *MEM_GetPageHandler(Bitu page){if(page>=MEM_TotalPages())E_Exit("Guest page bound");return &ram_page;}
Bitu IO_ReadB(Bitu){E_Exit("Unimplemented port read");}
Bitu IO_ReadW(Bitu){E_Exit("Unimplemented port read");}
Bitu IO_ReadD(Bitu){E_Exit("Unimplemented port read");}
void IO_WriteB(Bitu,Bitu){E_Exit("Unimplemented port write");}
void IO_WriteW(Bitu,Bitu){E_Exit("Unimplemented port write");}
void IO_WriteD(Bitu,Bitu){E_Exit("Unimplemented port write");}
Bits CPU_Core_Full_Run(){E_Exit("Unexpected full decoder");}
Bits CPU_Core_Simple_Run(){E_Exit("Unexpected simple decoder");}
static Bitu dos_call(){
 if(reg_ah==9){
  PhysPt p=SegPhys(ds)+reg_dx;unsigned n=0;
  while(n++<65536){if(p>=sizeof(ram))E_Exit("DOS string bound");uint8_t c=ram[p++];if(c=='$')return CBRET_NONE;
   if(result->bytes+1>=sizeof(result->text))E_Exit("DOS output bound");result->text[result->bytes++]=c;result->text[result->bytes]=0;}
  E_Exit("Unterminated DOS string");
 }
 if(reg_ah==0x4c){result->exit_code=reg_al;finished=true;return CBRET_STOP;}
 E_Exit("Unsupported DOS service %02x",reg_ah);
}
void DOSBOX_RunMachine(){E_Exit("Nested run unsupported in diagnostic");}
void DOSBOX_SetLoop(LoopHandler*){E_Exit("Unexpected loop change");}
void DOSBOX_SetNormalLoop(){}
extern void zz_dosbox_paging_init();
extern void CPU_Core_Normal_Init();
extern void CALLBACK_Init(Section*);
static int run_guest(const uint8_t *program,unsigned bytes,unsigned limit,DosboxResult *out,int (*stop)()){
 result=out;memset(out,0,sizeof(*out));finished=false;stop_fn=stop;
 if(!program||bytes>0xff00||!limit){out->fault=2;return 1;}
 if(setjmp(escape))return 1;
 memset(ram,0,sizeof(ram));memset(&cpu_regs,0,sizeof(cpu_regs));memset(&cpu,0,sizeof(cpu));memset(&Segs,0,sizeof(Segs));
 zz_dosbox_paging_init();CPU_ArchitectureType=CPU_ARCHTYPE_386SLOW;CPU_Core_Normal_Init();cpudecoder=CPU_Core_Normal_Run;
 for(unsigned seg=0;seg<6;seg++)SegSet16(seg,0x1000);
 cpu.stack.mask=0xffff;cpu.stack.notmask=0xffff0000;cpu.idt.SetBase(0);cpu.idt.SetLimit(1023);CPU_SetFlags(FLAG_IF,FMASK_ALL);
 reg_sp=0xfffe;reg_eip=0x100;memcpy(ram+0x10100,program,bytes);
 CALLBACK_Init(nullptr);Bitu cb=CALLBACK_Allocate();CALLBACK_Setup(cb,dos_call,CB_IRET,"diagnostic DOS");RealSetVec(0x21,CALLBACK_RealPointer(cb));
 while(!finished&&out->slices<limit){
  if(stop_fn&&stop_fn()){out->fault=3;return 2;}
  CPU_Cycles=1000;Bits n=CPU_Core_Normal_Run();out->slices++;
  if(n<0)E_Exit("Decoder error");
  if(n){if((Bitu)n>=CB_MAX||!CallBack_Handlers[n])E_Exit("Invalid callback");CallBack_Handlers[n]();}
 }
 if(!finished)E_Exit("Instruction budget exceeded");
 out->hash=2166136261u;for(unsigned i=0;i<256;i++)out->hash=(out->hash^ram[0x18000+i])*16777619u;
 return out->exit_code?1:0;
}

extern char *CallBack_Description[CB_MAX];
extern "C" int db_run(const uint8_t *program,unsigned bytes,unsigned limit,DosboxResult *out,int (*stop)()){
 if(!out)return 1;
 int rc=run_guest(program,bytes,limit,out,stop);
 for(unsigned i=0;i<CB_MAX;i++){delete[] CallBack_Description[i];CallBack_Description[i]=nullptr;}
 result=nullptr;
 return rc;
}
