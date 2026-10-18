#include "descent_clock.h"
#include <cassert>
#include <vector>
#include <cstdio>
static void put(std::vector<uint8_t>&b,unsigned at,uint32_t n){memcpy(b.data()+at,&n,4);}
static void fixture(std::vector<uint8_t>&b,unsigned code,unsigned counter){
 const uint8_t head[]={0x53,0x51,0x52,0x56,0x57,0x55,0x81,0xec,0x90,0,0,0,0x89,0xc5,0x83,0x3d};
 const uint8_t tail[]={0x81,0xc4,0x90,0,0,0,0x5d,0x5f,0x5e,0x5a,0x59,0x5b,0xc3,0x8d,0x40,0};
 memcpy(b.data()+code,head,sizeof(head));memcpy(b.data()+code+428,tail,sizeof(tail));
 b[code+417]=0xe8;b[code+422]=0xff;b[code+423]=5;put(b,code+424,counter);
}
int main(){
 std::vector<uint8_t>b(8*1024*1024);const unsigned code=0x123456,counter=0x345678;
 DescentClock absent;absent.tick(b.data(),100,0,1);assert(!absent.address);
 fixture(b,code,counter);DescentClock c;c.tick(b.data(),b.size(),0,1);assert(c.address==counter&&!c.start&&!c.warm);
 put(b,counter,2);c.tick(b.data(),b.size(),1,1);assert(c.warm&&!c.start);
 put(b,counter,52);c.tick(b.data(),b.size(),5001,5*c.hz+1);assert(c.start&&!c.end&&c.first==52);
 put(b,counter,352);c.tick(b.data(),b.size(),35001,35*c.hz+1);assert(c.end&&c.last-c.first==300&&!c.rejected);
 uint32_t report[12];c.report(report);assert(report[0]==0x44434c31&&report[1]==counter&&report[4]-report[3]==300);
 DescentClock reset;reset.tick(b.data(),b.size(),0,1);put(b,counter,1);reset.tick(b.data(),b.size(),1,2);
 // Returning to a menu/reset must not silently continue an old sample.
 put(b,counter,2);reset.tick(b.data(),b.size(),2,3);assert(reset.rejected);
 DescentClock changed;put(b,counter,2);changed.tick(b.data(),b.size(),0,1);b[code]=0;
 changed.tick(b.data(),b.size(),1,2);assert(changed.rejected);
 fixture(b,code,counter);fixture(b,0x124000,counter);DescentClock duplicate;duplicate.tick(b.data(),b.size(),0,1);assert(duplicate.rejected&&!duplicate.address);
 b[0x124000]=0;put(b,code+424,b.size()-3);DescentClock bound;bound.tick(b.data(),b.size(),0,1);assert(!bound.address);
 puts("PASS Descent clock: relocation, warmup, elapsed frames, reset, code change, ambiguity, bounds");
}
