/* GPL-3.0-or-later. Experimental presentation-only cadence controller.
 * All times are A9 global-timer ticks. No guest clock or CPU-cycle changes.
 * Mode 0: every frame, 1: adaptive, 2..4: fixed presentation divisor. */
#pragma once
#include <stdint.h>
#include <string.h>
struct pc_presentation {
 uint64_t end,last_present,work_sum,copy_sum,copy_total;
 uint32_t mode,hz,divisor,remaining,frames,presented,skipped,changes;
 uint32_t samples,copies,recovery,work_mean,copy_mean;
 bool started;
};
static inline void pc_presentation_reset(pc_presentation *s){memset(s,0,sizeof(*s));}
static inline void pc_presentation_history(pc_presentation *s){
 s->work_sum=s->copy_sum=0;s->samples=s->copies=s->recovery=0;
 s->work_mean=s->copy_mean=0;s->remaining=0;
}
static inline bool pc_presentation_begin(pc_presentation *s,unsigned mode,
 unsigned hz,uint64_t now){
 const uint64_t frequency=333333333u;
 if(mode>4)mode=0;
 if(hz!=30&&hz!=50&&hz!=60)hz=60;
 if(!s->started||s->mode!=mode||s->hz!=hz){
  s->mode=mode;s->hz=hz;s->divisor=mode>=2?mode:1;
  pc_presentation_history(s);s->started=false;
 }
 if(s->started){
  uint64_t work=now-s->end;
  /* File dialogs, pauses and long stalls are not steady frame samples. */
  if(work>frequency/4){
   pc_presentation_history(s);
   if(mode==1&&s->divisor!=1){s->divisor=1;s->changes++;}
  }else{
   s->work_sum+=work;s->samples++;
   if(s->samples>=16){
    s->work_mean=(uint32_t)(s->work_sum/s->samples);
    if(s->copies)s->copy_mean=(uint32_t)(s->copy_sum/s->copies);
    uint64_t cost=s->copy_mean,work_mean=s->work_mean;
    uint64_t budget=frequency/hz,current=work_mean+cost/s->divisor;
    /* At least 10% of unskipped cost must be presentation work. */
    if(mode==1&&s->copies&&current>budget&&cost*9>=work_mean){
     s->recovery=0;
     if(s->divisor<4){s->divisor++;s->changes++;}
    }else if(mode==1&&s->divisor>1&&s->copies&&
      ((work_mean+cost/(s->divisor-1))*10<budget*9||cost*19<work_mean)){
     /* Eight stable windows (128 source frames) before each recovery. */
     if(++s->recovery>=8){s->divisor--;s->changes++;s->remaining=0;s->recovery=0;}
    }else s->recovery=0;
    s->samples=s->copies=0;s->work_sum=s->copy_sum=0;
   }
  }
 }
 s->frames++;
 /* Refresh on first frame and at least every 100 ms when frames arrive. */
 if(s->remaining&&s->started&&now-s->last_present<frequency/10){
  s->remaining--;s->skipped++;return false;
 }
 return true;
}
static inline void pc_presentation_end(pc_presentation *s,uint64_t start,
 uint64_t now,bool presented){
 if(presented){
  uint64_t cost=now-start;
  s->presented++;s->copy_total+=cost;
  if(cost>333333333u/4){
   pc_presentation_history(s);
   if(s->mode==1&&s->divisor!=1){s->divisor=1;s->changes++;}
  }else{s->copies++;s->copy_sum+=cost;}
  s->last_present=now;s->remaining=s->divisor-1;
 }
 s->end=now;s->started=true;
}
