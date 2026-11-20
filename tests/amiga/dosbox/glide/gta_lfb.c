/* Exercise guest lock/edit/unlock against a byte-exact RGB565 reference. */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gta.h"
#include "guest_calls.h"
#define BYTES (GRX_GTA_PIXELS*2)
#define CHUNK 32768u
static unsigned char mem[2*1024*1024],gpu[BYTES],expected[BYTES];
static unsigned uploads,fail_upload,reads;
static unsigned get16(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static void put16(unsigned char *p,unsigned v){p[0]=v;p[1]=v>>8;}
static int readg(void *p,uint32_t a,void *v,unsigned n){(void)p;assert(a+n<=sizeof(mem));memcpy(v,mem+a,n);return 1;}
static int writeg(void *p,uint32_t a,const void *v,unsigned n){(void)p;assert(a+n<=sizeof(mem));memcpy(mem+a,v,n);return 1;}
static int upload(void *p,uint32_t a,const void *v,unsigned n){
 (void)p;assert(a>=0x06600000u&&a+n<=0x06600000u+BYTES);assert(n&&n<=CHUNK);
 uploads++;if(fail_upload&&uploads==fail_upload)return 0;
 memcpy(gpu+(a-0x06600000u),v,n);return 1;
}
static int readback(void *p,uint32_t a,void *v,unsigned n){(void)p;assert(a==0x06600000u&&n==BYTES);reads++;memcpy(v,gpu,n);return 1;}
static int submit(void *p,const grx_packet *v){(void)p;(void)v;return 1;}
static int present(void *p,const unsigned char *pixels,unsigned w,unsigned h){(void)p;assert(w==640&&h==480&&!memcmp(pixels,gpu,BYTES));return 1;}
int main(void){
 grx_gta *s=malloc(sizeof(*s));assert(s);grx_gta_backend b={0,upload,readback,submit,present};grx_guest_io io={readg,writeg,0,0};uint32_t reply;
 for(unsigned pattern=0;pattern<7;pattern++){
  grx_gta_init(s,&b,0x10000);uploads=reads=0;fail_upload=pattern==6?2:0;
  for(unsigned i=0;i<GRX_GTA_PIXELS;i++)put16(gpu+i*2,(i*31337u)^(i>>3));
  memcpy(expected,gpu,BYTES);
  grx_guest_call c={GRX_CALL_grGlideInit,0,{0}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK);
  c=(grx_guest_call){GRX_CALL_grSstWinOpen,7,{0,7,0,0,0,2,0}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK);
  c=(grx_guest_call){GRX_CALL_grLfbLock,6,{1,1,1,0,1,128}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK);
  for(unsigned i=0;i<GRX_GTA_PIXELS;i++){
   unsigned old=get16(expected+i*2),v=get16(mem+0x10000+i*2);
   assert(v==(((old>>1)&0x7fe0)|(old&31)));
   int change=pattern==1?(i==0||i==GRX_GTA_PIXELS-1):pattern==2?i%640==320:pattern==3?i%5==0:pattern>=4;
   if(change){v^=pattern==5?0x8000:0x5ad5;put16(mem+0x10000+i*2,v);unsigned g=(v>>5)&31;put16(expected+i*2,((v>>10)&31)*2048+(g*2+(g>>4))*32+(v&31));}
  }
  c=(grx_guest_call){GRX_CALL_grLfbUnlock,2,{1,1}};
  grx_result r=grx_gta_call(s,&c,&io,&reply);
  if(fail_upload){assert(r==GRX_BAD_STATE&&s->failed&&uploads==2);assert(grx_gta_call(s,&c,&io,&reply)==GRX_BAD_STATE);}
  else {assert(r==GRX_OK&&reply==1);assert(!memcmp(gpu,expected,BYTES));assert(reads==1);if(!pattern)assert(!uploads);
   /* A second lock and presentation reuse the exact CPU copy. */
   c=(grx_guest_call){GRX_CALL_grLfbLock,6,{1,1,1,0,1,128}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reads==1);
   c=(grx_guest_call){GRX_CALL_grLfbUnlock,2,{1,1}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK);
   c=(grx_guest_call){GRX_CALL_grBufferSwap,1,{1}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reads==1);
   /* GPU clear invalidates the copy, even if this mock leaves pixels unchanged. */
   c=(grx_guest_call){GRX_CALL_grBufferClear,3,{0,0,65535}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK);
   gpu[0]^=0x20;
   c=(grx_guest_call){GRX_CALL_grBufferSwap,1,{1}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reads==2);
  }
 }
 free(s);puts("PASS LFB exact unchanged RGB565, sparse/full edits, cached reads, clear invalidation and terminal failure");
}
