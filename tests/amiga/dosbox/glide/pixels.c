#include "pixels_neon.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void check(unsigned n, unsigned offset) {
 unsigned char *raw=malloc(2*n+offset), *s=raw+offset;
 uint32_t *d=malloc((n+2)*sizeof(*d));
 for(unsigned i=0;i<n;i++){unsigned v=i&65535;s[2*i]=v;s[2*i+1]=v>>8;}
 d[0]=d[n+1]=0xdeadbeef;
 grx_rgb565_xrgb(s,d+1,n);
 for(unsigned i=0;i<n;i++){
  unsigned v=i&65535,expected=(((v>>11)*255/31)<<16)|((((v>>5)&63)*255/63)<<8)|((v&31)*255/31);
  assert(d[i+1]==expected);
 }
 assert(d[0]==0xdeadbeef && d[n+1]==0xdeadbeef);free(d);free(raw);
}
int main(void){for(unsigned o=0;o<16;o++){for(unsigned n=0;n<32;n++)check(n,o);check(65536,o);check(640*480,o);}puts("NEON RGB565: all 65536 colours, tails, alignment and frame bounds PASS");}
