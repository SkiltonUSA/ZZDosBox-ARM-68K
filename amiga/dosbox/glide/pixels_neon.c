/* Exact floor-scaled RGB565 expansion, matching the existing GTA presenter.
 * Little-endian byte loads permit unaligned framebuffer input. */
#include "pixels_neon.h"
#include <arm_neon.h>
void grx_rgb565_xrgb(const unsigned char *src, uint32_t *dst, unsigned count)
{
 while (count >= 8) {
  uint16x8_t v = vreinterpretq_u16_u8(vld1q_u8(src));
  uint16x8_t r = vshrq_n_u16(v, 11);
  uint16x8_t g = vandq_u16(vshrq_n_u16(v, 5), vdupq_n_u16(63));
  uint16x8_t b = vandq_u16(v, vdupq_n_u16(31));
  /* These bounded integer identities equal x*255/31 and x*255/63
   * for every channel value, including the green low bit. */
  r = vshrq_n_u16(vmulq_n_u16(r, 1053), 7);
  g = vshrq_n_u16(vaddq_u16(vmulq_n_u16(g, 259), vdupq_n_u16(3)), 6);
  b = vshrq_n_u16(vmulq_n_u16(b, 1053), 7);
  uint8x8x4_t out = {{vmovn_u16(b), vmovn_u16(g), vmovn_u16(r), vdup_n_u8(0)}};
  vst4_u8((uint8_t *)dst, out);
  src += 16; dst += 8; count -= 8;
 }
 while (count--) {
  unsigned v = src[0] | (unsigned)src[1] << 8;
  *dst++ = (((v >> 11)*255/31)<<16) | ((((v>>5)&63)*255/63)<<8) | ((v&31)*255/31);
  src += 2;
 }
}

static uint16x8_t rgb555x8(uint16x8_t v){
 return vorrq_u16(vandq_u16(vshrq_n_u16(v,1),vdupq_n_u16(0x7fe0)),
                  vandq_u16(v,vdupq_n_u16(31)));
}
void grx_rgb565_555(const unsigned char *src,unsigned char *dst,unsigned count){
 while(count>=8){
  uint16x8_t v=rgb555x8(vreinterpretq_u16_u8(vld1q_u8(src)));
  vst1q_u8(dst,vreinterpretq_u8_u16(v));src+=16;dst+=16;count-=8;
 }
 while(count--){
  unsigned v=src[0]|(unsigned)src[1]<<8;v=((v>>1)&0x7fe0)|(v&31);
  dst[0]=v;dst[1]=v>>8;src+=2;dst+=2;
 }
}
void grx_lfb_dirty_span(const unsigned char *guest,const unsigned char *shadow,
                       unsigned count,unsigned *lo,unsigned *hi){
 unsigned x=0;*lo=count;*hi=0;
 for(;count-x>=8;x+=8){
  uint16x8_t old=rgb555x8(vreinterpretq_u16_u8(vld1q_u8(shadow+2*x)));
  uint16x8_t diff=vmvnq_u16(vceqq_u16(old,vreinterpretq_u16_u8(vld1q_u8(guest+2*x))));
  uint32x2_t any=vreinterpret_u32_u16(vorr_u16(vget_low_u16(diff),vget_high_u16(diff)));
  if(vget_lane_u32(any,0)|vget_lane_u32(any,1)){
   if(*lo==count)*lo=x;
   *hi=x+8;
  }
 }
 for(;x<count;x++){
  unsigned old=shadow[2*x]|(unsigned)shadow[2*x+1]<<8;
  unsigned v=guest[2*x]|(unsigned)guest[2*x+1]<<8;
  if(v!=(((old>>1)&0x7fe0)|(old&31))){if(*lo==count)*lo=x;*hi=x+1;}
 }
}
