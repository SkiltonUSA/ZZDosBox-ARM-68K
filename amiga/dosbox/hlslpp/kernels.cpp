/* GPL-3.0-or-later. Wrappers around MIT-licensed hlsl++; no OS/GPU dependency. */
#include "kernels.h"
#include <hlsl++/vector_float.h>
#include <hlsl++/matrix_float.h>
using namespace hlslpp;

void zz_mat_hlsl(const float *a, const float *b, float *out) {
    float4x4 ma, mb;
    load(ma, a); load(mb, b);
    store(out, mul(ma, mb));
}
void zz_vec_hlsl(const float *m, const float *v, float *out, unsigned n) {
    if (!n) return;
    float4x4 matrix;
    load(matrix, m);
    for (unsigned i=0; i<n; ++i) {
        float4 vertex; load(vertex, v+i*4);
        store(out+i*4, mul(matrix, vertex));
    }
}
/* TinyGL caches columns when its combined matrix is invalidated. Keep its
 * implicit input W=1 and accumulation order. No 4-float read from XYZ. */
void zz_vertex_hlsl(const float *columns, const float *xyz, float *out) {
    float4 c0,c1,c2,c3;
    load(c0, columns); load(c1, columns+4);
    load(c2, columns+8); load(c3, columns+12);
    float4 r=c0*float1(xyz[0]);
    r=r+c1*float1(xyz[1]);
    r=r+c2*float1(xyz[2]);
    store(out, r+c3);
}
