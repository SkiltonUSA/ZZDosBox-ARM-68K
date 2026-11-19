/* Soft-float wrapper: preserve SIMD state before entering softfp benchmark code. */
#include "capacity.h"
#include "bench.h"
extern int db_neon_available(uint32_t *);
extern void db_neon_snapshot(uint32_t *),db_neon_restore(const uint32_t *);
extern void db_neon_ieee_begin(void);
static uint32_t before[65] __attribute__((aligned(8)));
static uint32_t after[65] __attribute__((aligned(8)));
int mesen_test(int verify,struct MesenResult *r,int (*stop)(void),uint32_t (*clock)(void)) {
    uint32_t features[4];struct gc_result measured;
    if(!db_neon_available(features))return 6;
    db_neon_snapshot(before);db_neon_ieee_begin();
    int error=grx_capacity(&measured,stop,clock);
    db_neon_restore(before);db_neon_snapshot(after);
    for(unsigned i=0;i<65;i++)if(before[i]!=after[i])return 7;
    if(error)return error;
    r->checked=verify?measured.triangles:0;r->cycles=measured.triangles;
    r->hash=measured.hash;r->ticks=measured.ticks;
    return 0;
}
