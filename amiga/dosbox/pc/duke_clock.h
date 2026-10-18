/* Read-only benchmark for supplied Duke Nukem 3D shareware 1.3D.
 * DNRATE enables tics(), which advances a 64-entry history once per game render.
 * Recognize its relocated code/data, unwrap the index each guest millisecond,
 * settle five wall seconds, then measure thirty and request normal shutdown.
 * No game-memory/register writes; never included in the normal player. */
#pragma once
#include <stdint.h>
#include <string.h>
struct DukeClock {
 static const uint64_t hz=333333333u;
 uint64_t warm=0,start=0,end=0;
 uint32_t address=0,code=0,rejected=0,first=0,last=0,sim_first=0,sim_last=0,samples=0;
 uint32_t next_scan=0,index=0;
 static uint32_t read(const uint8_t*p){uint32_t n;memcpy(&n,p,4);return n;}
 static bool signature(const uint8_t*p){
  static const uint8_t head[]={0x53,0x51,0x52,0x56,0x83,0xec,0x0c,0x8b,0x35};
  static const uint8_t body[]={0x39,0xd6,0x74,0x50,0x89,0xf3,0xb8,0,0x1e,0,0,0x29,0xd3,0x89,0xc2,0xc1,0xfa,0x1f,0xf7,0xfb};
  static const uint8_t step[]={0x40,0x83,0xe0,0x3f,0xa3};
  static const uint8_t tail[]={0x83,0xc4,0x0c,0x5e,0x5a,0x59,0x5b,0xc3};
  return !memcmp(p,head,sizeof(head))&&p[13]==0xa1&&!memcmp(p+31,body,sizeof(body))&&p[115]==0xa1&&
   !memcmp(p+120,step,sizeof(step))&&!memcmp(p+129,tail,sizeof(tail))&&read(p+14)==read(p+116)&&read(p+14)==read(p+125);
 }
 void tick(const uint8_t*ram,uint32_t size,uint32_t sim,uint64_t now){
  if(end||rejected)return;
  if(!address){
   if(sim<next_scan||size<0x100000u+137u)return;
   next_scan=sim+1000;
   const uint32_t ceiling=size<0x400000u?size:0x400000u;
   for(uint32_t at=0x100000;at+137u<=ceiling;at++){
    if(ram[at]!=0x53||!signature(ram+at))continue;
    uint32_t operand=read(ram+at+14);
    if(operand<0x100000u||operand>size-4)continue; // Exclude unrelocated EXE file buffers.
    if(address){rejected=1;address=0;return;}
    address=operand;code=at;
   }
   if(!address)return;
   index=read(ram+address);
  }
  if(!signature(ram+code)){rejected=2;return;}
  uint32_t current=read(ram+address);
  if(current>63||index>63){rejected=3;return;}
  last+=(current-index)&63;index=current;
  if(last<2)return; // Wait for DNRATE; startup/menu presents do not count.
  if(!warm){warm=now;return;}
  if(!start){if(now-warm<5*hz)return;start=now;first=last;sim_first=sim;}
  sim_last=sim;samples++;
  if(now-start>=30*hz)end=now;
 }
 void report(uint32_t*w)const{
  const uint32_t values[]={0x444b4331,address,rejected,first,last,sim_first,sim_last,
   (uint32_t)(start>>32),(uint32_t)start,(uint32_t)(end>>32),(uint32_t)end,samples};
  memcpy(w,values,sizeof(values));
 }
};
