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
 puts("PASS shared readback oracle: five scenes, corruption detected in each");
}
