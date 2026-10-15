/* GPL-2.0-or-later. Bounded read cache for an exclusively owned disk image.
 * Writes remain synchronous. Invalidate the affected block before any write,
 * including failed writes (the device may have changed a partial sector).
 * Transport returns true only for a complete transfer. */
#pragma once
#include <stdint.h>
#include <string.h>
#ifndef DB_DISK_CACHE_SETS
#define DB_DISK_CACHE_SETS 8
#endif
static_assert(DB_DISK_CACHE_SETS>=8 && DB_DISK_CACHE_SETS<=128 && !(DB_DISK_CACHE_SETS&(DB_DISK_CACHE_SETS-1)),"cache sets must be a bounded power of two");
struct DbDiskCache {
 static constexpr unsigned sector_bytes=512, sectors=8, sets=DB_DISK_CACHE_SETS, ways=4;
 struct Line {uint8_t bytes[sector_bytes*sectors];unsigned first=0,count=0,age=0;};
 Line lines[sets][ways];
 unsigned hits=0,misses=0,writes=0,fallbacks=0;
 void reset(){for(auto &set:lines)for(auto &line:set)line.count=0;hits=misses=writes=fallbacks=0;}
 void touch(Line *set,unsigned chosen){
  for(unsigned i=0;i<ways;i++)if(i!=chosen&&set[i].count&&set[i].age<ways-1)set[i].age++;
  set[chosen].age=0;
 }
 template<class Transfer> bool access(unsigned sector,void *data,bool writing,
                                     unsigned total,Transfer transfer){
  if(sector>=total)return false;
  unsigned block=sector/sectors,first=block*sectors;
  Line *set=lines[(block^(block>>8))&(sets-1)];
  if(writing){
   writes++;
   for(unsigned i=0;i<ways;i++)if(set[i].count&&set[i].first/sectors==block)set[i].count=0;
   return transfer(sector,1,data,true);
  }
  for(unsigned i=0;i<ways;i++)if(set[i].count&&sector>=set[i].first&&sector-set[i].first<set[i].count){
   hits++;touch(set,i);memcpy(data,set[i].bytes+(sector-set[i].first)*sector_bytes,sector_bytes);return true;
  }
  misses++;
  unsigned chosen=0;
  for(unsigned i=0;i<ways;i++){if(!set[i].count){chosen=i;break;}if(set[i].age>set[chosen].age)chosen=i;}
  Line &line=set[chosen];line.count=0;
  unsigned n=total-first;if(n>sectors)n=sectors;
  if(!transfer(first,n,line.bytes,false)){
   /* Retry the requested sector, not the start of the speculative block. */
   fallbacks++;first=sector;
   if(n==1||!transfer(sector,1,line.bytes,false))return false;
   n=1;
  }
  line.first=first;line.count=n;touch(set,chosen);
  memcpy(data,line.bytes+(sector-first)*sector_bytes,sector_bytes);return true;
 }
};
