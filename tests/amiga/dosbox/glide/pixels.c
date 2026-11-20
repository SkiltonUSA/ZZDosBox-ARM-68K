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
static unsigned word(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static unsigned to555(unsigned v){return ((v>>1)&0x7fe0)|(v&31);}
static void check_lfb(unsigned n,unsigned offset){
 unsigned char *raw=malloc(2*n+offset+1),*src=raw+offset,*dst=malloc(2*n+2);
 for(unsigned i=0;i<n;i++){unsigned v=i&65535;src[2*i]=v;src[2*i+1]=v>>8;}
 memset(dst,0xa5,2*n+2);grx_rgb565_555(src,dst+1,n);
 for(unsigned i=0;i<n;i++)assert(word(dst+1+2*i)==to555(word(src+2*i)));
 assert(dst[0]==0xa5&&dst[2*n+1]==0xa5);
 unsigned lo,hi;grx_lfb_dirty_span(dst+1,src,n,&lo,&hi);assert(lo==n&&hi==0);
 if(n){
  unsigned positions[]={0,n/2,n-1};
  for(unsigned j=0;j<3;j++){
   unsigned i=positions[j];dst[2*i+2]^=0x80;
   grx_lfb_dirty_span(dst+1,src,n,&lo,&hi);
   assert(lo<=i&&hi>i&&hi<=n&&hi-lo<=8);
   dst[2*i+2]^=0x80;
  }
  for(unsigned i=0;i<n;i++)dst[2*i+1]^=1;
  grx_lfb_dirty_span(dst+1,src,n,&lo,&hi);assert(lo==0&&hi==n);
 }
 free(dst);free(raw);
}
int main(void){for(unsigned o=0;o<16;o++){
 for(unsigned n=0;n<32;n++){check(n,o);check_lfb(n,o);}
 check(65536,o);check(640*480,o);check_lfb(65536,o);check_lfb(640*480,o);
}puts("NEON pixels: all colours, LFB conversion/dirty spans, tails, alignment and bounds PASS");}
