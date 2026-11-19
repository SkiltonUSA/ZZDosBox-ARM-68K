/* Independent read-only Core1 witness. Never submits GPU commands, changes
 * MMU/L2, or writes framebuffer memory. All GPU work belongs to Core0. */
#include "witness_wire.h"
#include "probe_readback.h"
static void barrier(void){__asm__ volatile("dsb sy" ::: "memory");}
static uint32_t ticks(void){return *(volatile uint32_t *)0xf8f00200u;}
static void publish(volatile uint8_t *base,unsigned seq,unsigned kind,unsigned error,const unsigned *quick) {
 volatile uint8_t *e=base+GW_EARLY;
 ad_put(e,GW_EARLY_ERROR,error);ad_put(e,GW_EARLY_KIND,kind);
 for(unsigned k=0;k<4;k++)ad_put(e,GW_EARLY_SAMPLES+4*k,kind==2&&!error?quick[k]:0xffffffffu);
 barrier();ad_put(e,GW_EARLY_SEQ,seq);barrier();
}
void zz_worker(volatile uint8_t *base) {
 uint32_t sctlr,midr,mpidr,seq=0;
 __asm__ volatile("mrc p15,0,%0,c1,c0,0":"=r"(sctlr));
 __asm__ volatile("mrc p15,0,%0,c0,c0,0":"=r"(midr));
 __asm__ volatile("mrc p15,0,%0,c0,c0,5":"=r"(mpidr));
 if((sctlr&0x1005)!=0x1000||(mpidr&255)!=1||!(*(volatile uint32_t *)0xf8f00208u&1))return;
 ad_put(base,ZZ_DIAG+4,sctlr);ad_put(base,ZZ_DIAG+8,midr);ad_put(base,ZZ_DIAG+12,mpidr);
 barrier();ad_put(base,ZZ_DIAG,ZZ_READY);barrier();
 while(!ad_get(base,ZZ_STOP)) {
  volatile uint8_t *c=base+GW_CMD,*r=base+GW_REPLY;
  unsigned next=ad_get(c,GW_SEQ);if(next==seq)continue;
  barrier();unsigned scene=ad_get(c,GW_SCENE),marker=ad_get(c,GW_MARKER);
  unsigned offset=ad_get(c,GW_OFFSET),count=ad_get(c,GW_COUNT),idle=ad_get(c,GW_IDLE),reverse=ad_get(c,GW_REVERSE),sample=ad_get(c,GW_SAMPLE),publication=ad_get(c,GW_PUBLISH);
  if(next!=seq+1||ad_get(c,GW_NONCE)!=(next^ad_get(base,ZZ_CONTROL))||
     publication>2||(publication==2&&!sample)||(publication&&idle)||sample>1||reverse>1||idle>1||scene>=5||!marker||marker>65535||!count||count>64||
     offset>GRX_PROBE_BYTES/2-count)return;
  seq=next;ad_put(r,GW_ARMED,seq);barrier();
  /* No pixel reads until the host has written the GPU submit register. */
  while(ad_get(base,GW_GO)!=seq)if(ad_get(base,ZZ_STOP))return;
  barrier();const volatile uint16_t *pixels=(const volatile uint16_t *)GRX_PROBE_COLOR;
  unsigned start=ticks(),polls=0,i,checked=0,bad=0,error=0;
  if(!idle)do {
   if(ad_get(base,ZZ_STOP))return;
   barrier();for(i=0;i<count&&pixels[offset+i]==marker;i++){}
   ++polls;if(i==count)break;
   /* Bounded watchdog in raw ticks, not a claimed clock calibration. */
   if((uint32_t)(ticks()-start)>500000000u){error=1;break;}
  }while(1);
  unsigned quick[4];
  if(publication==1)publish(base,seq,publication,error,0);
  if(!error&&!idle&&sample){barrier();grx_probe_sample(pixels,quick);}
  if(publication==2)publish(base,seq,publication,error,quick);
  unsigned elapsed=ticks()-start;
  unsigned verify_start=ticks();struct grx_readback_trace trace;trace.count=0;
  if(!error&&!idle){barrier();bad=grx_probe_verify_order(pixels,scene,&checked,&trace,reverse);}
  unsigned verify_ticks=ticks()-verify_start;
  for(unsigned k=0;k<4;k++)ad_put(r,GW_SAMPLES+4*k,(!error&&!idle&&sample)?quick[k]:0xffffffffu);
  ad_put(r,GW_FIRST_OFFSET,trace.count?trace.samples[0].offset:0xffffffffu);
  ad_put(r,GW_FIRST_GOT,trace.count?trace.samples[0].got:0);
  ad_put(r,GW_FIRST_WANT,trace.count?trace.samples[0].want:0);
  ad_put(r,GW_VERIFY_TICKS,verify_ticks);
  ad_put(r,GW_ERROR,error);ad_put(r,GW_BAD,bad);ad_put(r,GW_CHECKED,checked);
  ad_put(r,GW_POLLS,polls);ad_put(r,GW_TICKS,elapsed);
  barrier();ad_put(r,GW_DONE,seq);barrier();
 }
}
