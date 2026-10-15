/* Native ASan/UBSan, no host timing claims. */
#include "experiment.h"
#include "zgl.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
extern void zz_tgl_scene_prepare(unsigned,unsigned);
uint64_t zz_tgl_clock(void){static uint64_t tick;return tick+=37;}
static unsigned rng=321;
static float rnd(void){rng=rng*1664525+1013904223;return ((int)(rng>>8)-8388608)/8388608.0f;}
int main(void){
    assert(zz_matrix_scene_open());
    const unsigned counts[]={1024,128,32};
    for(unsigned c=0;c<3;c++){
        zz_tgl_scene_prepare(c,counts[c]);zz_tgl_neon=0;zz_matrix_scene_draw(c,0);unsigned blank=zz_matrix_scene_hash();
        zz_matrix_scene_draw(c,counts[c]);unsigned want=zz_matrix_scene_hash();assert(want!=blank);
        GLContext *g=gl_get_context();size_t pixels=g->zb->linesize*480,depth=640*480*sizeof(*g->zb->zbuf);
        void *image=malloc(pixels),*z=malloc(depth);memcpy(image,g->zb->pbuf,pixels);memcpy(z,g->zb->zbuf,depth);
        for(unsigned m=0;m<2;m++)for(unsigned prof=0;prof<2;prof++){
            zz_tgl_neon=m;if(prof)zz_tgl_profile_begin();zz_matrix_scene_draw(c,counts[c]);
            if(prof){uint64_t total=zz_tgl_profile_end();assert(total>0&&!zz_tgl_faults);assert(zz_tgl_calls[ZZ_CLIP]==counts[c]);assert(zz_tgl_calls[ZZ_RASTER]>0);if(c==1)assert(zz_tgl_calls[ZZ_RASTER]>counts[c]);}
            assert(want==zz_matrix_scene_hash());assert(!memcmp(image,g->zb->pbuf,pixels));assert(!memcmp(z,g->zb->zbuf,depth));
        }
        free(image);free(z);printf("PASS scene %u identical colour/depth; profiled and untimed\n",c);
    }
    /* Exercise dirty-matrix invalidation, perspective and constant-W, lighting fallback.
       Two vertices of an incomplete triangle avoid rasterisation of random inputs. */
    for(unsigned t=0;t<1000;t++) {
        float a[16],b[16],v[3];for(unsigned i=0;i<16;i++){a[i]=rnd();b[i]=rnd();}for(unsigned i=0;i<3;i++)v[i]=rnd();
        if(t%2==0){a[3]=a[7]=a[11]=b[3]=b[7]=b[11]=0;a[15]=b[15]=1;}
        if(t%3==0)glEnable(GL_LIGHTING);else glDisable(GL_LIGHTING);
        V4 expected;
        for(unsigned m=0;m<2;m++){
            zz_tgl_neon=m;glMatrixMode(GL_PROJECTION);glLoadMatrixf(a);glMatrixMode(GL_MODELVIEW);glLoadMatrixf(b);
            glBegin(GL_TRIANGLES);glVertex3f(v[0],v[1],v[2]);V4 got=gl_get_context()->vertex[0].pc;glEnd();
            if(!m)expected=got;else for(unsigned i=0;i<4;i++)assert(isfinite(got.v[i])&&fabsf(got.v[i]-expected.v[i])<2e-6f);
            /* Toggle again without a matrix API call: cached columns stay valid. */
            zz_tgl_neon=1-m;glBegin(GL_TRIANGLES);glVertex3f(v[0],v[1],v[2]);V4 toggled=gl_get_context()->vertex[0].pc;glEnd();
            for(unsigned i=0;i<4;i++)assert(isfinite(toggled.v[i])&&fabsf(toggled.v[i]-got.v[i])<2e-6f);
        }
    }
    zz_matrix_scene_close();puts("PASS 1000 matrix/vertex cases, lighting fallback, constant/perspective W, cleanup");
}
