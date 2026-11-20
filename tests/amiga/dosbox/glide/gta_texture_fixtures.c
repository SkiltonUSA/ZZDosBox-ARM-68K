/* Exercise the GTA alpha/cutout path using analytic, non-game pixels. */
#include "probe_scene.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned word(const grx_packet *p,unsigned i){unsigned v=0;for(unsigned b=0;b<4;b++)v=v<<8|p->bytes[i*4+b];return v;}
int main(int argc,char **argv){
 unsigned linear=argc>1?atoi(argv[1]):0,offset=0;
 for(unsigned s=0;s<5;s++){
  grx_packet p;assert(grx_probe_scene(&p,s,0x4120+s));
  fprintf(stderr,"%u %u\n",offset,p.words+7);offset+=p.words+7;
  for(unsigned i=0;i<p.words;i++){
   unsigned v=word(&p,i);
   if(s==3&&i){
    unsigned prev=word(&p,i-1);
    if(prev==0x10000000)v|=8;
    if(prev==0x10000003)v|=0x2028;
    if(prev==0x1000000c)v=0x1044|(linear?0xc00:0);
   }
   printf("%08x\n",v);
  }
  printf("90000004\n00000000\n60000000\n50000008\n00000000\nabcddcba\n%08x\n",s+1);
 }
}
