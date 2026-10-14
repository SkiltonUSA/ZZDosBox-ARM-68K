/* GPL-3.0-or-later. Wrapper around separately licensed, unmodified CMSIS-DSP. */
#include "kernels.h"
#include "dsp/matrix_functions.h"
#include <stdlib.h>
void zz_mat_cmsis(const float *a,const float *b,float *c) {
    arm_matrix_instance_f32 A={4,4,(float*)a}, B={4,4,(float*)b}, C={4,4,c};
    if(arm_mat_mult_f32(&A,&B,&C)!=ARM_MATH_SUCCESS) abort();
}
void zz_vec_cmsis(const float *m,const float *v,float *out,unsigned n) {
    if(!n) return;
    if(n>4096) abort();
    /* AoS rows V times transpose(M). This preparation is inside timed calls. */
    float t[16];
    for(unsigned r=0;r<4;r++) for(unsigned c=0;c<4;c++) t[r*4+c]=m[c*4+r];
    arm_matrix_instance_f32 A={(uint16_t)n,4,(float*)v}, B={4,4,t}, C={(uint16_t)n,4,out};
    if(arm_mat_mult_f32(&A,&B,&C)!=ARM_MATH_SUCCESS) abort();
}
