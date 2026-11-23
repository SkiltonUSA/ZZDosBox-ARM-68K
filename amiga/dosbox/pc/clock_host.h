/* Called before emulation starts, while the launcher holds Core1 ownership. */
#include "clock_rate.h"
static int pc_clock_sample(uint64_t *ticks,ULONG *midpoint){
 ULONG start=us(),token=(sequence+1)^ad_get(shared,ZZ_CONTROL);
 publish(MB_HOST+8,token);publish(MB_HOST+MB_OP,MB_CLOCK);publish(MB_HOST+MB_SEQ,++sequence);
 do{
  syncio->pull(shared+MB_REPLY,96,NULL);
  if(ad_get(shared+MB_REPLY,MB_ACK)==sequence){
   if(ad_get(shared+MB_REPLY,68)!=(token^ZZ_XOR)||ad_get(shared+MB_REPLY,MB_ERROR))return 0;
   *ticks=(uint64_t)ad_get(shared+MB_REPLY,MB_HI)<<32|ad_get(shared+MB_REPLY,MB_LO);
   *midpoint=start+(ULONG)(us()-start)/2;return 1;
  }
  if(SetSignal(0,SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C)return 0;
  Delay(1);
 }while((ULONG)(us()-start)<2000000u);
 return 0;
}
static int pc_clock_calibrate(void){
 uint64_t first,last;ULONG before,after;
 if(!pc_clock_sample(&first,&before))return 0;
 Delay(100);
 if(!pc_clock_sample(&last,&after)||last<=first)return 0;
 ULONG elapsed=after-before;unsigned hz=pc_timer_rate(last-first,elapsed);
 printf("TIMER estimated_hz=%u selected_hz=%u calibration_us=%u\n",elapsed?(ULONG)((last-first)*1000000u/elapsed):0,hz,elapsed);
 if(!hz)return 0;
 publish(PC_TIMER_HZ,hz);publish(PC_TIMER_MAGIC,PC_TIMER_ABI);return 1;
}
