/* GPL-3.0-or-later. Isolated cached-RAM matrix/scene benchmark, after SIMD guard. */
#include "kernels.h"
#include "pc_wire.h"
#include "profile.h"
#include <string.h>
#if !defined(DB_MATRIX_BENCH) || !defined(DB_PIC_VFP) || defined(DB_OPENGL) || defined(DB_VIDEO_TRACE) || defined(DB_MEMORY_DEBUG) || defined(DB_PIXEL_BENCH)
#error Matrix benchmark requires its own checked SIMD diagnostic build
#endif
extern "C" volatile uint8_t *db_shared();
/* Model tests set this to one: they validate execution, never hardware speed. */
extern "C" { unsigned zz_matrix_quick=0; }
static constexpr unsigned report=PC_VIDEO+0xe00, cases=6, stride=40, capacity=4096*4;
static_assert(report+16+cases*stride<=PC_VIDEO+0xfa0,"owned report bounds");
alignas(64) static float a[16],b[16],input[capacity];
alignas(64) static float expected[capacity+8],output[capacity+8];
static zz_mat_fn matrices[]={zz_mat_scalar,zz_mat_cmsis,zz_mat_neon};
static zz_vec_fn vertices[]={zz_vec_scalar,zz_vec_cmsis,zz_vec_neon};
static unsigned hash(const float *v,unsigned n) {
    unsigned h=2166136261u;
    const unsigned char *p=(const unsigned char*)v;
    for(unsigned i=0;i<n*4;i++) h=(h^p[i])*16777619u;
    return h;
}
static void inputs(unsigned kind,unsigned n) {
    for(unsigned i=0;i<16;i++){a[i]=(float)((int)(i*13%31)-15)/16; b[i]=(float)((int)(i*7%23)-11)/16;}
    for(unsigned i=0;i<n*4;i++) input[i]=(float)((int)(i*17%127)-63)/64;
    if(kind>=4) {
        memset(a,0,sizeof(a)); a[0]=a[5]=0.875f; a[1]=0.125f; a[4]=-0.125f; a[10]=a[15]=1;
        for(unsigned i=0;i<n;i++) {
            unsigned t=i/3,corner=i%3;
            input[i*4]=((int)(t%32)-16)/16.0f+(corner==1?0.046875f:0.0f);
            input[i*4+1]=((int)((t/32)%32)-16)/16.0f+(corner==2?0.046875f:0.0f);
            input[i*4+2]=(float)(t%8)/16;
            input[i*4+3]=1;
        }
    }
}
static void execute(unsigned kind,unsigned method,unsigned n) {
    if(!kind) matrices[method](a,b,output+4);
    else vertices[method](a,input,output+4,n);
    if(kind>=4) zz_matrix_scene_draw(output+4,n/3);
}
int pc_matrix_benchmark(int (*stop)()) {
    volatile uint8_t *wire=db_shared()+report;
    ad_put(wire,0,0); unsigned errors=0;
    bool opened=false;
    const unsigned sizes[]={4,16,256,1024,384,3072};
    const unsigned repeats[]={8192,256,64,32,4,2};
    for(unsigned c=0;c<cases;c++) {
        if(stop&&stop()) {zz_matrix_scene_close();return 0;}
        unsigned n=sizes[c],len=c?n*4:16,reps=zz_matrix_quick?1:repeats[c];
        inputs(c,n);
        if(c>=4&&!opened) {
            if(!zz_matrix_scene_open()) {zz_matrix_scene_close();return -1;}
            opened=true;
        }
        unsigned inhash=hash(input,n*4)^hash(a,16)^hash(b,16), imagehash=0;
        for(unsigned m=0;m<3;m++) {
            for(unsigned i=0;i<capacity+8;i++) output[i]=12345.0f;
            execute(c,m,n);
            if(!m){memcpy(expected,output,sizeof(output));if(c>=4)imagehash=zz_matrix_scene_hash();}
            else if(memcmp(expected,output,sizeof(output)))errors++;
            for(unsigned i=0;i<4;i++)if(output[i]!=12345.0f)errors++;
            for(unsigned i=len+4;i<capacity+8;i++)if(output[i]!=12345.0f)errors++;
            if(c>=4&&zz_matrix_scene_hash()!=imagehash) errors++;
            if((hash(input,n*4)^hash(a,16)^hash(b,16))!=inhash)errors++;
        }
        uint64_t times[3]={0,0,0};
        /* A/B/C/C/B/A, warm each batch. Validation/hash excluded from timing. */
        const unsigned order[]={0,1,2,2,1,0};
        for(unsigned pass=0;pass<6;pass++) {
            if(stop&&stop()) {zz_matrix_scene_close();return 0;}
            unsigned m=order[pass]; execute(c,m,n);
            uint64_t begin=pc_clock_ticks();
            for(unsigned i=0;i<reps;i++) {execute(c,m,n);__asm__ volatile("":::"memory");}
            times[m]+=pc_clock_ticks()-begin;
        }
        unsigned o=16+c*stride;
        ad_put(wire,o,c);ad_put(wire,o+4,n);ad_put(wire,o+8,reps*2);
        ad_put(wire,o+12,c>=4?imagehash:hash(output+4,len));
        for(unsigned m=0;m<3;m++){ad_put(wire,o+16+m*8,times[m]>>32);ad_put(wire,o+20+m*8,times[m]);}
    }
    zz_matrix_scene_close();
    ad_put(wire,4,errors);ad_put(wire,8,cases);ad_put(wire,12,zz_matrix_quick);
    __asm__ volatile("dsb sy":::"memory");ad_put(wire,0,0x4d415431);
    return errors?-1:1;
}
