/* Local benchmark-only observer for the supplied, unmodified Doom 1.9 EXE.
 * Guest addresses are validated against the loaded executable's code/string.
 * Read-only sampling once per emulated ms; never alters x86 registers or RAM. */
#pragma once
#include <stdint.h>
#include <string.h>
#include "pc_wire.h"
struct DoomClock {
 uint64_t start=0,end=0;
 unsigned first=0,last=0,sim_first=0,sim_last=0,signature=0,rejected=0;
 void tick(const uint8_t *ram,unsigned memory_mb,unsigned sim,uint64_t now){
  if(end||rejected||memory_mb!=8)return;
  if(!signature){
   static const uint8_t code[]={0x53,0x51,0x52,0x56,0x55,0x83,0x3d,0xa4,0xa3,0x1b,0,0,0x74,0x20,0xe8,0xbd,0x7f,0xff,0xff,0x2b,0x05,0x98,0xa3,0x1b,0,0x50,0x8b,0x0d,0xbc,0xa3,0x1b,0,0x51,0x68,0xcc,0x13,0x1b,0,0xe8,0xa5,0x8e,0xff,0xff};
   if(memcmp(ram+0x169c40,code,sizeof(code))||memcmp(ram+0x1b13cc,"timed %i gametics in %i realtics",30))return;
   signature=1;
  }
  uint32_t timing,began,game;
  memcpy(&timing,ram+0x1ba3a4,4);memcpy(&began,ram+0x1ba398,4);memcpy(&game,ram+0x1ba3bc,4);
  if(timing!=1||!began)return;
  if(!start){if(game>1){rejected=1;return;}start=now;first=game;sim_first=sim;}
  if(game>=2134){if(game!=2134){rejected=1;return;}end=now;last=game;sim_last=sim;}
 }
 void publish(volatile uint8_t *b)const {
  const unsigned o=PC_VIDEO+0xd00;
  const uint32_t words[]={0x44424d31u,signature,rejected,first,last,sim_first,sim_last,
    (uint32_t)(start>>32),(uint32_t)start,(uint32_t)(end>>32),(uint32_t)end};
  for(unsigned i=0;i<11;i++)ad_put(b,o+i*4,words[i]);
  __asm__ volatile("dsb sy":::"memory");
 }
};
