/* GPL-2.0-or-later. Bounded read-ahead for an exclusively owned disk image.
 * No deferred writes. Reset before open, invalidate before every write.
 * Transport returns true only for a complete transfer. */
#pragma once
#include <stdint.h>
#include <string.h>
struct DbDiskCache {
 static constexpr unsigned sector_bytes=512, sectors=8;
 uint8_t bytes[sector_bytes*sectors];
 unsigned first=0,count=0;
 void reset(){count=0;}
 template<class Transfer> bool access(unsigned sector,void *data,bool writing,
                                     unsigned total,Transfer transfer){
  if(sector>=total)return false;
  if(writing){reset();return transfer(sector,1,data,true);}
  if(count&&sector>=first&&sector-first<count){
   memcpy(data,bytes+(sector-first)*sector_bytes,sector_bytes);return true;
  }
  reset();first=sector;
  unsigned n=total-sector;if(n>sectors)n=sectors;
  if(!transfer(sector,n,bytes,false)){
   /* A failed speculative read must not prevent a valid single read. */
   if(n==1||!transfer(sector,1,bytes,false))return false;
   n=1;
  }
  count=n;memcpy(data,bytes,sector_bytes);return true;
 }
};
