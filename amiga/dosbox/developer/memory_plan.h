/* GPL-3.0-or-later. Bounded guest-physical RAM debugger; no host addresses. */
#pragma once
#include <stdint.h>
#include <string.h>
#define DM_BYTES 256u
#define DM_MATCHES 4096u
#define DM_SCAN_MAX 65536u
enum {DM_STATUS=1,DM_PAUSE,DM_RESUME,DM_READ,DM_WRITE,DM_SCAN,DM_REFINE,DM_RESULTS,DM_STOP};
enum {DM_OK=0,DM_BAD_REQUEST,DM_RUNNING,DM_RANGE,DM_OVERFLOW,DM_NO_SCAN,DM_OFFLINE};
enum {DM_EQUAL=0,DM_UNKNOWN,DM_CHANGED,DM_UNCHANGED,DM_INCREASED,DM_DECREASED};
struct dm_request {uint32_t op,address,length,width,value,compare,index,expected;};
struct dm_reply {uint32_t error,paused,ram_bytes,count,eip,cs,esp,flags;uint8_t bytes[DM_BYTES];};
struct dm_scan {uint32_t count,width,valid;uint32_t address[DM_MATCHES],value[DM_MATCHES];};
typedef uint32_t (*dm_read_fn)(uint32_t,unsigned);
typedef void (*dm_write_fn)(uint32_t,unsigned,uint32_t);
/* Exclude legacy video/ROM holes, overflow, and the end of assigned guest RAM. */
static inline int dm_span(uint32_t address,uint32_t length,uint32_t ram){
 return length && address<ram && length<=ram-address &&
        (address>=0x100000u || (address<0xa0000u && length<=0xa0000u-address));
}
static inline int dm_width(unsigned n){return n==1||n==2||n==4;}
static inline uint32_t dm_mask(unsigned n){return n==1?255u:n==2?65535u:0xffffffffu;}
static inline int dm_compare(unsigned mode,uint32_t now,uint32_t before,uint32_t value){
 switch(mode){case DM_EQUAL:return now==value;case DM_UNKNOWN:return 1;
 case DM_CHANGED:return now!=before;case DM_UNCHANGED:return now==before;
 case DM_INCREASED:return now>before;case DM_DECREASED:return now<before;default:return 0;}
}
static inline void dm_execute(struct dm_scan *scan,const struct dm_request *q,
 struct dm_reply *r,uint32_t ram,dm_read_fn read,dm_write_fn write){
 uint32_t i,n;r->error=DM_OK;r->count=0;r->ram_bytes=ram;memset(r->bytes,0,sizeof(r->bytes));
 if(q->op==DM_STATUS)return;
 if(q->op==DM_PAUSE){r->paused=1;return;}
 if(q->op==DM_RESUME){r->paused=0;return;}
 if(q->op==DM_READ){
  if(q->length>DM_BYTES||!dm_span(q->address,q->length,ram)){r->error=DM_RANGE;return;}
  for(i=0;i<q->length;i++)r->bytes[i]=(uint8_t)read(q->address+i,1);
  r->count=q->length;return;
 }
 if(!r->paused){r->error=DM_RUNNING;return;}
 if(q->op==DM_WRITE){
  if(!dm_width(q->width)||!dm_span(q->address,q->width,ram)||q->value>dm_mask(q->width)){r->error=DM_RANGE;return;}
  /* Compare before writing: stale UI values must not silently overwrite RAM. */
  if(read(q->address,q->width)!=q->expected){r->error=DM_BAD_REQUEST;return;}
  write(q->address,q->width,q->value);r->count=1;return;
 }
 if(q->op==DM_SCAN){
  scan->valid=0;scan->count=0;
  if(!dm_width(q->width)||q->length>DM_SCAN_MAX||q->length<q->width||
     !dm_span(q->address,q->length,ram)||q->compare>DM_UNKNOWN||q->value>dm_mask(q->width)){
   r->error=DM_RANGE;return;
  }
  scan->width=q->width;
  /* Byte stride also finds unaligned words/dwords; no read beyond range. */
  for(i=0;i<=q->length-q->width;i++){
   uint32_t value=read(q->address+i,q->width);
   if(!dm_compare(q->compare,value,0,q->value))continue;
   if(scan->count==DM_MATCHES){scan->count=0;r->error=DM_OVERFLOW;return;}
   n=scan->count++;scan->address[n]=q->address+i;scan->value[n]=value;
  }
  scan->valid=1;r->count=scan->count;return;
 }
 if(q->op==DM_REFINE){
  if(!scan->valid){r->error=DM_NO_SCAN;return;}
  if(q->compare>DM_DECREASED||q->compare==DM_UNKNOWN||q->value>dm_mask(scan->width)){r->error=DM_BAD_REQUEST;return;}
  n=0;for(i=0;i<scan->count;i++){
   uint32_t value=read(scan->address[i],scan->width);
   if(dm_compare(q->compare,value,scan->value[i],q->value)){
    scan->address[n]=scan->address[i];scan->value[n++]=value;
   }
  }
  scan->count=n;r->count=n;return;
 }
 if(q->op==DM_RESULTS){
  if(!scan->valid){r->error=DM_NO_SCAN;return;}
  if(q->index>scan->count){r->error=DM_RANGE;return;}
  n=scan->count-q->index;if(n>DM_BYTES/8)n=DM_BYTES/8;
  for(i=0;i<n;i++){
   uint32_t a=scan->address[q->index+i],v=scan->value[q->index+i];unsigned j;
   for(j=0;j<4;j++){r->bytes[i*8+j]=(uint8_t)(a>>(24-8*j));r->bytes[i*8+4+j]=(uint8_t)(v>>(24-8*j));}
  }
  r->count=n;return;
 }
 r->error=DM_BAD_REQUEST;
}
