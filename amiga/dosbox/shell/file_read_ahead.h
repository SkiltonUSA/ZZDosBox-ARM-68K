/* GPL-2.0-or-later. Bounded per-open-file read-ahead; writes stay synchronous.
 * Transport read() updates count; rewind() moves backwards by unread bytes.
 * Discard all open-file buffers BEFORE a mutation: AmigaDOS may move other
 * handles' positions when truncating, so lazy invalidation is insufficient.
 * No EOF caching: a later read can observe a file that has grown. */
#pragma once
#include <stdint.h>
#include <string.h>
class DbFileReadAhead {
 uint8_t bytes[4096];
 unsigned at=0,end=0;
public:
 template<class IO> bool discard(IO &io){
  if(end!=at&&!io.rewind(end-at))return false;
  at=end=0;return true;
 }
 template<class IO> bool read(IO &io,uint8_t *out,unsigned &count,bool enabled){
  unsigned wanted=count,done=0;count=0;
  if(!wanted)return true;
  if(!enabled&&!discard(io))return false;
  while(done<wanted){
   if(at!=end){
    unsigned n=end-at;if(n>wanted-done)n=wanted-done;
    memcpy(out+done,bytes+at,n);at+=n;done+=n;
   }else{
    at=end=0;
    unsigned n=wanted-done;if(n>sizeof(bytes))n=sizeof(bytes);
    bool ahead=enabled&&n<sizeof(bytes);
    unsigned request=ahead?sizeof(bytes):n,got=request;
    if(!io.read(ahead?bytes:out+done,got)){count=done;return false;}
    if(!got)break;
    if(ahead)end=got;
    else {done+=got;if(got<request)break;}
   }
  }
  count=done;return true;
 }
};
