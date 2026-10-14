/* GPL-3.0-or-later. Exact integer four-pixel spans; scalar tail belongs to TinyGL. */
#ifndef ZZ_TGL_SPAN_H
#define ZZ_TGL_SPAN_H
#include "raster.h"
#include <arm_neon.h>
#if TGL_FEATURE_RENDER_BITS != 32
#error Diagnostic requires 32-bit pixels
#endif
static inline __attribute__((always_inline)) unsigned zz_span4(uint32_t *pp,uint16_t *pz,unsigned count,
    uint32_t z,uint32_t r,uint32_t g,uint32_t b,int dz,int dr,int dg,int db,int dt,int dw) {
    const uint32_t lanes[4]={0,1,2,3};uint32x4_t ix=vld1q_u32(lanes);
    uint32x4_t vz=vmlaq_n_u32(vdupq_n_u32(z),ix,(uint32_t)dz);
    uint32x4_t vr=vmlaq_n_u32(vdupq_n_u32(r),ix,(uint32_t)dr);
    uint32x4_t vg=vmlaq_n_u32(vdupq_n_u32(g),ix,(uint32_t)dg);
    uint32x4_t vb=vmlaq_n_u32(vdupq_n_u32(b),ix,(uint32_t)db);
    uint32x4_t sz=vdupq_n_u32((uint32_t)dz*4),sr=vdupq_n_u32((uint32_t)dr*4);
    uint32x4_t sg=vdupq_n_u32((uint32_t)dg*4),sb=vdupq_n_u32((uint32_t)db*4);
    unsigned i=0;
    for(;i+4<=count;i+=4) {
        uint32x4_t zz=vshrq_n_u32(vz,ZB_POINT_Z_FRAC_BITS);
        uint32x4_t pixel=vorrq_u32(vandq_u32(vr,vdupq_n_u32(0xff0000)),vorrq_u32(vandq_u32(vshrq_n_u32(vg,8),vdupq_n_u32(0xff00)),vandq_u32(vshrq_n_u32(vb,16),vdupq_n_u32(0xff))));
        if(dt){
            uint16x4_t oldz=vld1_u16(pz+i);
            /* Compare full shifted 32-bit depth BEFORE narrowing to stored u16. */
            uint32x4_t mask=vcgeq_u32(zz,vmovl_u16(oldz));
            vst1q_u32(pp+i,vbslq_u32(mask,pixel,vld1q_u32(pp+i)));
            if(dw)vst1_u16(pz+i,vbsl_u16(vmovn_u32(mask),vmovn_u32(zz),oldz));
        } else {
            vst1q_u32(pp+i,pixel);if(dw)vst1_u16(pz+i,vmovn_u32(zz));
        }
        vz=vaddq_u32(vz,sz);vr=vaddq_u32(vr,sr);vg=vaddq_u32(vg,sg);vb=vaddq_u32(vb,sb);
    }
    return i;
}
#endif
