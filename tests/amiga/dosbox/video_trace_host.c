/* GPL-3.0-or-later. Test the actual reader against overwrite/in-flight writes. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "video_trace.h"
typedef uint32_t ULONG;
static uint32_t storage[(PC_BOOT_TEXT+4096)/4];
static volatile uint8_t *shared=(volatile uint8_t*)storage;
static unsigned mutate;
static void pull(volatile void *p,size_t n,void *user){(void)p;(void)n;(void)user;}
static const struct ad_io io={pull,0,0,0,0},*syncio=&io;
static ULONG receive(unsigned o){
 if(mutate&&o==pc_trace_slot(mutate)){
  ad_put(shared,o,mutate+PC_TRACE_COUNT);mutate=0;
 }
 return ad_get(shared,o);
}
#include "video_trace_host.h"
static void record(unsigned serial){
 unsigned o=pc_trace_slot(serial);
 ad_put(shared,o,serial);ad_put(shared,o+4,PT_RENDER);
 ad_put(shared,o+8,1);ad_put(shared,o+12,3);
 ad_put(shared,o+16,0);ad_put(shared,o+20,serial*37);
 ad_put(shared,PC_TRACE,serial);
}
int main(void){
 video_trace_poll();assert(trace_seen==0);
 record(1);video_trace_poll();assert(trace_seen==1);
 record(2);ad_put(shared,pc_trace_slot(2),0);
 video_trace_poll();assert(trace_seen==1); /* Uncommitted fields. */
 record(2);mutate=2;video_trace_poll();assert(trace_seen==1); /* Torn sample. */
 record(2);video_trace_poll();assert(trace_seen==2);
 for(unsigned i=3;i<=40;i++)record(i);
 video_trace_poll();assert(trace_seen==40); /* Bounded overrun recovery. */
 record(41);video_trace_poll();assert(trace_seen==41);
 puts("PASS trace reader: empty, commit, torn sample, overrun and recovery");
}
