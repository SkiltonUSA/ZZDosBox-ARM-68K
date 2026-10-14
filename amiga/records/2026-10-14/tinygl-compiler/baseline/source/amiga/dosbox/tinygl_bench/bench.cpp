/* GPL-3.0-or-later. Actual internal TinyGL A/B, profiler disabled in speed runs. */
#include "experiment.h"
#include "pc_wire.h"
#include "profile.h"
#include <string.h>
#if !defined(DB_TINYGL_BENCH) || !defined(DB_MATRIX_BENCH) || !defined(DB_PIC_VFP)
#error Separate guarded diagnostic required
#endif
extern "C" volatile uint8_t *db_shared();
extern "C" void zz_tgl_scene_prepare(unsigned,unsigned);
extern "C" { unsigned zz_matrix_quick=0; }
extern "C" uint64_t zz_tgl_clock(void) {return pc_clock_ticks();}
static constexpr unsigned report=PC_VIDEO+0xe00,cases=3,stride=128;
static_assert(report+16+cases*stride<=PC_VIDEO+0xfa0,"owned report bounds");
static void put64(volatile uint8_t *w,unsigned o,uint64_t v){ad_put(w,o,v>>32);ad_put(w,o+4,v);}
int pc_matrix_benchmark(int (*stop)()) {
    volatile uint8_t *w=db_shared()+report;ad_put(w,0,0);unsigned errors=0;
    const unsigned counts[]={1024,128,32};
    if(!zz_matrix_scene_open()){zz_matrix_scene_close();return -1;}
    for(unsigned c=0;c<cases;c++) {
        unsigned n=counts[c],reps=zz_matrix_quick?1:8,hash=0;
        zz_tgl_scene_prepare(c,n);uint64_t times[2]={0,0};
        const unsigned order[]={0,1,1,0};
        for(unsigned pass=0;pass<4;pass++) {
            if(stop&&stop()){zz_matrix_scene_close();return 0;}
            unsigned m=order[pass];zz_tgl_neon=m;zz_tgl_profile=0;
            zz_matrix_scene_draw(c,n);unsigned h=zz_matrix_scene_hash();
            if(!pass)hash=h;else if(h!=hash)errors++;
            uint64_t begin=pc_clock_ticks();
            for(unsigned i=0;i<reps;i++)zz_matrix_scene_draw(c,n);
            times[m]+=pc_clock_ticks()-begin;
            if(zz_matrix_scene_hash()!=hash)errors++;
        }
        unsigned o=16+c*stride;ad_put(w,o,c);ad_put(w,o+4,n);ad_put(w,o+8,reps*2);ad_put(w,o+12,hash);
        put64(w,o+16,times[0]);put64(w,o+24,times[1]);
        for(unsigned m=0;m<2;m++) {
            if(stop&&stop()){zz_matrix_scene_close();return 0;}
            zz_tgl_neon=m;zz_matrix_scene_draw(c,n); /* warm, timers off */
            zz_tgl_profile_begin();zz_matrix_scene_draw(c,n);uint64_t wall=zz_tgl_profile_end();
            if(zz_tgl_faults || zz_matrix_scene_hash()!=hash)errors++;
            if(zz_tgl_calls[ZZ_CLIP]!=n || !zz_tgl_calls[ZZ_RASTER] || zz_tgl_calls[ZZ_TRANSFORM]<n*3 || zz_tgl_calls[ZZ_CLEAR]!=1)errors++;
            put64(w,o+32+m*48,wall);
            for(unsigned stage=0;stage<ZZ_STAGES;stage++)put64(w,o+40+m*48+stage*8,zz_tgl_times[stage]);
        }
    }
    zz_matrix_scene_close();ad_put(w,4,errors);ad_put(w,8,cases);ad_put(w,12,zz_matrix_quick);
    __asm__ volatile("dsb sy":::"memory");ad_put(w,0,0x54474c31);return errors?-1:1;
}
