#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "renderer.h"
int main(void){
 const unsigned modes[][2]={{320,200},{320,400},{640,480},{720,348},{800,600},{1024,768},{1,1},{321,201}};
 for(unsigned m=0;m<sizeof(modes)/sizeof(*modes);m++){
  unsigned w=modes[m][0],h=modes[m][1];uint32_t*p=malloc(w*h*4);assert(p);
  for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++)p[y*w+x]=((x&255)<<16)|((y&255)<<8)|((x^y)&255);
  assert(zz_gl_render(p,w,h));const uint32_t*q=zz_gl_pixels();assert(q);
  for(unsigned y=0;y<480;y++)for(unsigned x=0;x<640;x++){
   unsigned sx=(2*x+1)*w/1280,sy=(2*y+1)*h/960;
   if(q[y*640+x]!=p[sy*w+sx]){printf("mismatch %ux%u at %u,%u got %x wanted %x\n",w,h,x,y,q[y*640+x],p[sy*w+sx]);return 1;}
  }
  /* External RTG bitmap: both byte orders, tight/padded pitch, guarded ends.
   * Compare against the independent integer oracle, including row padding. */
  for(unsigned format=3;format<=4;format++)for(unsigned pitch=2560;pitch<=4096;pitch+=1536){
   size_t count=480*pitch/4;uint32_t *allocation=malloc((count+2)*4);assert(allocation);
   for(size_t i=0;i<count+2;i++)allocation[i]=0xdeadbeef;
   unsigned char stop[4]={0};
   assert(zz_gl_render_to(p,w,h,allocation+1,pitch,format,stop)==1);
   assert(zz_gl_pixels()==q); /* Never retain or free the borrowed target. */
   for(unsigned y=0;y<480;y++)for(unsigned x=0;x<pitch/4;x++){
    uint32_t expected=0xdeadbeef;
    if(x<640){expected=p[((2*y+1)*h/960)*w+(2*x+1)*w/1280];if(format==4)expected=__builtin_bswap32(expected);}
    assert(allocation[1+y*pitch/4+x]==expected);
   }
   assert(allocation[0]==0xdeadbeef&&allocation[count+1]==0xdeadbeef);
   assert(!zz_gl_render_to(p,w,h,allocation+1,2556,format,stop));
   assert(!zz_gl_render_to(p,w,h,allocation+1,pitch,2,stop));
   assert(!zz_gl_render_to(p,1025,h,allocation+1,pitch,format,stop));
   assert(zz_gl_pixels()==q);
   memset(allocation,0x5a,(count+2)*4);stop[3]=1; /* BE shared stop word */
   assert(zz_gl_render_to(p,w,h,allocation+1,pitch,format,stop)==-1);
   for(size_t i=0;i<count+2;i++)assert(allocation[i]==0x5a5a5a5a);
   assert(zz_gl_pixels()==q);free(allocation);
  }
  free(p);printf("PASS TinyGL %ux%u private/external pixels, byte orders, stride, guards, stop\n",w,h);
 }
 assert(!zz_gl_render(0,320,200));zz_gl_close();zz_gl_close();assert(!zz_gl_pixels());
 uint32_t p=0x123456;assert(zz_gl_render(&p,1,1));assert(zz_gl_pixels()[640*480-1]==p);zz_gl_close();puts("PASS TinyGL close/reopen and invalid geometry");return 0;
}
