/* GPL-3.0-or-later. Completed ARM frames / real global-timer interval. */
#pragma once
#include <stdint.h>
struct pc_fps {uint64_t ticks;uint32_t frame,tenths;bool sampled;};
static inline void pc_fps_update(pc_fps *f,uint64_t ticks,uint32_t frame,uint32_t hz){
 if(!f->sampled){f->ticks=ticks;f->frame=frame;f->sampled=true;return;}
 uint64_t elapsed=ticks-f->ticks;
 if(hz&&elapsed>=hz/2){
  uint64_t rate=(uint64_t)(frame-f->frame)*hz*10/elapsed;
  f->tenths=rate>9999?9999:(uint32_t)rate;f->ticks=ticks;f->frame=frame;
 }
}
static inline uint8_t pc_fps_glyph(char c,unsigned row){
 static const uint8_t digits[10][7]={{14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},{14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,1,14}};
 static const uint8_t f[]={31,16,16,30,16,16,16},p[]={30,17,17,30,16,16,16},s[]={15,16,16,14,1,1,30};
 if(c>='0'&&c<='9')return digits[c-'0'][row];
 return c=='F'?f[row]:c=='P'?p[row]:c=='S'?s[row]:c=='.'&&row==6?4:0;
}
static inline void pc_fps_draw(volatile uint8_t *dst,unsigned pitch,unsigned format,uint32_t tenths,unsigned width=640){
 char text[]="FPS 000.0";text[4]='0'+tenths/1000%10;text[5]='0'+tenths/100%10;text[6]='0'+tenths/10%10;text[8]='0'+tenths%10;
 if(text[4]=='0'){text[4]=' ';if(text[5]=='0')text[5]=' ';}
 uint32_t white=format==3?0x00ffffffu:0xffffff00u;
 for(unsigned y=0;y<18;y++){
  volatile uint32_t *row=(volatile uint32_t*)(dst+(y+4)*pitch);
  for(unsigned x=0;x<112;x++){
   unsigned col=x>=2?(x-2)/12:9,bit=x>=2?(x-2)%12:10;
   bool on=y>=2&&y<16&&col<9&&bit<10&&(pc_fps_glyph(text[col],(y-2)/2)&(16>>(bit/2)));
   row[width-116+x]=on?white:0;
  }
 }
}
