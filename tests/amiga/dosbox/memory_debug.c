#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory_plan.h"
static uint8_t ram[2*1024*1024];
static uint32_t rd(uint32_t a,unsigned n){uint32_t v=0;assert(dm_span(a,n,sizeof(ram)));for(unsigned i=0;i<n;i++)v|=(uint32_t)ram[a+i]<<(8*i);return v;}
static void wr(uint32_t a,unsigned n,uint32_t v){assert(dm_span(a,n,sizeof(ram)));for(unsigned i=0;i<n;i++)ram[a+i]=v>>(8*i);}
int main(void){
 struct dm_scan *s=calloc(1,sizeof(*s));struct dm_reply r={0};struct dm_request q={0};
#define RUN() dm_execute(s,&q,&r,sizeof(ram),rd,wr)
 assert(!dm_span(0xffffffff,2,sizeof(ram))&&!dm_span(0x9ffff,2,sizeof(ram))&&!dm_span(0xfffff,1,sizeof(ram)));
 q=(struct dm_request){.op=DM_WRITE,.address=1,.width=2,.value=42};RUN();assert(r.error==DM_RUNNING);
 q.op=DM_PAUSE;RUN();assert(r.paused);
 q.op=DM_WRITE;RUN();assert(!r.error&&rd(1,2)==42);
 q.value=43;RUN();assert(r.error==DM_BAD_REQUEST&&rd(1,2)==42);q.expected=42;RUN();assert(!r.error);
 q=(struct dm_request){.op=DM_SCAN,.address=0,.length=16,.width=2,.value=43};RUN();assert(!r.error&&r.count==1&&s->address[0]==1);
 wr(1,2,44);q.op=DM_REFINE;q.compare=DM_INCREASED;RUN();assert(!r.error&&r.count==1);
 q.compare=DM_CHANGED;RUN();assert(!r.error&&r.count==0);
 q=(struct dm_request){.op=DM_SCAN,.length=5000,.width=1,.compare=DM_UNKNOWN};RUN();assert(r.error==DM_OVERFLOW&&!s->valid&&!s->count);
 q.length=16;RUN();assert(!r.error&&r.count==16);
 q=(struct dm_request){.op=DM_RESULTS};RUN();assert(!r.error&&r.count==16&&r.bytes[15*8+3]==15);
 q=(struct dm_request){.op=DM_READ,.address=sizeof(ram)-1,.length=2};RUN();assert(r.error==DM_RANGE);
 q.length=1;RUN();assert(!r.error&&r.count==1);
 q=(struct dm_request){.op=DM_WRITE,.address=0x9ffff,.width=2};RUN();assert(r.error==DM_RANGE);
 q=(struct dm_request){.op=DM_RESUME};RUN();assert(!r.paused);
 q=(struct dm_request){.op=DM_READ,.address=1,.length=2};RUN();assert(!r.error&&r.bytes[0]==44);
 for(unsigned a=0;a<sizeof(ram);a+=997)for(unsigned n=1;n<=4;n*=2)if(dm_span(a,n,sizeof(ram))){wr(a,n,0x12345678u);assert(rd(a,n)==(0x12345678u&dm_mask(n)));}
 free(s);puts("PASS guest RAM bounds, paused/CAS edits, unaligned scans, refine, overflow, result pages and running snapshots");
}
