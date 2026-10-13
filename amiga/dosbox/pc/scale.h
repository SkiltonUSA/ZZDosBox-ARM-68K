/* GPL-3.0-or-later. Exact nearest-neighbour rows for the 640-pixel display.
 * Caller validates dimensions/format and retains the framebuffer mapping.
 * Scalar integer operations only: no new VFP/NEON state or alignment contract. */
#pragma once
#include <stdint.h>

template<bool Swap> static inline uint32_t pc_pixel(uint32_t c) {
 return Swap ? __builtin_bswap32(c) : c;
}

template<bool Swap> static inline void pc_row(volatile uint32_t *dst,
 const uint32_t *src,unsigned width) {
 if(width==320) {
  for(unsigned x=0;x<320;x++) {
   uint32_t c=pc_pixel<Swap>(src[x]);
   dst[x*2]=c;dst[x*2+1]=c;
  }
 } else if(width==640) {
  for(unsigned x=0;x<640;x++)dst[x]=pc_pixel<Swap>(src[x]);
 } else {
  for(unsigned x=0;x<640;x++)dst[x]=pc_pixel<Swap>(src[x*width/640]);
 }
}

static inline void pc_scale_row(volatile uint32_t *dst,const uint32_t *src,
 unsigned width,unsigned format) {
 if(format==3)pc_row<false>(dst,src,width);
 else pc_row<true>(dst,src,width);
}
