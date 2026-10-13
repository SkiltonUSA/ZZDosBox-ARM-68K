/* Exclusive accounting, nested work, cancellation and timer-low wrap. */
#include <assert.h>
#include <stdio.h>
#include "profile_plan.h"
int main(void){
 struct pc_profile p;uint64_t base=0xfffffff0ull,sum=0;
 pp_init(&p,base);pp_enter(&p,PP_EVENTS,base+10);
 pp_enter(&p,PP_CAPTURE,base+20);pp_leave(&p,base+40);
 pp_enter(&p,PP_GRANT,base+50);pp_leave(&p,base+80);
 pp_enter(&p,PP_COPY,base+90);pp_leave(&p,base+140);
 pp_leave(&p,base+150);pp_enter(&p,PP_CPU,base+160);
 /* A nested guest execution followed by longjmp leaves explicit scopes. */
 pp_enter(&p,PP_CPU,base+170);pp_unwind(&p,base+180);
 pp_finish(&p,base+200);
 assert(p.ticks[PP_OTHER]==40&&p.ticks[PP_EVENTS]==40);
 assert(p.ticks[PP_CAPTURE]==20&&p.ticks[PP_GRANT]==30);
 assert(p.ticks[PP_COPY]==50&&p.ticks[PP_CPU]==20);
 for(unsigned i=0;i<PP_COUNT;i++)sum+=p.ticks[i];
 assert(sum==200&&p.total==sum&&!p.error&&p.unwound==2&&p.calls[PP_CPU]==2);
 pp_init(&p,0);for(unsigned i=0;i<64;i++)pp_enter(&p,PP_CPU,i);
 assert(p.error==1&&p.depth==64);pp_finish(&p,100);assert(p.error==1);
 pp_init(&p,0);pp_leave(&p,1);assert(p.error==2);
 pp_init(&p,0);pp_enter(&p,PP_COUNT,1);assert(p.error==1);
 puts("PASS profile exclusive sum, nested scopes, cancellation unwind, low-word wrap, bounds");
}
