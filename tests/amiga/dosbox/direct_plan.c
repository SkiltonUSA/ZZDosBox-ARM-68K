/* Native ASan/UBSan: descriptor geometry, no clobber, format bytes and guards. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "direct_plan.h"
static uint32_t l1[4096],l2[1026];
int main(void){
 unsigned cases=0;
 for(unsigned fmt=1;fmt<=4;fmt++)for(unsigned pitch=640*pc_bpp(fmt);pitch<=4096;pitch+=4){
  uint32_t span=479*pitch+640*pc_bpp(fmt);
  uint32_t addresses[]={PC_APERTURE_FIRST,0x00fffffcu,0x00f80004u,PC_APERTURE_END-span};
  for(unsigned n=0;n<4;n++){
   uint32_t addr=addresses[n],first=addr>>20,last=(addr+span-1)>>20;
   memset(l1,0,sizeof(l1));memset(l2,0xa5,sizeof(l2));l1[0xfff]=0xdeadbeef;
   assert(pc_span(addr,pitch,fmt)==span);
   assert(pc_map(l1,l2+1,0x10000000,addr,pitch,fmt));
   assert(l2[0]==0xa5a5a5a5 && l2[1025]==0xa5a5a5a5);
   for(unsigned i=0;i<4096;i++)assert(l1[i]==(i==0xfff?0xdeadbeef:i>=first&&i<=last?(0x10000000+(i-first)*1024)|1:0));
   for(unsigned i=0;i<1024;i++){
    uint32_t page=(first<<20)+i*4096;
    uint32_t want=page>=(addr&~4095u) && page<addr+span ? page|PC_PAGE:0;
    assert(l2[i+1]==want);
   }
   assert(!pc_map(l1,l2+1,0x10000000,addr,pitch,fmt));
   pc_unmap(l1,addr,span);
   for(unsigned i=0;i<4096;i++)assert(l1[i]==(i==0xfff?0xdeadbeef:0));
   assert(!pc_span(PC_APERTURE_END-span+4,pitch,fmt));
   cases++;
  }
 }
 assert(!pc_span(0x1ffffc,768,PC_RGB24));assert(!pc_span(0xffffffff,768,1));
 assert(!pc_span(0x200001,768,1));assert(!pc_span(0x200000,769,1));
 assert(!pc_span(0x200000,764,1));assert(!pc_span(0x200000,4097,1));
 assert(!pc_span(0x200000,1024,0));assert(!pc_span(0x200000,1024,5));
 assert(!pc_map(l1,l2+1,0x10000001,0x200000,1024,3));
 printf("PC direct mapping: %u geometry, boundary, no-clobber and unmap cases PASS\n",cases);
}
