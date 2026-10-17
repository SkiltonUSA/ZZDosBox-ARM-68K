#include <assert.h>
#include <stdio.h>
#include "../../../amiga/dosbox/pc/cycle_plan.h"
#include "../../../amiga/dosbox/pc/paged_plan.h"
int main(void){
 struct pc_cycle_control c;unsigned n;
 pc_cycle_init(&c,1,3000);assert(!pc_cycle_sample(&c,0xfffffff0,0,3000));
 n=pc_cycle_sample(&c,499984,250,3000);assert(n<3000&&n>=2800);
 assert(!pc_cycle_sample(&c,999984,750,n));
 n=pc_cycle_sample(&c,1499984,1500,n);assert(n>2800);
 assert(!pc_cycle_sample(&c,1999984,1800,5000)&&!c.mode);
 pc_cycle_init(&c,2,100000);pc_cycle_sample(&c,1,0,100000);assert(!pc_cycle_sample(&c,500001,700,100000));
 pc_cycle_init(&c,2,500);pc_cycle_sample(&c,1,0,500);assert(!pc_cycle_sample(&c,500001,10,500));
 pc_cycle_init(&c,1,3000);pc_cycle_sample(&c,1,0,3000);assert(!pc_cycle_sample(&c,3000001,5,3000));
 pc_cycle_init(&c,2,3000);pc_cycle_sample(&c,1,0,3000);
 for(unsigned i=1;i<4;i++)assert(!pc_cycle_sample(&c,1+i*500000,i*500,3000));
 assert(pc_cycle_sample(&c,2000001,2000,3000)>3000); /* pacing still permits a headroom probe */
 for(unsigned op=0;op<256;op++)for(unsigned mr=0;mr<256;mr++){
  unsigned len=pc_paged_register_length(op,mr,1);assert(len<=6);
  if((op<=0x3b&&(op&7)<=3)||(op>=0x80&&op<=0x8b)||(op>=0xc0&&op<=0xd3)){
   if((mr&0xc0)!=0xc0)assert(!len);
  }
  if(op>=0xd8&&op<=0xdf)assert(!len);
 }
 assert(pc_paged_register_length(0xb8,0,1)==5&&pc_paged_register_length(0xb8,0,0)==3);
 assert(!pc_paged_register_length(0x66,0xc0,1));assert(!pc_paged_register_length(0xff,0xc0,1));
 puts("PASS cycle feedback: wrap, dead band, ceilings, manual override; register-only paging opcode boundaries");
}
