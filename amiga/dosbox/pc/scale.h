/* GPL-3.0-or-later. Exact nearest-neighbour rows for the 640-pixel display.
 * Caller validates dimensions/format and retains the framebuffer mapping.
 * Optional NEON fast paths run only inside the checked SIMD context. */
#pragma once
#include <stdint.h>
#ifdef DB_NEON
extern "C" void pc_neon_scale(volatile uint32_t *,const uint32_t *,unsigned,unsigned,unsigned);
#endif

template<bool Swap> static inline uint32_t pc_pixel(uint32_t c) {
 return Swap ? __builtin_bswap32(c) : c;
}

template<bool Swap> static inline void pc_row(volatile uint32_t *dst,
 const uint32_t *src,unsigned width,unsigned target) {
 if(width*2==target) {
  for(unsigned x=0;x<width;x++) {
   uint32_t c=pc_pixel<Swap>(src[x]);
   dst[x*2]=c;dst[x*2+1]=c;
  }
 } else if(width==target) {
  for(unsigned x=0;x<target;x++)dst[x]=pc_pixel<Swap>(src[x]);
 } else {
  /* Exact floor(x*width/target), including downscaling. Divide only once
   * per row, rather than once per pixel on the Cortex-A9. */
  const unsigned step=width/target, remainder=width%target;
  unsigned source=0,error=0;
  for(unsigned x=0;x<target;x++) {
   dst[x]=pc_pixel<Swap>(src[source]);
   source+=step;error+=remainder;
   if(error>=target){error-=target;source++;}
  }
 }
}

static inline void pc_scale_row(volatile uint32_t *dst,const uint32_t *src,
 unsigned width,unsigned format,unsigned target=640) {
#ifdef DB_NEON
 if(width==target||width*2==target){pc_neon_scale(dst,src,width,format,target);return;}
#endif
 if(format==3)pc_row<false>(dst,src,width,target);
 else pc_row<true>(dst,src,width,target);
}
