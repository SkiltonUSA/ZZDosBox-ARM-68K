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
 printf("PASS disk cache: 19 sequential sectors / %u transfers; random reads/writes, EOF, reopen, errors, guards\n",sequential_calls);
}
