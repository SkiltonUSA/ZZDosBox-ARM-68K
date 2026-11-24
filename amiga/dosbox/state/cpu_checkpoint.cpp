/* GPL-2.0-or-later. Fixed little-endian fields, no host pointers or JIT code.
 * The component list was informed by DOSBox Pure's DBPSerialize_CPU; this
 * restricted implementation is original and does not use its archive format. */
#include "dosbox.h"
#include "cpu.h"
#include "paging.h"
#include "lazyflags.h"
#include "cpu_checkpoint.h"
namespace {
enum { words=70, magic=0x3150435a, schema=1 };
struct Image { uint32_t v[words]; };
bool supported(){
 return cpudecoder==CPU_Core_Normal_Run&&!paging.enabled&&!cpu.pmode&&
  !(cpu.cr0&0x80000001u)&&!cpu.code.big&&!cpu.stack.big&&
  !cpu.trap_skip&&!(reg_flags&(FLAG_VM|FLAG_TF));
}
uint32_t checksum(const Image &image){
 uint32_t crc=~0u;
 for(unsigned n=0;n<words;n++)if(n!=3){
  uint32_t v=image.v[n];
  for(unsigned b=0;b<4;b++,v>>=8){
   crc^=v&255u;
   for(unsigned bit=0;bit<8;bit++)crc=(crc>>1)^(0xedb88320u& (0u-(crc&1u)));
  }
 }
 return ~crc;
}
Image capture(){
 Image x{};x.v[0]=magic;x.v[1]=schema;x.v[2]=db_cpu_checkpoint_bytes;x.v[4]=CPU_ArchitectureType;
 for(unsigned n=0;n<8;n++)x.v[5+n]=cpu_regs.regs[n].dword[0];
 x.v[13]=reg_eip;x.v[14]=reg_flags;
 for(unsigned n=0;n<8;n++){x.v[15+n]=Segs.val[n];x.v[23+n]=Segs.phys[n];}
 x.v[31]=lflags.var1.dword[0];x.v[32]=lflags.var2.dword[0];x.v[33]=lflags.res.dword[0];
 x.v[34]=lflags.type;x.v[35]=lflags.prev_type;x.v[36]=lflags.oldcf;
 x.v[37]=cpu.cpl;x.v[38]=cpu.mpl;x.v[39]=cpu.cr0;
 x.v[40]=cpu.gdt.GetBase();x.v[41]=cpu.gdt.GetLimit();x.v[42]=cpu.idt.GetBase();x.v[43]=cpu.idt.GetLimit();
 x.v[44]=cpu.stack.mask;x.v[45]=cpu.stack.notmask;x.v[46]=cpu.stack.big;x.v[47]=cpu.code.big;
 x.v[48]=cpu.exception.which;x.v[49]=cpu.exception.error;x.v[50]=cpu.direction;x.v[51]=cpu.trap_skip;
 for(unsigned n=0;n<8;n++){x.v[52+n]=cpu.drx[n];x.v[60+n]=cpu.trx[n];}
 x.v[68]=CPU_Cycles;x.v[69]=CPU_CycleLeft;x.v[3]=checksum(x);return x;
}
bool valid(const Image &x){
 for(unsigned n=0;n<8;n++)if(x.v[15+n]>0xffffu)return false;
 return x.v[0]==magic&&x.v[1]==schema&&x.v[2]==db_cpu_checkpoint_bytes&&x.v[3]==checksum(x)&&
  x.v[4]==unsigned(CPU_ArchitectureType)&&!(x.v[14]&(FLAG_VM|FLAG_TF))&&
  x.v[34]<t_LASTFLAG&&x.v[35]<t_LASTFLAG&&x.v[36]<=1&&x.v[37]<=3&&x.v[38]<=3&&
  !(x.v[39]&0x80000001u)&&x.v[41]<=0xffffu&&x.v[43]<=0xffffu&&
  x.v[44]==0xffffu&&x.v[45]==0xffff0000u&&
  x.v[50]==((x.v[14]&FLAG_DF)?0xffffffffu:1u)&&x.v[46]==0&&x.v[47]==0&&x.v[51]==0;
}
void restore(const Image &x){
 for(unsigned n=0;n<8;n++)cpu_regs.regs[n].dword[0]=x.v[5+n];
 reg_eip=x.v[13];reg_flags=x.v[14];
 for(unsigned n=0;n<8;n++){Segs.val[n]=x.v[15+n];Segs.phys[n]=x.v[23+n];}
 lflags.var1.dword[0]=x.v[31];lflags.var2.dword[0]=x.v[32];lflags.res.dword[0]=x.v[33];
 lflags.type=x.v[34];lflags.prev_type=x.v[35];lflags.oldcf=x.v[36];
 cpu.cpl=x.v[37];cpu.mpl=x.v[38];cpu.cr0=x.v[39];
 cpu.gdt.SetBase(x.v[40]);cpu.gdt.SetLimit(x.v[41]);cpu.idt.SetBase(x.v[42]);cpu.idt.SetLimit(x.v[43]);
 cpu.stack.mask=x.v[44];cpu.stack.notmask=x.v[45];cpu.stack.big=false;cpu.code.big=false;
 cpu.exception.which=x.v[48];cpu.exception.error=x.v[49];cpu.direction=(int32_t)x.v[50];cpu.trap_skip=false;
 for(unsigned n=0;n<8;n++){cpu.drx[n]=x.v[52+n];cpu.trx[n]=x.v[60+n];}
 CPU_Cycles=(int32_t)x.v[68];CPU_CycleLeft=(int32_t)x.v[69];PAGING_ClearTLB();
}
}
bool db_cpu_checkpoint_save(void *buffer,size_t size){
 if(!buffer||size!=db_cpu_checkpoint_bytes||!supported())return false;
 Image x=capture();if(!valid(x))return false;
 uint8_t *p=(uint8_t*)buffer;
 for(unsigned n=0;n<words;n++)for(unsigned b=0;b<4;b++)p[n*4+b]=(uint8_t)(x.v[n]>>(b*8));
 return true;
}
bool db_cpu_checkpoint_load(const void *buffer,size_t size){
 if(!buffer||size!=db_cpu_checkpoint_bytes||!supported())return false;
 const uint8_t *p=(const uint8_t*)buffer;Image x{};
 for(unsigned n=0;n<words;n++)for(unsigned b=0;b<4;b++)x.v[n]|=uint32_t(p[n*4+b])<<(b*8);
 if(!valid(x))return false;restore(x);return true;
}
