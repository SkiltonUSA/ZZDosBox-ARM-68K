#ifndef GRX_PIXELS_NEON_H
#define GRX_PIXELS_NEON_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void grx_rgb565_xrgb(const unsigned char *src, uint32_t *dst, unsigned count);
void grx_rgb565_555(const unsigned char *src, unsigned char *dst, unsigned count);
/* Conservative span of changed 8-pixel blocks; [count,0) means unchanged.
 * The upload path must still compare pixels within this span. */
void grx_lfb_dirty_span(const unsigned char *guest, const unsigned char *shadow,
                        unsigned count, unsigned *lo, unsigned *hi);
#ifdef __cplusplus
}
#endif
#endif
