/* GPL-3.0-or-later. Row-major finite f32 kernels for the isolated benchmark.
 * Inputs and output must not overlap. Vertices are AoS [x,y,z,w]. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*zz_mat_fn)(const float *, const float *, float *);
typedef void (*zz_vec_fn)(const float *, const float *, float *, unsigned);
void zz_mat_scalar(const float *, const float *, float *);
void zz_mat_cmsis(const float *, const float *, float *);
void zz_mat_neon(const float *, const float *, float *);
void zz_vec_scalar(const float *, const float *, float *, unsigned);
void zz_vec_cmsis(const float *, const float *, float *, unsigned);
void zz_vec_neon(const float *, const float *, float *, unsigned);
int zz_matrix_scene_open(void);
void zz_matrix_scene_draw(const float *, unsigned);
unsigned zz_matrix_scene_hash(void);
void zz_matrix_scene_close(void);
#ifdef __cplusplus
}
#endif
