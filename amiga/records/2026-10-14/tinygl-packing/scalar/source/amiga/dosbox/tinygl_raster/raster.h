/* GPL-3.0-or-later. Diagnostic-only owned-RAM TinyGL kernels. */
#ifndef ZZ_TGL_RASTER_H
#define ZZ_TGL_RASTER_H
#include <stdint.h>
#include <zbuffer.h>
#ifdef __cplusplus
extern "C" {
#endif
extern unsigned zz_tgl_raster,zz_tgl_clear;
int zz_clear_buffers(ZBuffer *,int,int,int,int,int,int);
void zz_fill32(uint32_t *,uint32_t,unsigned);
void zz_fill16(uint16_t *,uint16_t,unsigned);
void zz_tgl_clear_benchmark(unsigned);
#ifdef __cplusplus
}
#endif
#endif
