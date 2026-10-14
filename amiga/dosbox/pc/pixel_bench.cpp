/* GPL-3.0-or-later. Diagnostic private cached-RAM timings, NOT RTG bandwidth.
 * Invoked once after SIMD state acquisition. Never part of a normal build. */
#include "pc_wire.h"
#include "profile.h"
#if !defined(DB_PIXEL_BENCH) || !defined(DB_NEON_DISPATCH) || defined(DB_VIDEO_TRACE) || defined(DB_OPENGL)
#error Pixel microbenchmark requires isolated dispatch diagnostic build
#endif
/* Mutually exclusive reuse of the video-trace diagnostic area, owned by app. */
static constexpr unsigned report=PC_VIDEO+0xe00u,count=8,stride=32;
static_assert(report+16+count*stride<=PC_VIDEO+0xfa0u,"diagnostic report bounds");
extern "C" volatile uint8_t *db_shared();
extern "C" void pc_neon_scale(volatile uint32_t*,const uint32_t*,unsigned,unsigned,unsigned);
extern "C" void pc_neon_scale_reference(volatile uint32_t*,const uint32_t*,unsigned,unsigned,unsigned);
using kernel=void(*)(volatile uint32_t*,const uint32_t*,unsigned,unsigned,unsigned);
static uint32_t source[640] __attribute__((aligned(64)));
static uint32_t reference[1282] __attribute__((aligned(64)));
static uint32_t candidate[1282] __attribute__((aligned(64)));
static uint64_t timed(kernel fn,uint32_t *dst,unsigned n,unsigned fmt,unsigned target){
 fn(dst,source,n,fmt,target); // Warm instructions/data before each batch.
 uint64_t begin=pc_clock_ticks();
 for(unsigned i=0;i<1024;i++){
  fn(dst,source,n,fmt,target);
  __asm__ volatile("":::"memory");
 }
 return pc_clock_ticks()-begin;
}
void pc_pixel_benchmark(){
 volatile uint8_t *b=db_shared()+report;ad_put(b,0,0);
 for(unsigned x=0;x<640;x++)source[x]=(x*0x1234567u)^0xa59e32b7u;
 unsigned errors=0,k=0;
 for(unsigned n=320;n<=640;n+=320)for(unsigned ratio=1;ratio<=2;ratio++)for(unsigned fmt=3;fmt<=4;fmt++,k++){
  unsigned target=n*ratio;
  for(unsigned x=0;x<1282;x++)reference[x]=candidate[x]=0xa5a5a5a5u;
  pc_neon_scale_reference(reference+1,source,n,fmt,target);
  pc_neon_scale(candidate+1,source,n,fmt,target);
  for(unsigned x=0;x<1282;x++)if(reference[x]!=candidate[x])errors++;
  for(unsigned x=0;x<target;x++){
   uint32_t expected=source[x/ratio];if(fmt!=3)expected=__builtin_bswap32(expected);
   if(reference[x+1]!=expected)errors++;
  }
  if(reference[0]!=0xa5a5a5a5u||reference[target+1]!=0xa5a5a5a5u)errors++;
  uint64_t a=timed(pc_neon_scale_reference,reference+1,n,fmt,target);
  uint64_t c=timed(pc_neon_scale,candidate+1,n,fmt,target);
  c+=timed(pc_neon_scale,candidate+1,n,fmt,target);
  a+=timed(pc_neon_scale_reference,reference+1,n,fmt,target);
  unsigned o=16+k*stride;
  ad_put(b,o,n);ad_put(b,o+4,target);ad_put(b,o+8,fmt);ad_put(b,o+12,2048);
  ad_put(b,o+16,a>>32);ad_put(b,o+20,a);ad_put(b,o+24,c>>32);ad_put(b,o+28,c);
 }
 ad_put(b,4,errors);ad_put(b,8,count);__asm__ volatile("dsb sy":::"memory");ad_put(b,0,0x50495831u);
}
