/* GPL-2.0-or-later. Private JIT instrumentation; counters stay in ARM cached RAM. */
#include "dosbox.h"
#include "mem.h"
#include "pc_wire.h"
#include "jit_hot.h"
#include <stdlib.h>
extern "C" volatile uint8_t *db_shared();
extern unsigned db_memory_mb;
struct Hot { unsigned key,count; };
static Hot *table;
static unsigned used,dropped,sink,sampled;
static const unsigned slots=16384;
unsigned *db_jit_hot_counter(unsigned pc){
 if(!table){table=(Hot*)calloc(slots,sizeof(Hot));if(!table)E_Exit("JIT profile allocation");}
 unsigned key=pc+1,index=(pc*2654435761u)&(slots-1);
 for(unsigned i=0;i<slots;i++,index=(index+1)&(slots-1)){
  Hot &h=table[index];
  if(!h.key){h.key=key;used++;return &h.count;}
  if(h.key==key)return &h.count;
 }
 dropped++;return &sink;
}
void db_jit_hot_reset(){
 if(table)for(unsigned i=0;i<slots;i++)table[i].count=0;
 sink=0;sampled=1;
}
void db_jit_hot_report(){
 const unsigned o=PC_VIDEO+0xe60;static_assert(o+404<=PC_BOOT_TEXT,"JIT hot report bounds");
 volatile uint8_t*b=db_shared();unsigned top[16]={},total=0;
 if(table)for(unsigned i=0;i<slots;i++){
  total+=table[i].count;
  for(unsigned j=0;j<16;j++)if(table[i].count>(top[j]?table[top[j]-1].count:0)){
   for(unsigned k=15;k>j;k--)top[k]=top[k-1];top[j]=i+1;break;
  }
 }
 ad_put(b,o+400,sampled);ad_put(b,o,0x484f5431);ad_put(b,o+4,used);ad_put(b,o+8,dropped);ad_put(b,o+12,total);
 for(unsigned j=0;j<16;j++){
  unsigned pc=top[j]?table[top[j]-1].key-1:0,count=top[j]?table[top[j]-1].count:0,r=o+16+j*24;
  ad_put(b,r,pc);ad_put(b,r+4,count);
  for(unsigned k=0;k<4;k++){
   unsigned v=0;if(count&&pc<=db_memory_mb*1048576u-16)for(unsigned n=0;n<4;n++)v=v*256+MemBase[pc+k*4+n];
   ad_put(b,r+8+k*4,v);
  }
 }
}
