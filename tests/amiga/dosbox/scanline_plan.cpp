#include "scanline_plan.h"
#include <cassert>
int main(){
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
