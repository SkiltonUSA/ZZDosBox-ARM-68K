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
static inline unsigned grx_probe_verify(const volatile uint16_t *pixels,unsigned scene,unsigned *checked)
{
    unsigned bad=0;*checked=0;
    for(unsigned row=0;row<64;row++)for(unsigned x=0;x<64;x++) {
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
                    if(r<15||r>17||g||b<14||b>16)bad++;
                    continue;
                }
            }
        }
        (*checked)++;if(got!=want)bad++;
    }
    return bad;
}
#endif
