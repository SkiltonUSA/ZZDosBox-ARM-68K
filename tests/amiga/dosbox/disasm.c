#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "disasm.h"
int main(void){
 char out[256];unsigned n;uint8_t code[]={0xb8,0x34,0x12,0x90,0xc3};
 n=dm_disassemble(out,sizeof(out),code,sizeof(code),0x100,16);assert(n==3&&strstr(out,"mov")&&strstr(out,"1234"));
 n=dm_disassemble(out,sizeof(out),code,sizeof(code),0x100,32);assert(n==5&&strstr(out,"eax"));
 assert(dm_disassemble(out,sizeof(out),code,1,0,16)==1&&strstr(out,"db"));
 assert(dm_disassemble(out,sizeof(out),code,0,0,16)==0);
 uint8_t prefixes[16];memset(prefixes,0x66,sizeof(prefixes));assert(dm_disassemble(out,sizeof(out),prefixes,16,0,16)==1);
 uint32_t rng=1234567;uint8_t random[15];
 for(unsigned i=0;i<100000;i++){
  for(unsigned j=0;j<15;j++){rng=rng*1664525u+1013904223u;random[j]=rng>>24;}
  unsigned available=i%15+1;
  n=dm_disassemble(out,sizeof(out),random,available,0x8000,i&1?32:16);
  assert(n>=1&&n<=available&&strlen(out)<sizeof(out));
 }
 puts("PASS bounded DOSBox x86 decoder: 16/32-bit, truncation, prefix flood, 100000 random snapshots");
}
