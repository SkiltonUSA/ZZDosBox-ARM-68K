/* Bounded offscreen fixtures. Marker ordering follows Xanxi librixgpu and
 * the physically tested ZZUAE Core0 scissor-marker probe. */
#include "probe_scene.h"
#include "pipeline.h"
static void word(grx_packet *p,unsigned i,uint32_t v)
{for(unsigned n=0;n<4;n++)p->bytes[4*i+n]=(uint8_t)(v>>(24-8*n));}
static void append(grx_packet *p,uint32_t v){word(p,p->words++,v);}
static void reg(grx_packet *p,unsigned r,uint32_t v){append(p,0x10000000|r);append(p,v);}
static int scene_head(grx_packet *p,unsigned scene,grx_config *config)
{
    if(scene>=GRX_PROBE_SCENES)return 0;
    grx_config c={GRX_PROBE_WIDTH,GRX_PROBE_HEIGHT,GRX_PROBE_COLOR,0,0,0,GRX_LOWER_LEFT,GRX_OPAQUE,0,0,0};
    if(scene==2)c.blend=GRX_ADDITIVE;
    if(scene==3){c.texture_width=c.texture_height=16;c.texture_address=GRX_PROBE_TEXTURE;}
    if(scene==4)c.blend=GRX_ALPHA;
    if(grx_begin(p,&c)!=GRX_OK)return 0;
    if(!scene){reg(p,1,0);append(p,0x20000012u|((GRX_PROBE_WIDTH*GRX_PROBE_HEIGHT)<<8));}
    else {
        grx_vertex v[3]={{8,8,255,0,0,255,0,0,1},{56,8,255,0,0,255,256,0,1},{8,56,255,0,0,255,0,256,1}};
        for(unsigned i=0;i<3;i++) {
            if(scene==2){v[i].r=0;v[i].g=255;}
            if(scene==3){v[i].g=v[i].b=255;}
            if(scene==4)v[i].a=128;
        }
        if(grx_triangle(p,v)!=GRX_OK)return 0;
    }
    *config=c;return 1;
}
int grx_probe_scene(grx_packet *p,unsigned scene,unsigned marker)
{
    grx_config c;
    if(!marker||marker>65535||!scene_head(p,scene,&c))return 0;
    if(grx_end(p)!=GRX_OK||p->words+13>GRX_PACKET_WORDS)return 0;
    /* librixgpu's streamed-DDR path uses a marker, not FRAMEBUFFER COMMIT.
       The probe tests that reference sequence separately from grx_end(). */
    p->words--;
    /* Same address/resolution throughout. Restore colour writes and limit the
       marker memset to 64 pixels on GL row zero. ABI requires full-frame count. */
    append(p,0);append(p,0);
    reg(p,0,grx_features(&c)|0x20);reg(p,3,grx_fragment(&c));reg(p,6,0);reg(p,7,0x10040);
    reg(p,1,((marker&0xf800u)<<16)|((marker&0x7e0u)<<13)|((marker&31u)<<11));
    append(p,0x20000012u|((GRX_PROBE_WIDTH*GRX_PROBE_HEIGHT)<<8));
    word(p,0,0x90000000u|((p->words-2)*4));return 1;
}

/* Diagnostic alternative: a marker triangle in the same draw state, with no
 * scissor, fragment or feature register changes after the subject triangle.
 * Its centre at GL y=2, x=4..28 is strictly interior. Scene 2 adds green over
 * scene 1's red marker; scene 3 replaces yellow with red; scene 4 uses opaque
 * green (alpha=255). Every expected marker differs from the previous scene. */
int grx_probe_triangle_marker(grx_packet *p,unsigned scene)
{
    grx_config c;
    if(!scene)return grx_probe_scene(p,0,0x4120);
    if(!scene_head(p,scene,&c))return 0;
    grx_vertex v[3]={{1,1,255,0,0,255,0,0,1},{62,1,255,0,0,255,0,0,1},{1,7,255,0,0,255,0,0,1}};
    for(unsigned i=0;i<3;i++) {
        if(scene==2||scene==4){v[i].r=0;v[i].g=255;}
        if(scene==3){v[i].g=v[i].b=255;}
    }
    if(grx_triangle(p,v)!=GRX_OK||grx_end(p)!=GRX_OK)return 0;
    p->words--; /* no framebuffer COMMIT */
    word(p,0,0x90000000u|((p->words-2)*4));return 1;
}
