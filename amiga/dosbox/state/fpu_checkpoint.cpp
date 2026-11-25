/* GPL-2.0-or-later. Portable backend only; fixed LE fields, no host FP state.
 * p_regs is unused by this backend. Its architectural values live in regs.
 * Unlike guest FSAVE, this preserves scratch slot 8 and stale SW.TOP bits:
 * the interpreter synchronizes SW.TOP only when an instruction requests it. */
#include "dosbox.h"
#include "fpu.h"
#include "fpu_checkpoint.h"
#include <stdint.h>
#include <string.h>
#if !C_FPU || C_FPU_X86
#error This checkpoint requires the portable DOSBox x87 backend
#endif
namespace {
enum { words=db_fpu_checkpoint_bytes/4, magic=0x3150465a, schema=1, portable_double=1 };
struct Image { uint32_t v[words]; };
static_assert(sizeof(FPU_Reg)==8,"Unexpected portable FPU register layout");
uint32_t checksum(const Image &x){
 uint32_t c=~0u;
 for(unsigned n=0;n<words;n++)if(n!=3){
  uint32_t v=x.v[n];
  for(unsigned b=0;b<4;b++,v>>=8){
   c^=v&255;for(unsigned bit=0;bit<8;bit++)c=(c>>1)^(0xedb88320u&(0u-(c&1u)));
  }
 }
 return ~c;
}
bool valid(const Image &x){
 if(x.v[0]!=magic||x.v[1]!=schema||x.v[2]!=db_fpu_checkpoint_bytes||
    x.v[3]!=checksum(x)||x.v[4]!=portable_double||x.v[5]>0xffff||
    x.v[6]>0xffff||x.v[7]>7)return false;
 for(unsigned n=0;n<9;n++)if(x.v[10+n*3]>TAG_Empty)return false;
 return true;
}
bool decode(const void *buffer,size_t size,Image &x){
 if(!buffer||size!=db_fpu_checkpoint_bytes)return false;
 const uint8_t *p=(const uint8_t*)buffer;
 for(unsigned n=0;n<words;n++)for(unsigned b=0;b<4;b++)x.v[n]|=uint32_t(p[n*4+b])<<(b*8);
 return valid(x);
}
Image capture(){
 Image x{};x.v[0]=magic;x.v[1]=schema;x.v[2]=db_fpu_checkpoint_bytes;
 x.v[4]=portable_double;x.v[5]=fpu.cw;x.v[6]=fpu.sw;x.v[7]=fpu.top;
 for(unsigned n=0;n<9;n++){
  uint64_t bits;memcpy(&bits,&fpu.regs[n],sizeof(bits));
  x.v[8+n*3]=uint32_t(bits);x.v[9+n*3]=uint32_t(bits>>32);x.v[10+n*3]=fpu.tags[n];
 }
 x.v[3]=checksum(x);return x;
}
void restore(const Image &x){
 FPU_SetCW(x.v[5]);fpu.sw=x.v[6];fpu.top=x.v[7];
 for(unsigned n=0;n<9;n++){
  const uint64_t bits=x.v[8+n*3]|(uint64_t(x.v[9+n*3])<<32);
  memcpy(&fpu.regs[n],&bits,sizeof(bits));fpu.tags[n]=FPU_Tag(x.v[10+n*3]);
 }
}
}
bool db_fpu_checkpoint_save(void *buffer,size_t size){
 if(!buffer||size!=db_fpu_checkpoint_bytes)return false;
 Image x=capture();if(!valid(x))return false;
 uint8_t *p=(uint8_t*)buffer;
 for(unsigned n=0;n<words;n++)for(unsigned b=0;b<4;b++)p[n*4+b]=uint8_t(x.v[n]>>(b*8));
 return true;
}
bool db_fpu_checkpoint_validate(const void *buffer,size_t size){
 Image x{};return decode(buffer,size,x);
}
bool db_fpu_checkpoint_load(const void *buffer,size_t size){
 Image x{};if(!decode(buffer,size,x))return false;
 restore(x);return true;
}
