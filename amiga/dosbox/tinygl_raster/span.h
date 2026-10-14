/* GPL-3.0-or-later. Exact integer four-pixel spans; scalar tail belongs to TinyGL. */
#ifndef ZZ_TGL_SPAN_H
#define ZZ_TGL_SPAN_H
#include "raster.h"
#include <arm_neon.h>
#if TGL_FEATURE_RENDER_BITS != 32
#error Diagnostic requires 32-bit pixels
#endif
static inline __attribute__((always_inline)) unsigned zz_span4_neon(uint32_t *pp,uint16_t *pz,unsigned count,
    uint32_t z,uint32_t r,uint32_t g,uint32_t b,int dz,int dr,int dg,int db,int dt,int dw,int guarded) {
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
        uint32x4_t mask=vdupq_n_u32(~0u);uint16x4_t oldz=vdup_n_u16(0),mask16=vdup_n_u16(0xffff);
        uint64_t bits=~(uint64_t)0;
        if(dt){
            oldz=vld1_u16(pz+i);mask=vcgeq_u32(zz,vmovl_u16(oldz));
            mask16=vmovn_u32(mask);bits=vget_lane_u64(vreinterpret_u64_u16(mask16),0);
        }
        if(bits){ /* A wholly rejected block does no colour load or any stores. */
            /* Empty read/write vector operands stop colour packing being hoisted
               above rejection. No hardware fence or extra instruction. */
            if(guarded)__asm__ volatile("" : "+w"(vr), "+w"(vg), "+w"(vb));
            uint32x4_t pixel=vorrq_u32(vandq_u32(vr,vdupq_n_u32(0xff0000)),vorrq_u32(vandq_u32(vshrq_n_u32(vg,8),vdupq_n_u32(0xff00)),vandq_u32(vshrq_n_u32(vb,16),vdupq_n_u32(0xff))));
            if(bits==~(uint64_t)0){ /* No read/modify/write for wholly visible blocks. */
                vst1q_u32(pp+i,pixel);if(dw)vst1_u16(pz+i,vmovn_u32(zz));
            } else {
                vst1q_u32(pp+i,vbslq_u32(mask,pixel,vld1q_u32(pp+i)));
                if(dw)vst1_u16(pz+i,vbsl_u16(mask16,vmovn_u32(zz),oldz));
            }
        }
        vz=vaddq_u32(vz,sz);vr=vaddq_u32(vr,sr);vg=vaddq_u32(vg,sg);vb=vaddq_u32(vb,sb);
    }
    return i;
}
/* Mode 1 retains revision 2. Mode 2 guards packing. Mode 3 skips a
   fully rejected prefix using scalar depth only, then uses guarded NEON.
   Modes 4/5 use the original unguarded packer after rejection.
   All arithmetic wraps as uint32_t, including negative depth slopes. */
static inline __attribute__((always_inline)) unsigned zz_span4(uint32_t *pp,uint16_t *pz,unsigned count,
    uint32_t z,uint32_t r,uint32_t g,uint32_t b,int dz,int dr,int dg,int db,int dt,int dw) {
    if(zz_tgl_raster==1)return zz_span4_neon(pp,pz,count,z,r,g,b,dz,dr,dg,db,dt,dw,0);
    unsigned skipped=0;
    if(zz_tgl_raster>=3 && dt){
        while(skipped+4<=count){
            uint32_t z1=z+(uint32_t)dz,z2=z1+(uint32_t)dz,z3=z2+(uint32_t)dz;
            if((z>>ZB_POINT_Z_FRAC_BITS)>=pz[skipped] ||
               (z1>>ZB_POINT_Z_FRAC_BITS)>=pz[skipped+1] ||
               (z2>>ZB_POINT_Z_FRAC_BITS)>=pz[skipped+2] ||
               (z3>>ZB_POINT_Z_FRAC_BITS)>=pz[skipped+3])break;
            z=z3+(uint32_t)dz;skipped+=4;
        }
        if(skipped==(count&~3u))return skipped;
        r+=(uint32_t)dr*skipped;g+=(uint32_t)dg*skipped;b+=(uint32_t)db*skipped;
    }
    return skipped+zz_span4_neon(pp+skipped,pz+skipped,count-skipped,z,r,g,b,
        dz,dr,dg,db,dt,dw,zz_tgl_raster<4);
}
#endif
