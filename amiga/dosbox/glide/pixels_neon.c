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
