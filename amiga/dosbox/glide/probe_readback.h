/* RGB565 GPU memory is little endian on both observers. */
#ifndef GRX_PROBE_READBACK_H
#define GRX_PROBE_READBACK_H
#include "probe_scene.h"
static inline uint16_t grx_probe_pixel(uint16_t v) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
 return __builtin_bswap16(v);
#else
 return v;
#endif
}
/* Captures the values used by the verdict, never a second framebuffer read. */
#define GRX_TRACE_SAMPLES 16u
struct grx_bad_pixel {unsigned offset,got,want;};
struct grx_readback_trace {unsigned count;struct grx_bad_pixel samples[GRX_TRACE_SAMPLES];};
static inline void grx_trace_bad(struct grx_readback_trace *trace,unsigned offset,unsigned got,unsigned want) {
    if(trace&&trace->count<GRX_TRACE_SAMPLES) {
        struct grx_bad_pixel *p=&trace->samples[trace->count++];
        p->offset=offset;p->got=got;p->want=want;
    }
}
static inline unsigned grx_probe_verify_order(const volatile uint16_t *pixels,unsigned scene,unsigned *checked,struct grx_readback_trace *trace,unsigned reverse)
{
    unsigned bad=0;*checked=0;if(trace)trace->count=0;
    for(unsigned n=0;n<4096;n++) {
        unsigned offset=reverse?4095-n:n,row=offset/64,x=offset%64;
        unsigned y=GRX_PROBE_HEIGHT-1-row;
        int inside=x>=10&&y>=10&&x+y<=62;
        int outside=x<6||y<6||x+y>66;
        if(scene&&!inside&&!outside)continue; /* edge-rule probe comes later */
        unsigned got=grx_probe_pixel(pixels[row*64+x]),want=0;
        if(scene&&inside) {
            if(scene==1)want=0xf800;
            if(scene==2)want=0xffe0;
            if(scene>=3) {
                if(x>=29&&x<=35)continue; /* avoid texture boundary/filter ambiguity */
                want=x<32?0xf800:0x001f;
                if(scene==4&&x>32) {
                    unsigned r=got>>11,g=(got>>5)&63,b=got&31;
                    (*checked)++;
                    if(r<15||r>17||g||b<14||b>16){bad++;grx_trace_bad(trace,row*64+x,got,0x800f);}
                    continue;
                }
            }
        }
        (*checked)++;if(got!=want){bad++;grx_trace_bad(trace,row*64+x,got,want);}
    }
    return bad;
}
static inline unsigned grx_probe_verify_trace(const volatile uint16_t *pixels,unsigned scene,unsigned *checked,struct grx_readback_trace *trace) {
    return grx_probe_verify_order(pixels,scene,checked,trace,0);
}
static inline unsigned grx_probe_verify(const volatile uint16_t *pixels,unsigned scene,unsigned *checked) {
    return grx_probe_verify_trace(pixels,scene,checked,0);
}
#endif
