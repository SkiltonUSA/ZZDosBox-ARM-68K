#include "experiment.h"
#include "span.h"
#include "zgl.h"
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern unsigned zz_raster_selftest(void);
extern void zz_tgl_scene_prepare(unsigned,unsigned);
uint64_t zz_tgl_clock(void){static uint64_t t;return t+=37;}
static unsigned seed=8454;
static unsigned rnd(void){seed=seed*1664525+1013904223;return seed;}
static void span_tests(void){
 for(unsigned n=0;n<=643;n++)for(unsigned align=0;align<4;align++)for(unsigned flags=0;flags<4;flags++){
    unsigned bytes=(n+align)*4,zbytes=(n+align)*2;uint32_t *a=malloc(bytes),*b=malloc(bytes);uint16_t *za=malloc(zbytes),*zb=malloc(zbytes);
    for(unsigned i=0;i<n+align;i++){a[i]=b[i]=rnd();za[i]=zb[i]=rnd();}
    /* Exact unsigned recurrence exercises wrap, equality, pass, fail and >16-bit depth. */
    uint32_t z=rnd(),r=rnd(),g=rnd(),blue=rnd();int dz=(int)rnd(),dr=(int)rnd(),dg=(int)rnd(),db=(int)rnd();
    unsigned done=zz_span4(a+align,za+align,n,z,r,g,blue,dz,dr,dg,db,flags&1,flags>>1);assert(done==(n&~3u));
    for(unsigned i=0;i<done;i++){
        unsigned zz=z>>14;if(!(flags&1)||zz>=zb[i+align]){b[i+align]=(r&0xff0000)|((g>>8)&0xff00)|((blue>>16)&0xff);if(flags>>1)zb[i+align]=(uint16_t)zz;}
        z+=dz;r+=dr;g+=dg;blue+=db;
    }
    assert(!memcmp(a,b,bytes)&&!memcmp(za,zb,zbytes));free(a);free(b);free(za);free(zb);
 }
 puts("PASS 10304 span cases, exact integer recurrence, depth variants, alignments and untouched tails");
}
static void clear_tests(void){
 for(unsigned n=0;n<67;n++)for(unsigned off=0;off<8;off++){
    unsigned length=n+off;uint32_t *p=malloc(length*4);uint16_t *z=malloc(length*2);memset(p,0x95,length*4);memset(z,0x95,length*2);
    unsigned v=rnd();zz_fill32(p+off,v,n);zz_fill16(z+off,v,n);
    for(unsigned i=0;i<length;i++){assert(p[i]==(i<off?0x95959595:v));assert(z[i]==(i<off?0x9595:(uint16_t)v));}free(p);free(z);
 }
 for(unsigned width=1;width<36;width++)for(unsigned flags=0;flags<4;flags++)for(unsigned val=0;val<4;val++){
    unsigned pitch=width*4+12,bytes=pitch*3,zbytes=width*3*2;ZBuffer a={0},b={0};a.xsize=b.xsize=width;a.ysize=b.ysize=3;a.linesize=b.linesize=pitch;
    a.pbuf=malloc(bytes);b.pbuf=malloc(bytes);a.zbuf=malloc(zbytes);b.zbuf=malloc(zbytes);
    memset(a.pbuf,0x95,bytes);memset(b.pbuf,0x95,bytes);memset(a.zbuf,0x95,zbytes);memset(b.zbuf,0x95,zbytes);
    int depths[]={0,-1,0x5678,0x1ffff};int z=depths[val];
    zz_tgl_clear=0;ZB_clear(&a,flags&1,z,flags>>1,0x120000,0x340000,0x560000);
    zz_tgl_clear=1;ZB_clear(&b,flags&1,z,flags>>1,0x120000,0x340000,0x560000);
    assert(!memcmp(a.pbuf,b.pbuf,bytes)&&!memcmp(a.zbuf,b.zbuf,zbytes));free(a.pbuf);free(b.pbuf);free(a.zbuf);free(b.zbuf);
 }
 puts("PASS clears: 536 tail/alignment cases, 560 pitched/masked/nonzero comparisons");
}
int main(void){
 assert(!zz_raster_selftest());span_tests();clear_tests();assert(zz_matrix_scene_open());const unsigned counts[]={1024,128,32,32};zz_tgl_neon=zz_tgl_profile=0;
 for(unsigned flags=0;flags<4;flags++)for(unsigned c=0;c<4;c++){
    if(flags&1)glEnable(GL_DEPTH_TEST);else glDisable(GL_DEPTH_TEST);glDepthMask((flags>>1)?GL_TRUE:GL_FALSE);
    zz_tgl_scene_prepare(c,counts[c]);zz_tgl_raster=zz_tgl_clear=0;zz_matrix_scene_draw(c,counts[c]);
    GLContext *g=gl_get_context();unsigned bytes=g->zb->linesize*480,zbytes=640*480*2;void *color=malloc(bytes),*depth=malloc(zbytes);memcpy(color,g->zb->pbuf,bytes);memcpy(depth,g->zb->zbuf,zbytes);
    for(unsigned m=1;m<4;m++){
        zz_tgl_raster=m&1;zz_tgl_clear=m>>1;zz_matrix_scene_draw(c,counts[c]);
        assert(!memcmp(color,g->zb->pbuf,bytes)&&!memcmp(depth,g->zb->zbuf,zbytes));
    }
    free(color);free(depth);
 }
 zz_matrix_scene_close();puts("PASS 16 complete scene/depth combinations, 4 methods, exact colour/depth buffers, cleanup");
}
