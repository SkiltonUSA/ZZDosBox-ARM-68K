/* GPL-3.0-or-later. Included by the 68k host and its transport test. */
#include "video_trace.h"
static ULONG trace_seen;
static void video_trace_poll(void){
 ULONG latest=receive(PC_TRACE),pending=latest-trace_seen;unsigned emitted=0;
 if(!pending)return;
 if(pending>PC_TRACE_COUNT){
  printf("VIDEO_TRACE lost=%u\n",pending-PC_TRACE_COUNT);
  trace_seen=latest-PC_TRACE_COUNT;
 }
 while(trace_seen!=latest){
  ULONG serial=trace_seen+1,o=pc_trace_slot(serial),commit,phase,frame,detail,hi,lo;
  syncio->pull(shared+o,PC_TRACE_STRIDE,NULL);commit=ad_get(shared,o);
  phase=ad_get(shared,o+4);frame=ad_get(shared,o+8);detail=ad_get(shared,o+12);
  hi=ad_get(shared,o+16);lo=ad_get(shared,o+20);
  /* Re-read the commit after copying fields: never print a torn record. */
  if(commit!=serial||receive(o)!=serial)break;
  printf("VIDEO_TRACE serial=%u phase=%s frame=%u detail=%u ticks=%08x%08x\n",
         serial,pc_trace_name(phase),frame,detail,hi,lo);
  trace_seen=serial;emitted++;
 }
 if(emitted||pending>PC_TRACE_COUNT)fflush(stdout);
}
