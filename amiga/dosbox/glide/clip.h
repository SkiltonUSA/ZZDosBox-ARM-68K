/* GPL-2.0-or-later. Optional viewport clipper; encoder remains bounded and unchanged. */
#ifndef GRX_CLIP_H
#define GRX_CLIP_H
#include "translator.h"
#define GRX_CLIP_VERTICES 7u
#ifdef __cplusplus
extern "C" {
#endif
/* Convex polygon in original winding, clipped to encoder pixel-centre bounds.
   Caller emits the fan in order and handles packet FULL between triangles.
   No claim of exact Glide shared-edge/coverage equivalence. */
grx_result grx_clip_triangle(const grx_vertex [3],unsigned width,unsigned height,
                             grx_vertex out[GRX_CLIP_VERTICES],unsigned *count);
#ifdef __cplusplus
}
#endif
#endif
