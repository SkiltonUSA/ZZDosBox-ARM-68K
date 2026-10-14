/* GPL-3.0-or-later. Integer NEON only; caller owns the checked SIMD context.
 * No read beyond the source row, including unaligned RGB565/32-bit input. */
#include <arm_neon.h>
#include <stdint.h>
#include "scanline_plan.h"

extern "C" void pc_neon_black(volatile uint32_t *out,unsigned width){
 uint32_t *dst=const_cast<uint32_t *>(out);unsigned x=0;
 const uint32x4_t zero=vdupq_n_u32(0);
 for(;x+4<=width;x+=4)vst1q_u32(dst+x,zero);
 for(;x<width;x++)dst[x]=0;
}

extern "C" void pc_neon_capture(uint32_t *dst,const uint8_t *src,unsigned n,unsigned bpp){
 unsigned x=0;
 if(bpp==16){
  const uint16x8_t m5=vdupq_n_u16(31),m6=vdupq_n_u16(63);
  for(;x+8<=n;x+=8){
   uint16x8_t c=vreinterpretq_u16_u8(vld1q_u8(src+x*2));
   uint16x8_t r=vshrq_n_u16(c,11),g=vandq_u16(vshrq_n_u16(c,5),m6),b=vandq_u16(c,m5);
   r=vorrq_u16(vshlq_n_u16(r,3),vshrq_n_u16(r,2));
   g=vorrq_u16(vshlq_n_u16(g,2),vshrq_n_u16(g,4));
   b=vorrq_u16(vshlq_n_u16(b,3),vshrq_n_u16(b,2));
   uint32x4_t lo=vorrq_u32(vshlq_n_u32(vmovl_u16(vget_low_u16(r)),16),vshlq_n_u32(vmovl_u16(vget_low_u16(g)),8));
   uint32x4_t hi=vorrq_u32(vshlq_n_u32(vmovl_u16(vget_high_u16(r)),16),vshlq_n_u32(vmovl_u16(vget_high_u16(g)),8));
   vst1q_u32(dst+x,vorrq_u32(lo,vmovl_u16(vget_low_u16(b))));
   vst1q_u32(dst+x+4,vorrq_u32(hi,vmovl_u16(vget_high_u16(b))));
  }
  for(;x<n;x++){
   unsigned c=src[x*2]|(unsigned(src[x*2+1])<<8),r=c>>11,g=(c>>5)&63,b=c&31;
   dst[x]=(((r<<3)|(r>>2))<<16)|(((g<<2)|(g>>4))<<8)|(b<<3)|(b>>2);
  }
 }else{
  for(;x+4<=n;x+=4)vst1q_u32(dst+x,vreinterpretq_u32_u8(vld1q_u8(src+x*4)));
  for(;x<n;x++)dst[x]=src[x*4]|(uint32_t(src[x*4+1])<<8)|(uint32_t(src[x*4+2])<<16)|(uint32_t(src[x*4+3])<<24);
 }
}

static inline void scale(volatile uint32_t *out,const uint32_t *src,unsigned n,unsigned format,unsigned target,unsigned level){
 uint32_t *dst=const_cast<uint32_t *>(out);unsigned x=0;
 const bool twice=target==n*2,swap=format!=3;
 for(;x+4<=n;x+=4){
  uint32x4_t p=vld1q_u32(src+x);
  if(level!=8){
   uint8x16_t bytes=vreinterpretq_u8_u32(p);uint8x8_t factor=vdup_n_u8(level);
   p=vreinterpretq_u32_u8(vcombine_u8(vshrn_n_u16(vmull_u8(vget_low_u8(bytes),factor),3),vshrn_n_u16(vmull_u8(vget_high_u8(bytes),factor),3)));
  }
  if(swap)p=vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(p)));
  if(twice){uint32x4x2_t d=vzipq_u32(p,p);vst1q_u32(dst+x*2,d.val[0]);vst1q_u32(dst+x*2+4,d.val[1]);}
  else vst1q_u32(dst+x,p);
 }
 for(;x<n;x++){
  uint32_t p=level==8?src[x]:pc_scan_dim_pixel(src[x],level);if(swap)p=__builtin_bswap32(p);
  if(twice){dst[x*2]=p;dst[x*2+1]=p;}else dst[x]=p;
 }
}

extern "C" void pc_neon_scale(volatile uint32_t *out,const uint32_t *src,unsigned n,unsigned format,unsigned target){scale(out,src,n,format,target,8);}
extern "C" void pc_neon_scale_dim(volatile uint32_t *out,const uint32_t *src,unsigned n,unsigned format,unsigned target,unsigned level){scale(out,src,n,format,target,level);}
