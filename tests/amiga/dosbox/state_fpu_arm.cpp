/* GPL-2.0-or-later. Native expected bytes and actual ARM backend continuation. */
#include "dosbox.h"
#include "fpu.h"
#include "state/fpu_checkpoint.h"
#include <stdint.h>
#include <string.h>
#ifdef FPU_GOLDEN
#include <stdio.h>
#include <stdlib.h>
void E_Exit(const char *,...){abort();}
#else
#include "fpu-golden.h"
void E_Exit(const char *,...){for(;;){}}
#endif
static void continuation(){
 FPU_ESC1_Normal(0xfc); // FRNDINT ST0
 FPU_ESC0_Normal(0xc1); // FADD ST0,ST1
 FPU_ESC1_Normal(0xe0); // FCHS
 FPU_ESC1_Normal(0xfa); // FSQRT
}
int main(){
#ifdef FPU_GOLDEN
 uint8_t wire[db_fpu_checkpoint_bytes];
 for(unsigned mode=0;mode<4;mode++){
  memset(&fpu,0,sizeof(fpu));FPU_SetCW(0x37f|(mode<<10));
  fpu.top=3;fpu.sw=0x4000;for(unsigned n=0;n<9;n++)fpu.tags[n]=TAG_Empty;
  fpu.tags[3]=fpu.tags[4]=fpu.tags[8]=TAG_Valid;fpu.regs[3].d=1.5;fpu.regs[4].d=-2.5;
  for(unsigned phase=0;phase<2;phase++){
   if(!db_fpu_checkpoint_save(wire,sizeof(wire)))return 1;
   if(fwrite(wire,1,sizeof(wire),stdout)!=sizeof(wire))return 2;
   if(!phase)continuation();
  }
 }
#else
 uint8_t actual[db_fpu_checkpoint_bytes],bad[db_fpu_checkpoint_bytes];
 for(unsigned mode=0;mode<4;mode++){
  const uint8_t *before=golden+mode*2*db_fpu_checkpoint_bytes,*after=before+db_fpu_checkpoint_bytes;
  memset(&fpu,0xa5,sizeof(fpu));
  if(!db_fpu_checkpoint_load(before,db_fpu_checkpoint_bytes))return 1;
  continuation();
  if(!db_fpu_checkpoint_save(actual,sizeof(actual))||memcmp(actual,after,sizeof(actual)))return 2;
  for(unsigned byte=0;byte<sizeof(bad);byte++){
   memcpy(bad,before,sizeof(bad));bad[byte]^=0x80;
   if(db_fpu_checkpoint_load(bad,sizeof(bad)))return 3;
   if(!db_fpu_checkpoint_save(actual,sizeof(actual))||memcmp(actual,after,sizeof(actual)))return 4;
  }
 }
#endif
 return 0;
}
