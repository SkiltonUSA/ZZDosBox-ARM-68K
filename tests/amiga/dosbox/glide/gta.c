#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gta.h"
#include "guest_calls.h"
static unsigned char mem[2*1024*1024],gpu[2*1024*1024];
static unsigned sends,presents;
static int readg(void *p,uint32_t a,void *v,unsigned n){(void)p;if(a>sizeof(mem)||n>sizeof(mem)-a)return 0;memcpy(v,mem+a,n);return 1;}
static int writeg(void *p,uint32_t a,const void *v,unsigned n){(void)p;if(a>sizeof(mem)||n>sizeof(mem)-a)return 0;memcpy(mem+a,v,n);return 1;}
static int upload(void *p,uint32_t a,const void *v,unsigned n){(void)p;assert(a>=0x06600000&&a+n<=0x067f0000);memcpy(gpu+(a-0x06600000),v,n);return 1;}
static int readback(void *p,uint32_t a,void *v,unsigned n){(void)p;assert(a==0x06600000&&n==640*480*2);memcpy(v,gpu,n);return 1;}
#ifdef GRX_REAL_VALIDATOR
extern int grx_check_firmware(const grx_packet *);
#else
#define grx_check_firmware(p) 1
#endif
static int submit(void *p,const grx_packet *v){(void)p;assert(v->words&&v->words<=GRX_PACKET_WORDS);assert(grx_check_firmware(v));sends++;return 1;}
static int present(void *p,uint32_t a,unsigned w,unsigned h){(void)p;assert(a==0x06600000&&w==640&&h==480);presents++;return 1;}
int main(void){
 grx_gta *s=calloc(1,sizeof(*s));assert(s);
 grx_gta_backend b={0,upload,readback,submit,present};grx_guest_io io={readg,writeg,0,0};uint32_t reply;
 grx_gta_init(s,&b,0x10000);
 grx_guest_call c={GRX_CALL_grGlideInit,0,{0}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK);
 c=(grx_guest_call){GRX_CALL_grSstWinOpen,7,{0,7,0,0,0,2,0}};assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reply==1);
 c=(grx_guest_call){GRX_CALL_guTexAllocateMemory,15,{0,3,8,8,11,0,5,5,3,0,0,1,1,0,0}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reply==1);
 memset(mem+1024,255,128);c=(grx_guest_call){GRX_CALL_guTexDownloadMipMap,3,{1,1024,0}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&s->textures[0].downloaded);
 c=(grx_guest_call){GRX_CALL_grLfbLock,6,{1,1,1,0,1,128}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reply==1&&mem[128]==20);
 /* Only the changed pixel is written back; preserve untouched green LSBs. */
 gpu[4]=0x20;mem[0x10000]=0;mem[0x10001]=0x7c;
 c=(grx_guest_call){GRX_CALL_grLfbUnlock,2,{1,1}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&reply==1);
 assert(gpu[0]==0&&gpu[1]==0xf8&&gpu[4]==0x20);
 c=(grx_guest_call){GRX_CALL_grBufferSwap,1,{1}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_OK&&presents==1&&s->frames==1);
 c=(grx_guest_call){GRX_CALL_grDepthBufferMode,1,{1}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_UNSUPPORTED&&s->failed);
 c=(grx_guest_call){GRX_CALL_grBufferSwap,1,{1}};
 assert(grx_gta_call(s,&c,&io,&reply)==GRX_BAD_STATE&&presents==1);
 free(s);puts("PASS GTA allocation, LFB changed-pixel preservation, presentation and terminal unsupported state");
}
