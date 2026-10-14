/* GPL-3.0-or-later. Preserve colour pitch, untouched buffers and exact tails. */
#include "raster.h"
#include <arm_neon.h>
_Static_assert(sizeof(PIXEL)==4 && sizeof(uint32_t)==4,"32-bit pixel ABI");
unsigned zz_tgl_raster,zz_tgl_clear;
void zz_fill32(uint32_t *p,uint32_t value,unsigned count) {
    uint32x4_t v=vdupq_n_u32(value);unsigned i=0;
    for(;i+16<=count;i+=16){vst1q_u32(p+i,v);vst1q_u32(p+i+4,v);vst1q_u32(p+i+8,v);vst1q_u32(p+i+12,v);}
    for(;i+4<=count;i+=4)vst1q_u32(p+i,v);
    for(;i<count;i++)p[i]=value;
}
void zz_fill16(uint16_t *p,uint16_t value,unsigned count) {
    uint16x8_t v=vdupq_n_u16(value);unsigned i=0;
    for(;i+32<=count;i+=32){vst1q_u16(p+i,v);vst1q_u16(p+i+8,v);vst1q_u16(p+i+16,v);vst1q_u16(p+i+24,v);}
    for(;i+8<=count;i+=8)vst1q_u16(p+i,v);
    for(;i<count;i++)p[i]=value;
}
int zz_clear_buffers(ZBuffer *zb,int clear_z,int z,int clear_color,int r,int g,int b) {
#if TGL_FEATURE_RENDER_BITS != 32 || TGL_HAS(DIRTY_RECTANGLE)
    return 0;
#else
#if TGL_HAS(FORCE_CLEAR_NO_COPY_COLOR)
    uint32_t color=TGL_NO_COPY_COLOR;
#else
    uint32_t color=RGB_TO_PIXEL(r,g,b);
#endif
    /* Physical v1 showed nonzero clears slower: retain upstream for those. */
    if((clear_z && (uint16_t)z!=0) || (clear_color && color!=0))return 0;
    if(clear_z)zz_fill16(zb->zbuf,0,(unsigned)zb->xsize*zb->ysize);
    if(clear_color)for(int y=0;y<zb->ysize;y++)zz_fill32((uint32_t*)((unsigned char*)zb->pbuf+y*zb->linesize),0,zb->xsize);
    return 1;
#endif
}

/* Bounded exact kernel checks also execute on the real A9 before timing. */
#include "span.h"
unsigned zz_raster_selftest(void) {
    uint32_t p[48],ref[48];uint16_t z[48],zr[48];unsigned errors=0;
    for(unsigned mode=1;mode<=3;mode++)for(unsigned pattern=0;pattern<4;pattern++)for(unsigned n=0;n<=40;n++)for(unsigned off=0;off<4;off++)for(unsigned flags=0;flags<4;flags++) {
        for(unsigned i=0;i<48;i++){p[i]=ref[i]=0x95345678u+i;z[i]=zr[i]=(i%4==0?65535:i%4==1?0:12345);}
        uint32_t vz=0xfffffff0u,vr=0xffff0000u,vg=0x7fffffff, vb=0x12ffff;
        int dz=0x1234567,dr=-54321,dg=76543,db=-99887;
        zz_tgl_raster=mode;
        if(pattern){vz=100u<<14;dz=0;for(unsigned i=0;i<48;i++)z[i]=zr[i]=pattern==1?101:pattern==2?100:((i&4)?101:99);}
        unsigned done=zz_span4(p+off,z+off,n,vz,vr,vg,vb,dz,dr,dg,db,flags&1,flags>>1);
        if(done!=(n&~3u))errors++;
        for(unsigned i=0;i<done;i++){
            uint32_t zz=vz>>14;
            if(!(flags&1)||zz>=zr[off+i]){ref[off+i]=(vr&0xff0000)|((vg>>8)&0xff00)|((vb>>16)&0xff);if(flags>>1)zr[off+i]=(uint16_t)zz;}
            vz+=dz;vr+=dr;vg+=dg;vb+=db;
        }
        for(unsigned i=0;i<48;i++)if(p[i]!=ref[i]||z[i]!=zr[i])errors++;
        zz_fill32(p+off,0xab123456,n);zz_fill16(z+off,0x5678,n);
        for(unsigned i=0;i<48;i++){
            if(i>=off&&i<off+n){ref[i]=0xab123456;zr[i]=0x5678;}
            if(p[i]!=ref[i]||z[i]!=zr[i])errors++;
        }
    }
    zz_tgl_raster=0;return errors;
}
