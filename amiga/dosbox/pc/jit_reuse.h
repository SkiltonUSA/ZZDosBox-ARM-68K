/* GPL-2.0-or-later. Forward an immediately preceding CPU-register STR.
 * Only unconditional word accesses through the resident r10 base qualify.
 * The caller must exclude block entries and branch targets. */
#pragma once
#include <stdint.h>
static inline int db_arm_stored_register(uint32_t previous,uint32_t load){
 if((load&0xff7f0000u)!=0xe51a0000u)return -1;
 if((previous&0xffff0fffu)!=((load^0x00100000u)&0xffff0fffu))return -1;
 unsigned src=(previous>>12)&15u,dst=(load>>12)&15u;
 return src<4&&dst<4?(int)src:-1;
}

/* Translation-time lookback only. The caller bounds this span and excludes
 * control-flow/patch boundaries. No guest value survives a helper or branch.
 * Accept only r10-based word memory accesses and ordinary unconditional A32
 * arithmetic. Unknown encodings stop the search rather than infer clobbers. */
static inline int db_arm_reuse_register(const uint32_t *code,unsigned count,uint32_t load){
 if((load&0xff7f0000u)!=0xe51a0000u || ((load>>12)&15u)>=4)return -1;
 int offset=(int)(load&4095u);if(!(load&0x800000u))offset=-offset;
 unsigned clobbered=0;
 while(count){
  uint32_t ins=code[--count];unsigned rd=(ins>>12)&15u;
  uint32_t memory=ins&0xff7f0000u;
  if(memory==0xe50a0000u || memory==0xe51a0000u){
   if(rd==10 || rd==15)return -1;
   int at=(int)(ins&4095u);if(!(ins&0x800000u))at=-at;
   if(at==offset && rd<4 && !(clobbered&(1u<<rd)))return (int)rd;
   if(memory==0xe50a0000u){
    if(at<offset+4 && offset<at+4)return -1;
   }else clobbered|=1u<<rd;
   continue;
  }
  uint32_t wide=ins&0xfff00000u;
  if(wide!=0xe3000000u && wide!=0xe3400000u){
   if((ins&0xfe000000u)!=0xe2000000u &&
      ((ins&0xfe000000u)!=0xe0000000u || (ins&0x90u)==0x90u))return -1;
   unsigned op=(ins>>21)&15u;
   if(op>=8 && op<=11){
    if(!(ins&0x100000u) || rd)return -1;
    continue; /* TST/TEQ/CMP/CMN change flags, not register values. */
   }
  }
  if(rd==10 || rd==15)return -1;
  clobbered|=1u<<rd;
 }
 return -1;
}
