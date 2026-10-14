/* GPL-3.0-or-later. Exclusive nested times; no timer reads when disabled. */
#include "experiment.h"
#include <string.h>
unsigned zz_tgl_neon, zz_tgl_profile, zz_tgl_faults;
uint64_t zz_tgl_times[ZZ_STAGES];
unsigned zz_tgl_calls[ZZ_STAGES];
static struct { uint64_t start,children; unsigned stage; } stack[8];
static unsigned depth;
void zz_tgl_enter(unsigned stage) {
    if(depth>=8 || stage>=ZZ_STAGES) {zz_tgl_faults++;return;}
    stack[depth].start=zz_tgl_clock();stack[depth].children=0;stack[depth++].stage=stage;
    zz_tgl_calls[stage]++;
}
void zz_tgl_leave(void) {
    uint64_t now=zz_tgl_clock();
    if(!depth) {zz_tgl_faults++;return;}
    --depth;uint64_t elapsed=now-stack[depth].start;
    if(elapsed<stack[depth].children){zz_tgl_faults++;return;}
    zz_tgl_times[stack[depth].stage]+=elapsed-stack[depth].children;
    if(depth)stack[depth-1].children+=elapsed;
}
void zz_tgl_profile_begin(void) {
    depth=0;zz_tgl_faults=0;memset(zz_tgl_times,0,sizeof(zz_tgl_times));memset(zz_tgl_calls,0,sizeof(zz_tgl_calls));
    zz_tgl_profile=1;zz_tgl_enter(ZZ_OTHER);
}
uint64_t zz_tgl_profile_end(void) {
    zz_tgl_leave();zz_tgl_profile=0;
    if(depth)zz_tgl_faults++;
    uint64_t sum=0;for(unsigned i=0;i<ZZ_STAGES;i++)sum+=zz_tgl_times[i];return sum;
}
