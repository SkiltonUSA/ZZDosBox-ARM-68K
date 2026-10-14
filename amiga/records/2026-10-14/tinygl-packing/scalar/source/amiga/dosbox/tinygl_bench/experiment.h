/* GPL-3.0-or-later. Diagnostic-only hooks, single owned Core1/context. */
#ifndef ZZ_TGL_EXPERIMENT_H
#define ZZ_TGL_EXPERIMENT_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { ZZ_TRANSFORM, ZZ_CLIP, ZZ_RASTER, ZZ_CLEAR, ZZ_OTHER, ZZ_STAGES };
extern unsigned zz_tgl_neon, zz_tgl_profile, zz_tgl_faults;
extern uint64_t zz_tgl_times[ZZ_STAGES];
extern unsigned zz_tgl_calls[ZZ_STAGES];
uint64_t zz_tgl_clock(void);
void zz_tgl_enter(unsigned stage);
void zz_tgl_leave(void);
void zz_tgl_profile_begin(void);
uint64_t zz_tgl_profile_end(void);
int zz_matrix_scene_open(void);
void zz_matrix_scene_close(void);
void zz_matrix_scene_draw(unsigned kind, unsigned triangles);
unsigned zz_matrix_scene_hash(void);
void zz_mat_neon(const float *,const float *,float *);
#ifdef __cplusplus
}
#endif
#endif
