/* Model a disk independently; verify bytes, persistence, errors and RPC count. */
#include "disk_cache.h"
#include <assert.h>
#include <stdio.h>
#include <vector>
int main(){
 constexpr unsigned total=19;
 std::vector<uint8_t> disk(total*512);
 for(unsigned i=0;i<disk.size();i++)disk[i]=(i/512)*17+i%251;
 DbDiskCache cache;unsigned calls=0;bool fail_batch=false,fail_all=false;
 auto transfer=[&](unsigned sector,unsigned n,void *data,bool writing){
  calls++;assert(n&&n<=8&&sector<total&&n<=total-sector);
  if(fail_all||(fail_batch&&n>1))return false;
  if(writing){assert(n==1);memcpy(disk.data()+sector*512,data,512);}
  else memcpy(data,disk.data()+sector*512,n*512);
  return true;
 };
 uint8_t buffer[514];buffer[0]=0xa5;buffer[513]=0x5a;
 auto read=[&](unsigned sector){
  assert(cache.access(sector,buffer+1,false,total,transfer));
  assert(!memcmp(buffer+1,disk.data()+sector*512,512));
  assert(buffer[0]==0xa5&&buffer[513]==0x5a);
 };
 for(unsigned i=0;i<total;i++)read(i);
 assert(calls==3); // 19 sequential sectors, including the short final batch.
 unsigned sequential_calls=calls;
 for(unsigned i=16;i<total;i++)read(i);
 assert(calls==3);
 memset(buffer+1,0x37,512);
 assert(cache.access(17,buffer+1,true,total,transfer));
 assert(disk[17*512]==0x37&&calls==4); // Written through before returning.
 read(17);assert(calls==5);
 // A failing write can partially change real media: always discard old data.
 fail_all=true;assert(!cache.access(17,buffer+1,true,total,transfer));
 disk[17*512]=0x91;fail_all=false;read(17);assert(calls==7);
 // Reopening a different image must discard all cached data.
 disk[17*512]=0x49;cache.reset();read(17);assert(calls==8);
 cache.reset();fail_batch=true;read(0);assert(calls==10);
 read(0);assert(calls==10); // Single-sector fallback is cached.
 cache.reset();fail_all=true;
 assert(!cache.access(0,buffer+1,false,total,transfer));
 fail_all=false;fail_batch=false;disk[0]=0x53;read(0);
 unsigned before=calls;
 assert(!cache.access(total,buffer+1,false,total,transfer));
 assert(!cache.access(UINT32_MAX,buffer+1,true,total,transfer));
 assert(calls==before);
 // Repeated random access with writes compared against authoritative media.
 uint32_t rng=1;
 for(unsigned i=0;i<10000;i++){
  rng=rng*1664525u+1013904223u;unsigned sector=rng%total;
  if((rng&31)==0){memset(buffer+1,rng>>24,512);assert(cache.access(sector,buffer+1,true,total,transfer));}
  else read(sector);
 }
 // A write to another block must preserve hot metadata.
 cache.reset();read(0);read(16);before=calls;
 memset(buffer+1,0x61,512);assert(cache.access(17,buffer+1,true,total,transfer));
 read(0);assert(calls==before+1);read(17);assert(calls==before+2);
 // Failed speculative reads away from a block boundary retry the requested sector.
 cache.reset();fail_batch=true;before=calls;read(7);assert(calls==before+2);
 read(7);assert(calls==before+2);fail_batch=false;
 // Exercise more blocks than the whole cache and same-set conflicts.
 {
  constexpr unsigned blocks=DbDiskCache::sets*DbDiskCache::ways*2+1;
  constexpr unsigned large_total=blocks*8;
  DbDiskCache big;unsigned transfers=0;uint8_t data[512];
  auto tx=[&](unsigned sector,unsigned n,void *p,bool w){
   assert(!w&&n&&n<=8&&sector+n<=large_total);transfers++;
   for(unsigned i=0;i<n*512;i++)((uint8_t*)p)[i]=(sector+i/512)*29+(i%512)%251;
   return true;
  };
  auto check=[&](unsigned sector){
   assert(big.access(sector,data,false,large_total,tx));
   for(unsigned i=0;i<512;i++)assert(data[i]==(uint8_t)(sector*29+i%251));
  };
  for(unsigned round=0;round<20;round++)for(unsigned block=0;block<blocks;block++)check(block*8+round%8);
  assert(big.misses>blocks); // Replacement really occurred at this capacity.
  big.reset();transfers=0;
  for(unsigned round=0;round<100;round++)for(unsigned block=0;block<16;block++)check(block*8+round%8);
  assert(transfers==16&&big.hits==1584&&big.misses==16);
  printf("PASS synthetic 16-block alternating workload: 1600 reads / %u transfers (not a Windows FPS benchmark)\n",transfers);
 }
 printf("PASS disk cache: 19 sequential sectors / %u transfers; random reads/writes, EOF, reopen, errors, guards\n",sequential_calls);
}
