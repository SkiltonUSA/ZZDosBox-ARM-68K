#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <initializer_list>
#include "capture.h"
static uint32_t reference(unsigned c){
 unsigned r=c>>11,g=(c>>5)&63,b=c&31;
 return (((r<<3)|(r>>2))<<16)|(((g<<2)|(g>>4))<<8)|(b<<3)|(b>>2);
}
int main(){
 uint32_t palette[256];for(unsigned i=0;i<256;i++)palette[i]=i*0x1020304u;
 for(unsigned c=0;c<65536;c++)assert((pc_rgb565.low[c&255]|pc_rgb565.high[c>>8])==reference(c));
 for(unsigned w=1;w<=1024;w++)for(unsigned bpp:{8u,15u,16u,32u})for(unsigned shift=0;shift<4;shift++){
  unsigned n=w*((bpp+7)/8);auto raw=(uint8_t*)malloc(n+shift);auto src=raw+shift;
  auto dst=(uint32_t*)malloc((w+2)*4);dst[0]=dst[w+1]=0x12345678;
  for(unsigned i=0;i<n;i++)src[i]=(i*137+w*71+shift*19)&255;
  assert(pc_capture_row(dst+1,src,w,bpp,palette));
  for(unsigned x=0;x<w;x++){
   uint32_t wanted;
   if(bpp==8)wanted=palette[src[x]];
   else if(bpp==15){unsigned v=src[x*2]|(src[x*2+1]<<8);unsigned r=(v>>10)&31,g=(v>>5)&31,b=v&31;wanted=(((r<<3)|(r>>2))<<16)|(((g<<3)|(g>>2))<<8)|(b<<3)|(b>>2);}
   else if(bpp==16)wanted=reference(src[x*2]|(src[x*2+1]<<8));
   else memcpy(&wanted,src+x*4,4);
   assert(dst[x+1]==wanted);
  }
  assert(dst[0]==0x12345678&&dst[w+1]==0x12345678);free(raw);free(dst);
 }
 uint32_t dst=0x12345678;assert(!pc_capture_row(&dst,nullptr,1,24,palette)&&dst==0x12345678);
 puts("PASS all 65536 RGB565 colours, 1024 widths, 8/15/16/32 bpp, four source alignments and guards");
}
