#include "presentation_plan.h"
#include <cassert>
#include <cstdio>
static constexpr uint64_t ms=333333;
static bool frame(pc_presentation &s,unsigned mode,uint64_t work,uint64_t copy,unsigned hz=60){
 uint64_t start=s.end+work;
 bool show=pc_presentation_begin(&s,mode,hz,start);
 pc_presentation_end(&s,start,start+(show?copy:0),show);
 assert(s.frames==s.presented+s.skipped);
 assert(s.divisor>=1&&s.divisor<=4);
 return show;
}
int main(){
 pc_presentation s{};
 for(int i=0;i<100;i++)assert(frame(s,0,10*ms,20*ms));
 assert(s.skipped==0&&s.divisor==1);
 pc_presentation_reset(&s);
 for(int i=0;i<100;i++)assert(frame(s,2,5*ms,5*ms)==(i%2==0));
 assert(s.presented==50&&s.skipped==50);
 pc_presentation_reset(&s);
 for(int i=0;i<160;i++)frame(s,1,10*ms,20*ms);
 assert(s.divisor==3&&s.skipped>50&&s.changes==2);
 unsigned changes=s.changes;
 for(int i=0;i<64;i++)frame(s,1,2*ms,ms);
 assert(s.divisor==3&&s.changes==changes); // no rapid quality oscillation
 for(int i=0;i<300;i++)frame(s,1,2*ms,ms);
 assert(s.divisor==1);
 pc_presentation_reset(&s);
 for(int i=0;i<200;i++)assert(frame(s,1,60*ms,ms)); // CPU-bound: leave quality alone
 assert(s.divisor==1);
 pc_presentation_reset(&s);
 for(int i=0;i<200;i++)frame(s,1,12*ms,70*ms);
 assert(s.divisor==4); // bounded even if target cannot be met
 assert(frame(s,1,500*ms,ms)); // pause forces a refresh and resets history
 assert(s.divisor==1);
 assert(frame(s,1,ms,15000*ms)); // a long grant must not overflow averages
 assert(s.divisor==1&&s.copies==0&&s.copy_mean==0);
 pc_presentation_reset(&s);
 assert(frame(s,4,ms,ms));
 assert(frame(s,4,101*ms,ms)); // minimum refresh, even in fixed mode
 assert(frame(s,0,ms,ms)); // switching off forces immediate frame
 pc_presentation_reset(&s);
 s.end=0xfffffff0ULL;
 for(int i=0;i<64;i++)frame(s,1,10*ms,20*ms); // beyond 32-bit timer rollover
 assert(s.end>0x100000000ULL&&s.divisor==3);
 pc_presentation_reset(&s);
 for(int i=0;i<100;i++)assert(frame(s,1,10*ms,20*ms,30));
 assert(s.divisor==1); // requested budget applied
 assert(frame(s,999,ms,ms,999)&&s.mode==0&&s.hz==60);
 puts("PASS presentation: baseline, fixed cadence, overload, slow recovery, CPU-bound gate, pause, refresh, rollover, bounds");
}
