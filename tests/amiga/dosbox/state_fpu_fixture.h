/* GPL-2.0-or-later. Shared pinned-interpreter x87 continuation fixture. */
#pragma once
#include "../../../amiga/dosbox/adapter.cpp"
#include "fpu.h"
#include "state/cpu_checkpoint.h"
#include "state/fpu_checkpoint.h"
#include "state/processor_checkpoint.h"
#include "fpu-program.h"
extern void FPU_Init(Section*);
static void initialise(){
 memset(ram,0,sizeof(ram));memset(&cpu_regs,0,sizeof(cpu_regs));
 memset(&cpu,0,sizeof(cpu));memset(&Segs,0,sizeof(Segs));memset(&lflags,0,sizeof(lflags));
 memset(&fpu,0,sizeof(fpu));FPU_Init(nullptr);
 zz_dosbox_paging_init();CPU_ArchitectureType=CPU_ARCHTYPE_386SLOW;
 CPU_Core_Normal_Init();cpudecoder=CPU_Core_Normal_Run;
 for(unsigned seg=0;seg<6;seg++)SegSet16(seg,0x1000);
 cpu.stack.mask=0xffff;cpu.stack.notmask=0xffff0000;
 cpu.idt.SetBase(0);cpu.idt.SetLimit(1023);CPU_SetFlags(FLAG_IF,FMASK_ALL);
 reg_sp=0xfffe;reg_eip=0x100;CPU_CycleLeft=0;CPU_Cycles=0;
 memcpy(ram+0x10100,program,sizeof(program));
 for(unsigned n=0;n<4;n++)mem_writew(0x18000+n*2,0x37f|(n<<10));
 double a=1.5,b=-2.5;memcpy(ram+0x18010,&a,8);memcpy(ram+0x18018,&b,8);
}
static void step(unsigned count){
 for(unsigned n=0;n<count;n++){CPU_Cycles=1;assert(CPU_Core_Normal_Run()==0);}
}
