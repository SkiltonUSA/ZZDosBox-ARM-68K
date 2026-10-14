/* GPL-3.0-or-later. Fuse brightness with scaling; never read RTG memory back. */
#pragma once
#include "scale.h"
#include "scanline_plan.h"
#ifdef DB_NEON
extern "C" void pc_neon_scale_dim(volatile uint32_t *,const uint32_t *,unsigned,unsigned,unsigned,unsigned);
#endif
static inline void pc_scan_scale_row(volatile uint32_t *out,const uint32_t *src,
 unsigned n,unsigned format,unsigned width,unsigned level){
 if(level==8){pc_scale_row(out,src,n,format,width);return;}
#ifdef DB_NEON
 if(width==n||width==n*2){pc_neon_scale_dim(out,src,n,format,width,level);return;}
#endif
 unsigned index=0,error=0,step=n/width,remainder=n%width;
 for(unsigned x=0;x<width;x++){
  uint32_t pixel=pc_scan_dim_pixel(src[index],level);
  out[x]=format==3?pixel:__builtin_bswap32(pixel);
  index+=step;error+=remainder;if(error>=width){error-=width;index++;}
 }
}
