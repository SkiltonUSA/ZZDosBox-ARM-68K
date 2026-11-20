#include <assert.h>
#include <stdio.h>
#include "translator.h"
static unsigned reg(const grx_packet *p,unsigned r){
 for(unsigned i=2;i+1<p->words;i+=2){
  unsigned w=0,v=0;for(unsigned b=0;b<4;b++){w=w<<8|p->bytes[i*4+b];v=v<<8|p->bytes[(i+1)*4+b];}
  if(w==0x10000000u+r)return v;
 }
 assert(0);return 0;
}
int main(void){
 grx_packet p;grx_config c={64,64,0x06600000,16,16,0x06610000,GRX_LOWER_LEFT,GRX_OPAQUE,0,7,1};
 assert(grx_begin(&p,&c)==GRX_OK);
 assert(reg(&p,0)==0x48&&reg(&p,3)==0x7a028&&reg(&p,12)==0x1c44);
 c.texture_options=8;assert(grx_begin(&p,&c)==GRX_BAD_ARGUMENT&&!p.open);
 c.texture_options=0;c.alpha_gt128=2;assert(grx_begin(&p,&c)==GRX_BAD_ARGUMENT&&!p.open);
 c.alpha_gt128=0;assert(grx_begin(&p,&c)==GRX_OK);
 assert(reg(&p,0)==0x40&&reg(&p,3)==0x78000&&reg(&p,12)==0x2044);
 puts("PASS GTA texture flags, alpha threshold and unchanged RGB565 defaults");
}
