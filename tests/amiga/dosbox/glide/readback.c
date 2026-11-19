#include <assert.h>
#include <stdio.h>
#include "amiga/dosbox/glide/probe_readback.h"
static uint16_t pixels[GRX_PROBE_BYTES/2];
static void set(unsigned x,unsigned y,unsigned color) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
 color=__builtin_bswap16(color);
#endif
 pixels[(GRX_PROBE_HEIGHT-1-y)*64+x]=(uint16_t)color;
}
int main(void) {
 unsigned checked;
 for(unsigned scene=0;scene<5;scene++) {
  for(unsigned row=0;row<GRX_PROBE_HEIGHT;row++)for(unsigned x=0;x<64;x++) {
   unsigned y=GRX_PROBE_HEIGHT-1-row,c=0;
   if(scene&&x>=8&&y>=8&&x+y<=64)
    c=scene==1?0xf800:scene==2?0xffe0:x<32?0xf800:scene==3?0x001f:0x8010;
   set(x,y,c);
  }
  assert(grx_probe_verify(pixels,scene,&checked)==0&&checked>3000);
  set(16,12,0x07e0);
  assert(grx_probe_verify(pixels,scene,&checked)==1);
 }
 for(unsigned i=0;i<GRX_PROBE_BYTES/2;i++)pixels[i]=0;
 set(5,12,0xffff);set(7,11,0xf800);
 struct grx_readback_trace trace={0};
 assert(grx_probe_verify_trace(pixels,0,&checked,&trace)==2);
 assert(trace.count==2 && trace.samples[0].offset==(GRX_PROBE_HEIGHT-1-12)*64+5);
 assert(trace.samples[0].got==0xffff && trace.samples[0].want==0);
 assert(trace.samples[1].got==0xf800);
 assert(grx_probe_verify_trace(pixels,0,&checked,NULL)==2);
 for(unsigned i=0;i<GRX_PROBE_BYTES/2;i++)pixels[i]=0xffff;
 assert(grx_probe_verify_trace(pixels,0,&checked,&trace)==4096);
 assert(trace.count==GRX_TRACE_SAMPLES && trace.samples[15].offset==15);
 puts("PASS mismatch trace records values from the original reads");
 puts("PASS shared readback oracle: five scenes, corruption detected in each");
}
