/* GPL-3.0-or-later. Independent ARMv7 NEON implementation, no paper code copied. */
#include "kernels.h"
#include <arm_neon.h>
void zz_mat_neon(const float *a,const float *b,float *c) {
    float32x4_t b0=vld1q_f32(b), b1=vld1q_f32(b+4), b2=vld1q_f32(b+8), b3=vld1q_f32(b+12);
    float32x4_t r0=vmulq_n_f32(b0,a[0]), r1=vmulq_n_f32(b0,a[4]);
    float32x4_t r2=vmulq_n_f32(b0,a[8]), r3=vmulq_n_f32(b0,a[12]);
    r0=vmlaq_n_f32(r0,b1,a[1]); r1=vmlaq_n_f32(r1,b1,a[5]);
    r2=vmlaq_n_f32(r2,b1,a[9]); r3=vmlaq_n_f32(r3,b1,a[13]);
    r0=vmlaq_n_f32(r0,b2,a[2]); r1=vmlaq_n_f32(r1,b2,a[6]);
    r2=vmlaq_n_f32(r2,b2,a[10]); r3=vmlaq_n_f32(r3,b2,a[14]);
    r0=vmlaq_n_f32(r0,b3,a[3]); r1=vmlaq_n_f32(r1,b3,a[7]);
    r2=vmlaq_n_f32(r2,b3,a[11]); r3=vmlaq_n_f32(r3,b3,a[15]);
    vst1q_f32(c,r0); vst1q_f32(c+4,r1); vst1q_f32(c+8,r2); vst1q_f32(c+12,r3);
}
void zz_vec_neon(const float *m,const float *v,float *out,unsigned n) {
    unsigned i=0;
    for(;i+4<=n;i+=4) {
        float32x4x4_t p=vld4q_f32(v+i*4), q;
        q.val[0]=vmulq_n_f32(p.val[0],m[0]); q.val[1]=vmulq_n_f32(p.val[0],m[4]);
        q.val[2]=vmulq_n_f32(p.val[0],m[8]); q.val[3]=vmulq_n_f32(p.val[0],m[12]);
        q.val[0]=vmlaq_n_f32(q.val[0],p.val[1],m[1]); q.val[1]=vmlaq_n_f32(q.val[1],p.val[1],m[5]);
        q.val[2]=vmlaq_n_f32(q.val[2],p.val[1],m[9]); q.val[3]=vmlaq_n_f32(q.val[3],p.val[1],m[13]);
        q.val[0]=vmlaq_n_f32(q.val[0],p.val[2],m[2]); q.val[1]=vmlaq_n_f32(q.val[1],p.val[2],m[6]);
        q.val[2]=vmlaq_n_f32(q.val[2],p.val[2],m[10]); q.val[3]=vmlaq_n_f32(q.val[3],p.val[2],m[14]);
        q.val[0]=vmlaq_n_f32(q.val[0],p.val[3],m[3]); q.val[1]=vmlaq_n_f32(q.val[1],p.val[3],m[7]);
        q.val[2]=vmlaq_n_f32(q.val[2],p.val[3],m[11]); q.val[3]=vmlaq_n_f32(q.val[3],p.val[3],m[15]);
        vst4q_f32(out+i*4,q);
    }
    if(i<n) zz_vec_scalar(m,v+i*4,out+i*4,n-i);
}
