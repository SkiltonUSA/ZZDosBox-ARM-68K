/* GPL-3.0-or-later. Slow feedback from actual wall time and guest milliseconds.
 * Auto preserves a conservative ceiling; Max permits more emulated work.
 * Neither mode promises a CPU-equivalence or a minimum game frame rate. */
#pragma once
#include <stdint.h>
struct pc_cycle_control {uint32_t wall,ticks,requested;unsigned mode,ready,steady;};
static inline void pc_cycle_init(struct pc_cycle_control*c,unsigned mode,unsigned initial){
 c->wall=c->ticks=0;c->requested=initial;c->mode=mode;c->ready=c->steady=0;
}
static inline unsigned pc_cycle_sample(struct pc_cycle_control*c,uint32_t wall,uint32_t ticks,unsigned request){
 uint32_t elapsed,guest;unsigned next,step,cap=c->mode==2?100000:10000;
 if(!c->mode)return 0;
 /* A keyboard adjustment takes over from automatic control for this session. */
 if(request!=c->requested){c->mode=0;return 0;}
 if(!c->ready){c->ready=1;c->wall=wall;c->ticks=ticks;return 0;}
 elapsed=wall-c->wall;
 if(elapsed<500000)return 0;
 guest=ticks-c->ticks;c->wall=wall;c->ticks=ticks;
 /* Startup/paused sessions and very long stalls are not speed samples. */
 if(!guest||elapsed>2000000||guest>10000){c->steady=0;return 0;}
 next=request;step=request/16;
 if(step<32)step=32;
 /* Audio and display pacing can hold guest time at exactly real time even
  * when compute headroom exists. Probe upward after four healthy samples;
  * do not mistake the pacing dead band for a permanent CPU ceiling. */
 if((uint64_t)guest*1000000u<(uint64_t)elapsed*950u){next=request>step?request-step:500;c->steady=0;}
 else if((uint64_t)guest*1000000u>(uint64_t)elapsed*1030u){next=request+step;c->steady=0;}
 else if(++c->steady>=4){next=request+(step/2);c->steady=0;}
 if(next<500)next=500;
 if(next>cap)next=cap;
 if(next==request)return 0;
 c->requested=next;return next;
}
