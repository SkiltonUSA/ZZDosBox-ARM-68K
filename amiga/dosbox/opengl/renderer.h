/* GPL-3.0-or-later. Private single-Core1 TinyGL software rendering context. */
#ifndef ZZ_GL_RENDERER_H
#define ZZ_GL_RENDERER_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
int zz_gl_render(const uint32_t *pixels,unsigned width,unsigned height);
/* Prepare before mapping any external bitmap: all context allocations occur here. */
int zz_gl_prepare(void);
/* Fixed 640x480, caller-owned validated mapping. Returns 1, 0 on error, -1 on
 * cancellation. No allocation or callbacks; detaches before returning. */
int zz_gl_render_to(const uint32_t *pixels,unsigned width,unsigned height,
                   volatile uint32_t *dst,unsigned pitch,unsigned format,
                   const volatile unsigned char *stop);
const uint32_t *zz_gl_pixels(void);
void zz_gl_close(void);
#ifdef __cplusplus
}
#endif
#endif
