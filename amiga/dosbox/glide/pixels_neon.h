#ifndef GRX_PIXELS_NEON_H
#define GRX_PIXELS_NEON_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void grx_rgb565_xrgb(const unsigned char *src, uint32_t *dst, unsigned count);
#ifdef __cplusplus
}
#endif
#endif
