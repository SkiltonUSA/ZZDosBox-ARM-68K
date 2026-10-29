/* Independent PL330 subset interpreter: no hardware execution claim. */
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include "amiga/dosbox/dma/copy_program.h"

static void exercise(unsigned bytes,unsigned ns,unsigned swap32)
{
 const unsigned src=0x10000008,dst=0x11000008;
 pc_dma_program program{};
 assert(pc_dma_build_copy(&program,{src,bytes},{dst,bytes},src,dst,bytes,ns,swap32));
 assert(program.size<=48);
 std::vector<uint8_t> source(bytes+16),destination(bytes+16,0xa5),fifo;
 for(unsigned i=0;i<source.size();i++)source[i]=(i*71u+(i>>7))&255;
 auto original=source;
 uint32_t reg[3]={},loops[2]={};unsigned pc=0,copied=0,steps=0;bool drained=false,ended=false;
 while(pc<program.size) {
  assert(++steps<10000000);
  unsigned op=program.code[pc];
  if(op==0xbc) {
   unsigned r=program.code[pc+1];assert(r<3&&pc+6<=program.size);
   uint32_t v=0;for(unsigned i=0;i<4;i++)v|=uint32_t(program.code[pc+2+i])<<(8*i);
   reg[r]=v;pc+=6;
  } else if(op==0x20||op==0x22) {
   loops[(op>>1)&1]=unsigned(program.code[pc+1])+1;pc+=2;
  } else if(op==0x38||op==0x3c) {
   unsigned l=(op>>2)&1;assert(loops[l]);
   if(--loops[l]) {assert(program.code[pc+1]<=pc);pc-=program.code[pc+1];}
   else pc+=2;
  } else if(op==4||op==8) {
   uint32_t c=reg[1];
   assert((c&1)&&((c>>14)&1));
   assert(((c>>9)&1)==ns&&((c>>23)&1)==ns);
   assert((c>>28)==2*swap32&&((c>>11)&7)==0&&((c>>25)&7)==0);
   assert(((c>>8)&7)==ns*2&&((c>>22)&7)==ns*2);
   unsigned n=(1u<<((c>>1)&7))*(((c>>4)&15)+1);
   assert(n==(1u<<((c>>15)&7))*(((c>>18)&15)+1)&&n<=128);
   if(op==4) {
    assert(fifo.empty()&&reg[0]>=src&&reg[0]-src+n<=bytes);
    unsigned off=8+reg[0]-src;
    fifo.assign(source.begin()+off,source.begin()+off+n);reg[0]+=n;
    if(swap32)for(unsigned j=0;j<n;j+=4) {
     assert(j+4<=n);
     for(unsigned k=0;k<4;k++)fifo[j+k]=source[off+j+3-k];
    }
   } else {
    assert(fifo.size()==n&&reg[2]>=dst&&reg[2]-dst+n<=bytes);
    memcpy(destination.data()+8+reg[2]-dst,fifo.data(),n);
    reg[2]+=n;copied+=n;fifo.clear();drained=false;
   }
   pc++;
  } else if(op==0x13) {assert(fifo.empty());drained=true;pc++;}
  else {assert(op==0&&drained&&pc+1==program.size);ended=true;break;}
 }
 assert(ended&&copied==bytes&&reg[0]==src+bytes&&reg[2]==dst+bytes);
 assert(source==original);
 for(unsigned i=0;i<bytes;i++)assert(destination[8+i]==source[8+(swap32?(i^3):i)]);
 for(unsigned i=0;i<8;i++)assert(destination[i]==0xa5&&destination[bytes+8+i]==0xa5);
}
int main()
{
 const unsigned sizes[]={1,7,8,127,128,129,255,256,32767,32768,32769,
  65535,65536,256000,307200,1228800,PC_DMA_COPY_LIMIT-1,PC_DMA_COPY_LIMIT};
 for(unsigned n:sizes)for(unsigned ns=0;ns<2;ns++) {
  exercise(n,ns,0);exercise((n+3)&~3u,ns,1);
 }
 uint32_t random=42;for(unsigned i=0;i<128;i++) {
  random=random*1664525u+1013904223u;unsigned n=random%100000+1;
  exercise(n,i&1,0);exercise((n+3)&~3u,i&1,1);
 }
 pc_dma_program p{};pc_dma_span a={0x10000000,PC_DMA_COPY_LIMIT},b={0x11000000,PC_DMA_COPY_LIMIT};
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address,0,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address,PC_DMA_COPY_LIMIT+1,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address+1,b.address,128,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address+1,128,0,0));
 assert(!pc_dma_build_copy(&p,a,a,a.address,a.address+8,128,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address-8,b.address,128,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address+a.bytes-8,b.address,128,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address+b.bytes-8,128,0,0));
 assert(!pc_dma_build_copy(&p,{0xfffffff8,16},b,0xfffffff8,b.address,8,0,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address,128,2,0));
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address,127,0,1));
 assert(!pc_dma_build_copy(&p,a,b,a.address,b.address,128,0,2));
 assert(!p.size);
 puts("PASS DMA program model: exact copies, burst/loop/tail boundaries, guards, bounds and rejection");
}
