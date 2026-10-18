/* Read-only, opt-in benchmark for unmodified Descent shareware 1.4 / DOS32A.
 * Recognizes render_frame's prologue/epilogue, then reads the relocated
 * FrameCount operand. It counts 3D renders, not VGA refresh callbacks.
 * Five seconds settle, thirty seconds measured; native tests use fake ticks.
 * Neither guest state nor the normal release binary is changed. */
#pragma once
#include <stdint.h>
#include <string.h>
struct DescentClock {
 static const uint64_t hz=333333333u;
 uint64_t warm=0,start=0,end=0;
 uint32_t address=0,code=0,rejected=0,first=0,last=0,sim_first=0,sim_last=0,samples=0;
 uint32_t next_scan=0;
 static uint32_t read(const uint8_t*p){uint32_t n;memcpy(&n,p,4);return n;}
 static bool signature(const uint8_t *p){
  static const uint8_t head[]={0x53,0x51,0x52,0x56,0x57,0x55,0x81,0xec,0x90,0,0,0,0x89,0xc5,0x83,0x3d};
  static const uint8_t tail[]={0x81,0xc4,0x90,0,0,0,0x5d,0x5f,0x5e,0x5a,0x59,0x5b,0xc3,0x8d,0x40,0};
  return !memcmp(p,head,sizeof(head)) && p[417]==0xe8 && p[422]==0xff && p[423]==5 && !memcmp(p+428,tail,sizeof(tail));
 }
 void tick(const uint8_t*ram,uint32_t size,uint32_t sim,uint64_t now){
  if(end||rejected)return;
  if(!address){
   if(sim<next_scan||size<0x100000u+444u)return;
   next_scan=sim+1000;
   const uint32_t ceiling=size<0x400000u?size:0x400000u;
   for(uint32_t at=0x100000;at+444u<=ceiling;at++){
    if(ram[at]!=0x53||!signature(ram+at))continue;
    uint32_t operand=read(ram+at+424);
    if(operand>size-4||operand<0x100000u)continue;
    if(address){rejected=1;address=0;return;}
    address=operand;code=at;
   }
   if(!address)return;
  }
  uint32_t count=read(ram+address);
  if(count<2)return; // Pilot/menu/loading are not rendered game frames.
  if(!warm){warm=now;last=count;return;}
  if(count<last||!signature(ram+code)){rejected=2;return;}
  last=count;
  if(!start){if(now-warm<5*hz)return;start=now;first=count;sim_first=sim;}
  sim_last=sim;samples++;
  if(now-start>=30*hz)end=now;
 }
 void report(uint32_t*w)const{
  const uint32_t values[]={0x44434c31,address,rejected,first,last,sim_first,sim_last,
   (uint32_t)(start>>32),(uint32_t)start,(uint32_t)(end>>32),(uint32_t)end,samples};
  memcpy(w,values,sizeof(values));
 }
};
