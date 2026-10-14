/* Host sanitizer/accuracy check. Host timings are deliberately not collected. */
#include "kernels.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned rng=7803;
static float random_float(void){rng=rng*1664525u+1013904223u;return ((int)(rng>>8)-8388608)/8388608.0f;}
int main(void) {
    zz_mat_fn mat[]={zz_mat_scalar,zz_mat_cmsis,zz_mat_neon};
    zz_vec_fn vec[]={zz_vec_scalar,zz_vec_cmsis,zz_vec_neon};
    float a[16],b[16],c[16];
    for(unsigned t=0;t<1000;t++) {
        for(unsigned i=0;i<16;i++){a[i]=random_float();b[i]=random_float();}
        for(unsigned f=0;f<3;f++) {
            mat[f](a,b,c);
            for(unsigned i=0;i<16;i++) {
                double want=0,scale=1;
                for(unsigned k=0;k<4;k++){double p=(double)a[(i/4)*4+k]*b[k*4+i%4];want+=p;scale+=fabs(p);}
                assert(isfinite(c[i])&&fabs(c[i]-want)<2e-6*scale);
            }
        }
    }
    const unsigned sizes[]={0,1,2,3,4,5,7,8,9,15,16,17,31,32,33,127,256,1023,1024,1025,3072,4096};
    for(unsigned t=0;t<sizeof(sizes)/sizeof(*sizes);t++) {
        unsigned n=sizes[t];
        float *in=malloc(n*4*sizeof(float)),*out=malloc(n*4*sizeof(float));
        for(unsigned i=0;i<n*4;i++)in[i]=random_float();
        for(unsigned f=0;f<3;f++) {
            vec[f](a,in,out,n);
            for(unsigned i=0;i<n*4;i++) {
                double want=0,scale=1;
                for(unsigned k=0;k<4;k++){double p=(double)a[(i%4)*4+k]*in[(i/4)*4+k];want+=p;scale+=fabs(p);}
                assert(isfinite(out[i])&&fabs(out[i]-want)<2e-6*scale);
            }
        }
        free(in);free(out);
    }
    assert(zz_matrix_scene_open());
    float identity[16]={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    float tri[12]={-0.5f,-0.5f,0,1, 0.5f,-0.5f,0,1, 0,0.5f,0,1},transformed[12];
    zz_matrix_scene_draw(tri,0);unsigned blank=zz_matrix_scene_hash(),first=0;
    for(unsigned f=0;f<3;f++) {
        vec[f](identity,tri,transformed,3);zz_matrix_scene_draw(transformed,1);
        unsigned h=zz_matrix_scene_hash();assert(h!=blank);
        if(!f)first=h;else assert(h==first);
    }
    zz_matrix_scene_close();
    puts("PASS native ASan/UBSan matrix/vector bounds and double-oracle accuracy; nonblank identical scene and cleanup. No A9 speed claim.");
}
