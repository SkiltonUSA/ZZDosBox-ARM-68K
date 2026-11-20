/* Replay private capture through the real guest translator; no rasterization.
 * Observed swaps are not GPU completions or FPS. Game bytes stay private. */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gta.h"
#include "guest_calls.h"
static unsigned char mem[32*1024*1024],gpu[0x1f0000];
static unsigned packets,presents,uploads,calls;
static int readg(void *p,uint32_t a,void *v,unsigned n){(void)p;if(a>sizeof(mem)||n>sizeof(mem)-a)return 0;memcpy(v,mem+a,n);return 1;}
static int writeg(void *p,uint32_t a,const void *v,unsigned n){(void)p;if(a>sizeof(mem)||n>sizeof(mem)-a)return 0;memcpy(mem+a,v,n);return 1;}
static int upload(void *p,uint32_t a,const void *v,unsigned n){(void)p;assert(a>=0x06600000&&a+n<=0x067f0000);memcpy(gpu+(a-0x06600000),v,n);uploads++;return 1;}
static int readback(void *p,uint32_t a,void *v,unsigned n){(void)p;assert(a==0x06600000&&n==640*480*2);memcpy(v,gpu,n);return 1;}
#ifdef GRX_REAL_VALIDATOR
extern int grx_check_firmware(const grx_packet *);
#else
#define grx_check_firmware(p) 1
#endif
static int submit(void *p,const grx_packet *v){(void)p;assert(v->words&&v->words<=GRX_PACKET_WORDS);assert(grx_check_firmware(v));packets++;return 1;}
static int present(void *p,uint32_t a,unsigned w,unsigned h){(void)p;assert(a==0x06600000&&w==640&&h==480);presents++;return 1;}
static unsigned word(FILE *f){unsigned char b[4];assert(fread(b,1,4,f)==4);return b[0]|(unsigned)b[1]<<8|(unsigned)b[2]<<16|(unsigned)b[3]<<24;}
int main(int argc,char **argv){
 assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);assert(word(f)==0x31415447);
 grx_gta *s=calloc(1,sizeof(*s));assert(s);grx_gta_backend b={0,upload,readback,submit,present};
 grx_guest_io io={readg,writeg,0,0};grx_gta_init(s,&b,0x100000);
 unsigned char *blob=malloc(GRX_GTA_PIXELS*2);assert(blob);
 while(1){int c0=fgetc(f);if(c0==EOF)break;ungetc(c0,f);
  unsigned tick=word(f);grx_guest_call c={0};c.id=word(f);c.count=word(f);unsigned bytes=word(f),expected=word(f);
  assert(c.count<=16&&bytes<=GRX_GTA_PIXELS*2&&c.id<GRX_CALL_COUNT);
  for(unsigned i=0;i<c.count;i++)c.args[i]=word(f);
  assert(fread(blob,1,bytes,f)==bytes);
  if(c.id==GRX_CALL_guDrawTriangleWithClip||c.id==GRX_CALL_grDrawTriangle){assert(bytes==180);for(unsigned i=0;i<3;i++)assert(writeg(0,c.args[i],blob+60*i,60));}
  else if(c.id==GRX_CALL_guTexDownloadMipMap)assert(writeg(0,c.args[1],blob,bytes));
  else if(c.id==GRX_CALL_grLfbUnlock)assert(writeg(0,s->lfb_address,blob,bytes));
  else assert(!bytes);
  unsigned reply;grx_result r=grx_gta_call(s,&c,&io,&reply);calls++;
  if(r!=GRX_OK){fprintf(stderr,"FAIL tick=%u call=%s status=%u index=%u frames=%u\n",tick,grx_calls[c.id].name,r,calls,presents);return 1;}
  if(reply!=expected){fprintf(stderr,"Reply differs call=%s got=%u wanted=%u\n",grx_calls[c.id].name,reply,expected);return 2;}
 }
 printf("PASS GTA private call replay: calls=%u swaps=%u translated_packets=%u uploads=%u textures=%u bytes=%u; no rendering/FPS\n",calls,presents,packets,uploads,s->texture_count,s->used);
 free(blob);free(s);fclose(f);
}
