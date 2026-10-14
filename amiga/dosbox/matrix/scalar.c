/* GPL-3.0-or-later. Compile with scalar VFP, no auto-vectorization. */
#include "kernels.h"
void zz_mat_scalar(const float *a,const float *b,float *c) {
    for(unsigned r=0;r<4;r++) for(unsigned j=0;j<4;j++) {
        float s=a[r*4]*b[j];
        for(unsigned k=1;k<4;k++) s+=a[r*4+k]*b[k*4+j];
        c[r*4+j]=s;
    }
}
void zz_vec_scalar(const float *m,const float *v,float *out,unsigned n) {
    for(unsigned i=0;i<n;i++) for(unsigned r=0;r<4;r++) {
        float s=m[r*4]*v[i*4];
        for(unsigned k=1;k<4;k++) s+=m[r*4+k]*v[i*4+k];
        out[i*4+r]=s;
    }
}
