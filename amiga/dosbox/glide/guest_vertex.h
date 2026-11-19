/* GPL-2.0-or-later. Glide 2 GrVertex is 60 little-endian bytes, not our native structure. */
#ifndef GRX_GUEST_VERTEX_H
#define GRX_GUEST_VERTEX_H
#include "guest.h"
#include "translator.h"
#ifdef __cplusplus
extern "C" {
#endif
/* tmu_w: GR_STWHINT_W_DIFF_TMU0 selected; otherwise use global vertex oow.
   Failure leaves output unchanged. No geometry/state validity is implied. */
int grx_guest_vertices(grx_vertex [3],const uint32_t [3],int tmu_w,const grx_guest_io *);
#ifdef __cplusplus
}
#endif
#endif
