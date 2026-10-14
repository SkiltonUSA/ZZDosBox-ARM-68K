/* GPL-3.0-or-later. Isolate spans, clear and combination; transforms stay scalar. */
#include "raster.h"
#include "experiment.h"
#include "pc_wire.h"
#include "profile.h"
#ifndef DB_TINYGL_RASTER
#error Diagnostic build only
#endif
extern "C" volatile uint8_t *db_shared();
extern "C" unsigned zz_raster_selftest(void);
extern "C" void zz_tgl_scene_prepare(unsigned,unsigned);
extern "C" {unsigned zz_matrix_quick=0;}
extern "C" uint64_t zz_tgl_clock(void){return pc_clock_ticks();}
static constexpr unsigned report=PC_VIDEO+0xe00,cases=8,stride=48;
static_assert(report+16+cases*stride<=PC_VIDEO+0xfa0,"owned report bounds");
static void execute(unsigned c,unsigned n){if(c<4||c==7)zz_matrix_scene_draw(c,n);else zz_tgl_clear_benchmark(c);}
int pc_matrix_benchmark(int (*stop)()){
    volatile uint8_t *w=db_shared()+report;ad_put(w,0,0);unsigned errors=zz_raster_selftest();
    const unsigned counts[]={1024,128,32,32,0,0,0,32};
    const unsigned prior_hashes[]={0x88b1e9f7u,0x0628ae41u,0x88077dd1u};zz_tgl_neon=zz_tgl_profile=0;
    if(!zz_matrix_scene_open()){zz_matrix_scene_close();return -1;}
    for(unsigned c=0;c<cases;c++){
        unsigned n=counts[c],reps=zz_matrix_quick?1:((c<4||c==7)?8:64),hash=0;
        if(c<4||c==7)zz_tgl_scene_prepare(c,n);
        uint64_t times[4]={0,0,0,0};const unsigned order[]={0,1,2,3,3,2,1,0};
        for(unsigned pass=0;pass<8;pass++){
            if(stop&&stop()){zz_matrix_scene_close();return 0;}
            unsigned m=order[pass];zz_tgl_raster=m;zz_tgl_clear=0;
            execute(c,n);unsigned h=zz_matrix_scene_hash();
            if(!pass){hash=h;if(c<3&&h!=prior_hashes[c])errors++;}else if(h!=hash)errors++;
            uint64_t begin=pc_clock_ticks();for(unsigned i=0;i<reps;i++)execute(c,n);
            times[m]+=pc_clock_ticks()-begin;if(zz_matrix_scene_hash()!=hash)errors++;
        }
        unsigned o=16+c*stride;ad_put(w,o,c);ad_put(w,o+4,n);ad_put(w,o+8,reps*2);ad_put(w,o+12,hash);
        for(unsigned m=0;m<4;m++){ad_put(w,o+16+m*8,times[m]>>32);ad_put(w,o+20+m*8,times[m]);}
    }
    zz_matrix_scene_close();ad_put(w,4,errors);ad_put(w,8,cases);ad_put(w,12,zz_matrix_quick);
    __asm__ volatile("dsb sy":::"memory");ad_put(w,0,0x52535432);return errors?-1:1;
}
