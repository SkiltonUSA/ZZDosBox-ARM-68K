#ifndef GRX_PROBE_SCENE_H
#define GRX_PROBE_SCENE_H
#include "translator.h"
#define GRX_PROBE_COLOR 0x06610000u
#define GRX_PROBE_TEXTURE 0x06620000u
#define GRX_PROBE_WIDTH 64u
#ifndef GRX_PROBE_HEIGHT
#define GRX_PROBE_HEIGHT 72u /* bottom eight GL rows are marker-only */
#endif
#define GRX_PROBE_BYTES (GRX_PROBE_WIDTH*GRX_PROBE_HEIGHT*2u)
/* clear, red, green additive, red/blue texture, half-alpha red */
#define GRX_PROBE_SCENES 5u
#ifdef __cplusplus
extern "C" {
#endif
int grx_probe_scene(grx_packet *,unsigned scene,unsigned marker);
int grx_probe_triangle_marker(grx_packet *,unsigned scene);
static inline uint16_t grx_probe_texel(unsigned i){return (i%16)<8?0xf800:0x001f;}
#ifdef __cplusplus
}
#endif
#endif
