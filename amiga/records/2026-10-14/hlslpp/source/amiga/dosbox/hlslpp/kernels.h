/* GPL-3.0-or-later. Diagnostic C ABI; finite float32, nonoverlapping arrays. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void zz_mat_hlsl(const float *, const float *, float *);
void zz_vec_hlsl(const float *, const float *, float *, unsigned);
void zz_vertex_hlsl(const float *columns, const float *xyz, float *xyzw);
#ifdef __cplusplus
}
#endif
