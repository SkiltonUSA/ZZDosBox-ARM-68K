/* GPL-3.0-or-later. No DMAC/SLCR writes, no interrupts or DMA submissions. */
#include "probe_wire.h"
static uint32_t read_reg(uint32_t address)
{ return *(const volatile uint32_t *)address; }
void zz_worker(volatile uint8_t *base)
{
 uint32_t sctlr,midr,mpidr,i,reset,security,controller;
 volatile uint8_t *r=base+DP_REPORT;
 __asm__ volatile("mrc p15,0,%0,c1,c0,0":"=r"(sctlr));
 __asm__ volatile("mrc p15,0,%0,c0,c0,0":"=r"(midr));
 __asm__ volatile("mrc p15,0,%0,c0,c0,5":"=r"(mpidr));
 if((sctlr&0x1005u)||midr!=0x413fc090u||(mpidr&255u)!=1)return;
 ad_put(base,ZZ_DIAG+4,sctlr);ad_put(base,ZZ_DIAG+8,midr);
 ad_put(base,ZZ_DIAG+12,mpidr);ad_put(base,ZZ_DIAG,ZZ_READY);
 __asm__ volatile("dsb sy":::"memory");
 reset=read_reg(0xf800020cu);security=read_reg(0xf8000440u);
 ad_put(r,4,reset);ad_put(r,8,security);
 ad_put(r,12,read_reg(0xf8000444u));
 controller=(security&1u)?0xf8004000u:0xf8003000u;
 ad_put(r,16,controller);
 if(!(reset&1u)) {
  for(i=0;i<DP_WORDS;i++)ad_put(r,32+4*i,read_reg(controller+dp_offsets[i]));
  ad_put(r,20,DP_WORDS);
 }
 __asm__ volatile("dsb sy":::"memory");ad_put(r,0,DP_MAGIC);
 __asm__ volatile("dsb sy":::"memory");
}
