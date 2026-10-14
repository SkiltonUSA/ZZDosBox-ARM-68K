/* GPL-2.0-or-later. Exact scanline conversion; arbitrary source alignment. */
#pragma once
#include <stdint.h>
#include <string.h>
#ifdef DB_NEON
extern "C" void pc_neon_capture(uint32_t *,const uint8_t *,unsigned,unsigned);
#endif
struct pc_rgb565_tables {
 uint32_t low[256],high[256];
 constexpr pc_rgb565_tables():low{},high{} {
  for(unsigned i=0;i<256;i++){
   unsigned blue=i&31,red=i>>3,green=i&7;
   low[i]=((i>>5)<<10)|(blue<<3)|(blue>>2);
   high[i]=(((red<<3)|(red>>2))<<16)|(((green<<5)|(green>>1))<<8);
  }
 }
};
static constexpr pc_rgb565_tables pc_rgb565;
static inline bool pc_capture_row(uint32_t *dst,const uint8_t *src,
 unsigned width,unsigned bpp,const uint32_t *palette){
#ifdef DB_NEON
 if(bpp==16||bpp==32){pc_neon_capture(dst,src,width,bpp);return true;}
#endif
 switch(bpp){
 case 8:
  for(unsigned x=0;x<width;x++)dst[x]=palette[src[x]];
  return true;
 case 15:
  for(unsigned x=0;x<width;x++){
   unsigned v=src[x*2]|((unsigned)src[x*2+1]<<8),r=(v>>10)&31,g=(v>>5)&31,b=v&31;
   dst[x]=(((r<<3)|(r>>2))<<16)|(((g<<3)|(g>>2))<<8)|(b<<3)|(b>>2);
  }
  return true;
 case 16:
  for(unsigned x=0;x<width;x++)dst[x]=pc_rgb565.low[src[x*2]]|pc_rgb565.high[src[x*2+1]];
  return true;
 case 32:memcpy(dst,src,width*4);return true;
 default:return false;
 }
}
