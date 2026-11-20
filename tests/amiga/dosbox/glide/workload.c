#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "workload.h"
extern int grx_check_firmware(const grx_packet *);
static uint16_t pixels[640*480];
int main(void){
 grx_packet p;unsigned checked;
 assert(!grx_workload_packet(&p,0,0));assert(!grx_workload_packet(&p,5,0));
 for(unsigned profile=1;profile<=4;profile++){
  unsigned w=grx_workload_width(profile),h=grx_workload_height(profile);
  for(unsigned batch=0;batch<65;batch++){
   assert(grx_workload_packet(&p,profile,batch));assert(grx_check_firmware(&p));
   assert(p.config.width==w&&p.config.height==h&&p.words<=248);
  }
  assert(!grx_workload_packet(&p,profile,65));
  memset(pixels,0,sizeof(pixels));assert(grx_workload_verify(pixels,profile,&checked)==640&&checked==960);
  for(unsigned row=0;row<16;row++)for(unsigned col=0;col<20;col++){
   unsigned x=col*(w/20),y=row*(h/16);
   pixels[(h-1-(y+2))*w+x+2]=0xf800;
   pixels[(h-1-(y+2))*w+x+1+3*(w/20-3)/4]=0x001f;
  }
  assert(grx_workload_verify(pixels,profile,&checked)==0&&checked==960);
  pixels[(h-1-2)*w+2]=0;
  assert(grx_workload_verify(pixels,profile,&checked)==1);
 }
 puts("PASS 260 workload packets through firmware validator; dimensions, texture bounds and pixel verifier");
}
