#include "scanline_plan.h"
#include <cassert>
int main(){
 for(unsigned mode=0;mode<=4;mode++)for(unsigned y=0;y<16;y++){
  unsigned expected=mode==1||mode==2?(y%2?0:8):mode==3?(y%2?5:8):mode==4?(y%4==0?8:y%4==2?4:6):8;
  assert(pc_scan_level(mode,y)==expected);
 }
 for(unsigned c=0;c<256;c++)for(unsigned level=0;level<=8;level++){
  uint32_t p=c|((255-c)<<8)|(((c*3)&255)<<16)|(((c*7)&255)<<24),expected=0;
  for(unsigned k=0;k<4;k++)expected|=((((p>>(k*8))&255)*level)/8)<<(k*8);
  assert(pc_scan_dim_pixel(p,level)==expected);
 }
 pc_scan_state s={};
 for(unsigned pass=0;pass<5;pass++)for(unsigned slot=0;slot<3;slot++)
  assert(pc_scan_begin(&s,2,1,0x200000+slot*0x140000,2560,640,480,3)==(pass==0));
 assert(pc_scan_begin(&s,0,2,0x200000,2560,640,480,3));
 assert(pc_scan_begin(&s,2,3,0x200000,2560,640,480,3));
 assert(!pc_scan_begin(&s,2,3,0x200000,2560,640,480,3));
 assert(pc_scan_begin(&s,2,3,0x200000,3840,640,480,3));
 assert(pc_scan_begin(&s,2,3,0x200000,3840,640,480,4));
 assert(pc_scan_begin(&s,2,4,0x200000,3840,640,480,4));
 uint32_t row[643];for(auto &v:row)v=0xa5a5a5a5;
 pc_scan_black(row+1,640);
 assert(row[0]==0xa5a5a5a5&&row[641]==0xa5a5a5a5&&row[642]==0xa5a5a5a5);
 for(unsigned i=1;i<=640;i++)assert(!row[i]);
}
