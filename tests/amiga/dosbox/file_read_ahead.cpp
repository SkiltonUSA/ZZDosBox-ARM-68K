#include "file_read_ahead.h"
#include <assert.h>
#include <vector>
#include <stdio.h>
struct IO {
 std::vector<uint8_t>&data;unsigned pos=0,reads=0,rewinds=0;bool fail_read=false,fail_seek=false;
 bool read(uint8_t*p,unsigned&n){++reads;if(fail_read)return false;unsigned left=pos<data.size()?data.size()-pos:0;if(n>left)n=left;memcpy(p,data.data()+pos,n);pos+=n;return true;}
 bool rewind(unsigned n){++rewinds;if(fail_seek)return false;assert(n<=pos);pos-=n;return true;}
};
int main(){
 std::vector<uint8_t>d(13001);for(unsigned i=0;i<d.size();i++)d[i]=i%251;
 {IO io{d};DbFileReadAhead c;for(unsigned i=0;i<d.size();i++){uint8_t b;unsigned n=1;assert(c.read(io,&b,n,true)&&n==1&&b==d[i]);}assert(io.reads==4);uint8_t b;unsigned n=1;assert(c.read(io,&b,n,true)&&!n);d.push_back(77);n=1;assert(c.read(io,&b,n,true)&&n==1&&b==77);d.pop_back();}
 {IO io{d};DbFileReadAhead c;uint8_t b[5000];unsigned n=3;assert(c.read(io,b,n,true));assert(io.pos==4096);assert(c.discard(io)&&io.pos==3);io.pos+=100;n=5;assert(c.read(io,b,n,true)&&b[0]==d[103]);assert(c.discard(io));d[108]=99;n=1;assert(c.read(io,b,n,true)&&b[0]==99);assert(io.rewinds==2);n=5000;assert(c.read(io,b,n,true)&&n==5000);for(unsigned i=0;i<n;i++)assert(b[i]==d[109+i]);}
 {IO a{d},b{d};DbFileReadAhead ca,cb;uint8_t out[10];unsigned n=1;assert(ca.read(a,out,n,true));n=2;assert(cb.read(b,out,n,true));assert(ca.discard(a)&&cb.discard(b));d.resize(2);a.pos=a.pos>2?2:a.pos;b.pos=b.pos>2?2:b.pos;n=10;assert(ca.read(a,out,n,true)&&n==1);n=10;assert(cb.read(b,out,n,true)&&n==0);}
 {d.resize(5000,123);IO io{d};DbFileReadAhead c;uint8_t b[5000];unsigned n=1;assert(c.read(io,b,n,true));io.fail_seek=true;assert(!c.discard(io));io.fail_seek=false;assert(c.discard(io));n=1;assert(c.read(io,b,n,true)&&n==1&&b[0]==d[1]);io.fail_read=true;n=5000;assert(!c.read(io,b,n,true)&&n==4095);io.fail_read=false;n=10;assert(c.read(io,b,n,true)&&n==10);}
 {IO io{d};DbFileReadAhead c;uint8_t b[20];unsigned n=1;assert(c.read(io,b,n,true));n=20;assert(c.read(io,b,n,false)&&n==20&&io.pos==21);unsigned reads=io.reads;n=0;assert(c.read(io,b,n,true)&&io.reads==reads);}
 puts("PASS file read-ahead: sequential batching, EOF growth, seek, alias mutation/truncation, failure/retry, disable and zero reads");
}
