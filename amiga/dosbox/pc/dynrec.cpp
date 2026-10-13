/* GPL-2.0-or-later. Core1-local JIT coherency; never touches shared L2. */
#include "dosbox.h"
#include "dynrec_plan.h"
#include "pc_wire.h"
#include <stddef.h>
extern "C" volatile uint8_t *db_shared();
extern "C" size_t fb_heap_peak();
static unsigned commits,selftest;
extern "C" void *db_dynrec_alloc(unsigned size) {
 uint32_t base=(uint32_t)db_shared(),at=db_jit_begin(base);
 if(!db_jit_span(base,at,size))E_Exit("JIT allocation bound");
 return (void *)at;
}
extern "C" void db_dynrec_commit(const void *ptr,unsigned size) {
 uint32_t at=(uint32_t)ptr,base=(uint32_t)db_shared();
 if(!db_jit_span(base,at,size))E_Exit("JIT commit bound");
 uint32_t end=at+size,zero=0;
 /* Private pages are inner WB/WA, outer NC. A9 L1 lines are 32 bytes,
  * checked by the launcher's CCSIDR preflight. DCCMVAU then local ICIMVAU. */
 for(uint32_t p=at&~31u;p<end;p+=32)
  __asm__ volatile("mcr p15,0,%0,c7,c11,1"::"r"(p):"memory");
 __asm__ volatile("dsb sy":::"memory");
 for(uint32_t p=at&~31u;p<end;p+=32)
  __asm__ volatile("mcr p15,0,%0,c7,c5,1"::"r"(p):"memory");
 __asm__ volatile("mcr p15,0,%0,c7,c5,6\n dsb sy\n isb"::"r"(zero):"memory");
 commits++;
}
extern "C" void db_dynrec_preflight() {
 uint32_t sctlr;__asm__ volatile("mrc p15,0,%0,c1,c0,0":"=r"(sctlr));
 if(sctlr&2u)E_Exit("JIT requires normal-memory unaligned access");
 volatile uint32_t *code=(volatile uint32_t *)db_dynrec_alloc(8);
 for(unsigned n=41;n<=43;n++) {
  code[0]=0xe3a00000u|n;code[1]=0xe12fff1eu;
  db_dynrec_commit((const void *)code,8);
  if(((unsigned (*)())code)()!=n)E_Exit("JIT instruction-cache witness");
 }
 selftest=1;
}
extern unsigned db_dynrec_dispatches;
extern "C" void db_dynrec_report() {
 volatile uint8_t *p=db_shared()+PC_VIDEO+0xd40;
 ad_put(p,0,0x4a495431u);ad_put(p,4,commits);ad_put(p,8,selftest);
 ad_put(p,12,db_dynrec_dispatches);ad_put(p,16,DB_JIT_CACHE);
 ad_put(p,20,(unsigned)fb_heap_peak());
 __asm__ volatile("dsb sy":::"memory");
}
