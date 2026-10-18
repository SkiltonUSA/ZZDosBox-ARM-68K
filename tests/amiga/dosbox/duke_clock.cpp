#include "duke_clock.h"
#include <cassert>
#include <vector>
#include <cstdio>
static void put(std::vector<uint8_t>&b,unsigned at,uint32_t n){memcpy(b.data()+at,&n,4);}
static void fixture(std::vector<uint8_t>&b,unsigned code,unsigned counter){
 const uint8_t head[]={0x53,0x51,0x52,0x56,0x83,0xec,0x0c,0x8b,0x35};
 const uint8_t body[]={0x39,0xd6,0x74,0x50,0x89,0xf3,0xb8,0,0x1e,0,0,0x29,0xd3,0x89,0xc2,0xc1,0xfa,0x1f,0xf7,0xfb};
 const uint8_t step[]={0x40,0x83,0xe0,0x3f,0xa3};
 const uint8_t tail[]={0x83,0xc4,0x0c,0x5e,0x5a,0x59,0x5b,0xc3};
 memcpy(b.data()+code,head,sizeof(head));memcpy(b.data()+code+31,body,sizeof(body));
 b[code+13]=b[code+115]=0xa1;memcpy(b.data()+code+120,step,sizeof(step));memcpy(b.data()+code+129,tail,sizeof(tail));
 for(unsigned at:{14u,116u,125u})put(b,code+at,counter);
}
int main(){
 std::vector<uint8_t>b(8*1024*1024);const unsigned code=0x123456,counter=0x345678;
 DukeClock absent;absent.tick(b.data(),100,0,1);assert(!absent.address);
 fixture(b,code,counter);fixture(b,0x180000,0x76c4); // Unrelocated file buffer is ignored.
 put(b,counter,62);DukeClock c;c.tick(b.data(),b.size(),0,1);assert(c.address==counter&&!c.last);
 put(b,counter,0);c.tick(b.data(),b.size(),1,2);assert(c.last==2&&c.warm&&!c.start);
 put(b,counter,1);c.tick(b.data(),b.size(),5001,5*c.hz+2);assert(c.start&&c.first==3);
 for(unsigned i=1;i<=1800;i++){put(b,counter,(1+i)&63);c.tick(b.data(),b.size(),5001+i*1000/60,5*c.hz+2+((uint64_t)i*c.hz)/60);}
 assert(c.end&&!c.rejected&&c.last-c.first==1800&&c.sim_last-c.sim_first==30000);
 uint32_t report[12];c.report(report);assert(report[0]==0x444b4331&&report[4]-report[3]==1800);
 DukeClock changed;changed.tick(b.data(),b.size(),0,1);b[code]=0;changed.tick(b.data(),b.size(),1,2);assert(changed.rejected==2);
 fixture(b,code,counter);put(b,counter,64);DukeClock invalid;invalid.tick(b.data(),b.size(),0,1);assert(invalid.rejected==3);
 put(b,counter,0);fixture(b,0x190000,counter);DukeClock duplicate;duplicate.tick(b.data(),b.size(),0,1);assert(duplicate.rejected==1&&!duplicate.address);
 b[0x190000]=0;fixture(b,code,b.size()-3);DukeClock bound;bound.tick(b.data(),b.size(),0,1);assert(!bound.address);
 fixture(b,code,counter);put(b,code+125,counter+4);DukeClock inconsistent;inconsistent.tick(b.data(),b.size(),0,1);assert(!inconsistent.address);
 puts("PASS Duke clock: relocated code, file-buffer exclusion, ring wraps, 30-second sample, code change, ambiguity, bounds");
}
