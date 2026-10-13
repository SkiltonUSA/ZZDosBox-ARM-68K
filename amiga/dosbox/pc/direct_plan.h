/* GPL-3.0-or-later. Temporary NC/XN mapping of an OS-owned, locked bitmap.
 * This validates geometry/aperture only; ownership comes from the host's
 * P96 allocation and held lock, challenged at BOTH ends before any write. */
#pragma once
#include <stdint.h>
#define PC_RGB24 1u
#define PC_BGR24 2u
#define PC_BGRA32 3u
#define PC_ARGB32 4u
#define PC_APERTURE_FIRST 0x00200000u
#define PC_APERTURE_END 0x041f0000u
#define PC_PAGE 0x453u /* normal NC, shared, privileged RW, XN */
static inline uint32_t pc_bpp(uint32_t format){return format==1||format==2?3:format==3||format==4?4:0;}
static inline uint32_t pc_span_size(uint32_t addr,uint32_t pitch,uint32_t format,uint32_t width,uint32_t height){
 uint32_t bpp=pc_bpp(format),span;
 if(!bpp || width<320 || width>960 || height<240 || height>720 || (addr&3) || (pitch&3) || pitch<width*bpp || pitch>4096)return 0;
 span=(height-1)*pitch+width*bpp;
 if(addr<PC_APERTURE_FIRST || addr>PC_APERTURE_END-span)return 0;
 return span;
}
static inline int pc_map_size(uint32_t *l1,uint32_t *l2,uint32_t table,uint32_t addr,uint32_t pitch,uint32_t format,uint32_t width,uint32_t height){
 uint32_t span=pc_span_size(addr,pitch,format,width,height),first=addr>>20,last,i,page;
 if(!span || (table&1023))return 0;
 last=(addr+span-1)>>20;
 if(last-first>3)return 0;
 for(i=first;i<=last;i++)if(l1[i])return 0; /* never replace another mapping */
 for(i=0;i<1024;i++)l2[i]=0;
 for(page=addr&~4095u;page<addr+span;page+=4096)
  l2[((page>>20)-first)*256+((page>>12)&255)]=page|PC_PAGE;
 for(i=first;i<=last;i++)l1[i]=(table+(i-first)*1024)|1;
 return 1;
}
static inline void pc_unmap(uint32_t *l1,uint32_t addr,uint32_t span){
 uint32_t first=addr>>20,last=(addr+span-1)>>20;
 for(uint32_t i=first;i<=last;i++)l1[i]=0;
}

static inline uint32_t pc_span(uint32_t addr,uint32_t pitch,uint32_t format){return pc_span_size(addr,pitch,format,640,480);}
static inline int pc_map(uint32_t *l1,uint32_t *l2,uint32_t table,uint32_t addr,uint32_t pitch,uint32_t format){return pc_map_size(l1,l2,table,addr,pitch,format,640,480);}
