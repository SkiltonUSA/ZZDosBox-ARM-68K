/* ASan/UBSan event model: copy/PAN overlap, ACK ordering and lock lifetime. */
#include <assert.h>
#include <stdio.h>
#include "present_plan.h"
static unsigned random_value=314159;
static unsigned next(void){random_value=random_value*1664525u+1013904223u;return random_value;}
static unsigned simulate(unsigned limit,unsigned frames,unsigned copy_delay,unsigned pan_delay){
 struct nf_slots s;struct pc_flow p;nf_init(&s);pc_flow_init(&p,limit);
 unsigned time=0,produced=0,done=0,copy_until=0,pan_until=0,release=0;
 unsigned generations[3]={0},copy_slot=0,overlap=0,lock=0,burst=0;
 uint16_t ack=65530;
 while(done<frames){
  assert(time++<frames*100);
  if(s.pending>=0&&time>=pan_until){
   assert(generations[s.pending]==done+1);done++;
   unsigned prev=s.front;
   assert(nf_ack(&s,(uint16_t)(ack+2))==-1&&s.front==prev);
   assert(nf_ack(&s,ack)==0&&s.front==prev);
   ack++;assert(nf_ack(&s,ack)==1);
  }
  if(p.copying){
   assert(lock&&copy_slot==s.render&&copy_slot!=s.front&&(int)copy_slot!=s.pending);
   assert(!pc_flow_release(&p,&s));assert(!pc_flow_grant(&p,&s));
   if(time>=copy_until){generations[copy_slot]=produced;assert(pc_flow_complete(&p));assert(!pc_flow_complete(&p));}
  }
  if(p.ready){
   assert(lock&&generations[s.render]>done);
   if(s.pending<0){assert(pc_flow_queue(&p,&s,ack));pan_until=time+pan_delay+(next()%3);}
   else assert(!pc_flow_queue(&p,&s,ack));
  }
  if(lock&&pc_flow_idle(&p,&s)){
   assert(pc_flow_release(&p,&s));lock=0;release++;assert(burst<=limit);burst=0;
  }
  if(produced<frames&&pc_flow_can_grant(&p,&s)){
   if(!lock)lock=1;
   if(s.pending>=0)overlap++;
   assert(pc_flow_grant(&p,&s));copy_slot=s.render;
   produced++;burst++;copy_until=time+copy_delay+(next()%3);
  }
  if(s.pending>=0||p.ready||p.copying)assert(!pc_flow_release(&p,&s));
 }
 assert(produced==frames&&!p.copying&&!p.ready&&s.pending<0);
 assert(release>0&&(!overlap||limit==2));
 if(limit==2)assert(overlap>0);
 return time;
}
static void fault(void){
 struct nf_slots s;struct pc_flow p;nf_init(&s);pc_flow_init(&p,2);
 assert(pc_flow_grant(&p,&s));assert(pc_flow_complete(&p));assert(pc_flow_queue(&p,&s,123));
 assert(pc_flow_grant(&p,&s));assert(p.copying&&s.render==2);
 /* A timeout cancels the pending PAN, but cannot release a copying bitmap. */
 s.pending=-1;assert(!pc_flow_release(&p,&s));
 /* Copy may finish during stop; its completed buffer still cannot be freed. */
 assert(pc_flow_complete(&p));assert(!pc_flow_release(&p,&s));
 /* Only synchronous worker reset permits unconditional OS cleanup. */
 nf_init(&s);pc_flow_init(&p,2);assert(pc_flow_release(&p,&s));
}
int main(void){
 unsigned runs=0;
 for(unsigned c=1;c<=7;c++)for(unsigned p=1;p<=7;p++)for(unsigned limit=1;limit<=2;limit++){
  simulate(limit,1000,c,p);runs++;
 }
 simulate(2,70000,1,5);fault();
 printf("PASS %u latency schedules, 168000 frames, ACK wrap, slot exclusivity, bounded batches, timeout/stop lock retention\n",runs);
}
