/* GPL-2.0-or-later. Differential test of the actual upstream PIC queue.
 * Compare the complete output with cache off/on; no Amiga speed claims.
 * Exercises reuse/removal, equal deadlines, fractional/negative delays,
 * recurring handlers, millisecond rebasing and cycle changes inside callbacks.
 */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "dosbox.h"
#include "cpu.h"
#include "pic.h"
#include "timer.h"
#include "setup.h"
extern void PIC_Init(Section*);
extern void PIC_Destroy(Section*);
extern void zz_pic_deadline_case(float, Bits);
static uint64_t digest=1469598103934665603ull;
static unsigned callbacks,recurring;
static void mix(uint32_t n){digest=(digest^n)*1099511628211ull;}
static void event(Bitu value){
 ++callbacks;mix(value);mix(PIC_Ticks);mix(PIC_TickIndexND());
 // Recompute a previously cached queue head under a different CPU budget.
 if(value==17)CPU_CycleMax=CPU_CycleMax==3000?6000:3000;
}
static void repeat(Bitu value){
 event(value);
 if(++recurring<40)PIC_AddEvent(repeat,0.03125f,value+1);
}
int test_pic_queue(){
 // Fractional boundaries, the fast-path limits and negative fallback cases.
 const float edges[]={-2.0f,-0.0001f,0.0f,0.0001f,0.99999994f,1.0f,
  1.00000012f,6.5535998f,6.5536f,6.5536003f,65.536f,100.0f};
 const Bits indices[]={-100000,-1,0,1,999,1000,2999,3000,65535,65536,65537,100000};
 for(float when:edges)for(Bits index:indices)zz_pic_deadline_case(when,index);
 uint32_t seed=0x9020;
 for(unsigned i=0;i<20000;i++){
  seed=seed*1664525u+1013904223u;
  float when=(int32_t)(seed%2000001)-1000000;
  when/=10000.0f;
  seed=seed*1664525u+1013904223u;
  zz_pic_deadline_case(when,(int32_t)(seed%200001)-100000);
 }
 const float delays[]={-0.01f,0.0f,0.0001f,0.125f,0.125f,
  0.33333334f,0.5f,nextafterf(1.0f,0.0f),1.0f,
  nextafterf(1.0f,2.0f),1.0001f,7.75f,80.25f};
 const int budgets[]={1000,3000,6000,9999,500};
 for(unsigned trial=0;trial<5;trial++){
  PIC_Init(nullptr);CPU_CycleMax=budgets[trial];CPU_CycleLeft=CPU_CycleMax;
  CPU_Cycles=0;CPU_SetFlags(0,FMASK_ALL);recurring=0;
  for(unsigned i=0;i<sizeof(delays)/sizeof(*delays);i++)PIC_AddEvent(event,delays[i],i+10);
  PIC_AddEvent(repeat,0.03125f,100);
  unsigned steps=0;
  while(PIC_Ticks<100){
   assert(++steps<200000);
   bool run=PIC_RunQueue();mix(run);mix(CPU_Cycles);mix(CPU_CycleLeft);mix(PIC_Ticks);
   if(!run){
    TIMER_AddTick();
    if(PIC_Ticks%3==0)CPU_CycleMax=budgets[(PIC_Ticks/3)%5];
    if(PIC_Ticks%7==0){
     PIC_RemoveSpecificEvents(event,777);
     PIC_AddEvent(event,0.875f,777);
     PIC_AddEvent(event,0.875f,778); // stable order for equal deadlines
     PIC_AddEvent(event,0.00001f,17);
    }
    if(PIC_Ticks==50){PIC_RemoveEvents(event);PIC_AddEvent(event,0.0f,999);}
   }else{
    // Emulate short decoder returns as well as an exhausted cycle slice.
    int consumed=(steps%11==0)?CPU_Cycles:1+(steps%97);
    if(consumed>CPU_Cycles)consumed=CPU_Cycles;
    CPU_Cycles-=consumed;
   }
  }
  PIC_RemoveEvents(event);PIC_RemoveEvents(repeat);PIC_Destroy(nullptr);
  printf("PIC trial=%u steps=%u callbacks=%u trace=%016llx\n",trial,steps,callbacks,(unsigned long long)digest);
 }
 assert(callbacks>300);puts("PASS PIC queue trace");return 0;
}
