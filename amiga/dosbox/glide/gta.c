/* Experimental bounded GTA ECTS Glide subset, driven by captured game calls.
 * Every submit retires before textures, framebuffer or state can be reused.
 * No claim of general Glide compatibility or pixel-exact shared-edge coverage. */
#include "gta.h"
#include "guest_calls.h"
#include "guest_vertex.h"
#include <string.h>
#define COLOR 0x06600000u
#define TEX_START (COLOR+GRX_GTA_PIXELS*2)
#define TEX_END 0x067f0000u
static unsigned get16(const unsigned char *p){return p[0]|(unsigned)p[1]<<8;}
static void put16(unsigned char *p,unsigned v){p[0]=v;p[1]=v>>8;}
static void put32(unsigned char *p,unsigned v){for(unsigned i=0;i<4;i++)p[i]=v>>(8*i);}
static int readg(const grx_guest_io *io,unsigned ptr,void *dst,unsigned n){
 uint64_t a=(uint64_t)io->data_base+ptr;
 return a<=UINT32_MAX&&n<=UINT32_MAX-a+1&&io->read(io->opaque,(uint32_t)a,dst,n);
}
static int writeg(const grx_guest_io *io,unsigned ptr,const void *src,unsigned n){
 uint64_t a=(uint64_t)io->data_base+ptr;
 return a<=UINT32_MAX&&n<=UINT32_MAX-a+1&&io->write(io->opaque,(uint32_t)a,src,n);
}
void grx_gta_init(grx_gta *s,const grx_gta_backend *b,uint32_t lfb){
 memset(s,0,sizeof(*s));s->backend=*b;s->lfb_address=lfb;s->resident_end=TEX_START;
}
static grx_result flush(grx_gta *s){
 grx_result r=grx_draw_flush(&s->stream);return r==GRX_EMPTY?GRX_OK:r;
}
static int power2(unsigned n){return n&&n<=256&&!(n&(n-1));}
static grx_result allocate(grx_gta *s,const uint32_t *a,uint32_t *reply){
 if(a[0]||a[1]!=3||!power2(a[2])||a[2]!=a[3]||a[4]!=11||a[5]||
    a[6]!=a[7]||a[7]>8||(256u>>a[7])!=a[2]||a[8]!=3||a[9]||a[10]||
    a[11]>1||a[12]>1||a[13]||a[14])return GRX_UNSUPPORTED;
 unsigned n=a[2]*a[3]*2;
 if(s->texture_count==1024||n>GRX_GTA_TEXTURE_BYTES-s->used){*reply=UINT32_MAX;return GRX_OK;}
 grx_gta_texture *t=&s->textures[s->texture_count++];
 *t=(grx_gta_texture){a[2],a[3],s->used,n,1|(a[11]?2:0)|(a[12]?4:0),0,0};
 s->used+=n;*reply=s->texture_count;return GRX_OK;
}
static grx_result download(grx_gta *s,const uint32_t *a,const grx_guest_io *io){
 if(!a[0]||a[0]>s->texture_count||a[2])return GRX_BAD_ARGUMENT;
 grx_gta_texture *t=&s->textures[a[0]-1];
 grx_result r=flush(s);if(r!=GRX_OK)return r;
 if(!readg(io,a[1],s->texture+t->offset,t->size))return GRX_BAD_ARGUMENT;
 /* Glide ARGB1555 -> RasterIX RGBA5551, retaining the alpha bit. */
 for(unsigned i=0;i<t->size;i+=2){unsigned v=get16(s->texture+t->offset+i);put16(s->texture+t->offset+i,(v<<1)|(v>>15));}
 if(t->resident&&!s->backend.upload(s->backend.context,t->resident,s->texture+t->offset,t->size))return GRX_BAD_STATE;
 t->downloaded=1;return GRX_OK;
}
static grx_result resident(grx_gta *s,grx_gta_texture *t){
 if(t->resident)return GRX_OK;
 grx_result r=flush(s);if(r!=GRX_OK)return r;
 unsigned n=(t->size+2047)&~2047u;
 if(n>TEX_END-s->resident_end){
  /* All prior references retired before recycling the bounded texture cache. */
  for(unsigned i=0;i<s->texture_count;i++)s->textures[i].resident=0;
  s->resident_end=TEX_START;
 }
 t->resident=s->resident_end;s->resident_end+=n;
 return s->backend.upload(s->backend.context,t->resident,s->texture+t->offset,t->size)?GRX_OK:GRX_BAD_STATE;
}
static grx_result draw(grx_gta *s,const grx_guest_call *call,const grx_guest_io *io){
 if(s->locked||!s->selected||s->colour!=4||s->alpha!=1||s->alpha_func!=4||s->alpha_ref!=128)return GRX_UNSUPPORTED;
 grx_gta_texture *t=&s->textures[s->selected-1];if(!t->downloaded)return GRX_BAD_STATE;
 grx_vertex v[3];if(!grx_guest_vertices(v,call->args,0,io))return GRX_BAD_ARGUMENT;
 grx_result r=resident(s,t);if(r!=GRX_OK)return r;
 grx_config c={640,480,COLOR,t->width,t->height,t->resident,GRX_UPPER_LEFT,GRX_OPAQUE,0,t->options,1};
 if(memcmp(&c,&s->stream.packet.config,sizeof(c))){r=grx_draw_configure(&s->stream,&c);if(r!=GRX_OK)return r;}
 /* GTA selects texture decal RGB and texture alpha; vertex colours are unused. */
 for(unsigned i=0;i<3;i++)v[i].r=v[i].g=v[i].b=v[i].a=255;
 r=grx_draw_vertices(&s->stream,v);return r==GRX_EMPTY?GRX_OK:r;
}
static grx_result lock(grx_gta *s,const uint32_t *a,const grx_guest_io *io,uint32_t *reply){
 if(s->locked||a[0]!=1||a[1]!=1||a[2]!=1||a[3]||a[4]!=1||!s->lfb_address||s->lfb_address<io->data_base)return GRX_UNSUPPORTED;
 grx_result r=flush(s);if(r!=GRX_OK)return r;
 if(!s->backend.readback(s->backend.context,COLOR,s->lfb,sizeof(s->lfb)))return GRX_BAD_STATE;
 for(unsigned i=0;i<sizeof(s->lfb);i+=2){unsigned v=get16(s->lfb+i);put16(s->before+i,((v>>1)&0x7fe0)|(v&31));}
 if(!io->write(io->opaque,s->lfb_address,s->before,sizeof(s->before)))return GRX_BAD_ARGUMENT;
 unsigned char info[20];put32(info,20);put32(info+4,s->lfb_address-io->data_base);put32(info+8,1280);put32(info+12,1);put32(info+16,0);
 if(!writeg(io,a[5],info,20))return GRX_BAD_ARGUMENT;
 s->locked=1;*reply=1;return GRX_OK;
}
static grx_result unlock(grx_gta *s,const uint32_t *a,const grx_guest_io *io,uint32_t *reply){
 if(!s->locked||a[0]!=1||a[1]!=1)return GRX_BAD_STATE;
 if(!io->read(io->opaque,s->lfb_address,s->lfb,sizeof(s->lfb)))return GRX_BAD_ARGUMENT;
 for(unsigned y=0;y<480;y++){
  unsigned lo=640,hi=0;
  for(unsigned x=0;x<640;x++){
   unsigned i=(y*640+x)*2,v=get16(s->lfb+i),old=get16(s->before+i);
   if(v!=old){if(lo==640)lo=x;hi=x+1;}
   /* Upload only changed runs, preserving untouched RGB565 green LSBs. */
  }
  for(unsigned x=lo;x<hi;){
   unsigned i=(y*640+x)*2;
   if(get16(s->lfb+i)==get16(s->before+i)){x++;continue;}
   unsigned start=x;
   do {
    i=(y*640+x)*2;unsigned v=get16(s->lfb+i),g=(v>>5)&31;
    put16(s->lfb+i,((v&0x7c00)<<1)|(g<<6)|((g>>4)<<5)|(v&31));x++;
   }while(x<hi&&get16(s->lfb+(y*640+x)*2)!=get16(s->before+(y*640+x)*2));
   if(!s->backend.upload(s->backend.context,COLOR+(y*640+start)*2,s->lfb+(y*640+start)*2,(x-start)*2))return GRX_BAD_STATE;
  }
 }
 s->locked=0;*reply=1;return GRX_OK;
}
static void packet_word(grx_packet *p,unsigned v){for(unsigned i=0;i<4;i++)p->bytes[p->words*4+i]=v>>(24-8*i);p->words++;}
static grx_result clear(grx_gta *s,const uint32_t *a){
 if(s->locked||a[0]||a[1]||a[2]!=65535)return GRX_UNSUPPORTED;
 grx_result r=flush(s);if(r!=GRX_OK)return r;
 grx_config c={640,480,COLOR,0,0,0,GRX_UPPER_LEFT,GRX_OPAQUE,0,0,0};grx_packet p;
 if(grx_begin(&p,&c)!=GRX_OK)return GRX_BAD_STATE;
 packet_word(&p,0x10000001);packet_word(&p,0);packet_word(&p,0x20000012u|(GRX_GTA_PIXELS<<8));
 if(grx_end(&p)!=GRX_OK||!s->backend.submit(s->backend.context,&p))return GRX_BAD_STATE;
 return GRX_OK;
}
static grx_result dispatch(grx_gta *s,const grx_guest_call *c,const grx_guest_io *io,uint32_t *reply){
 const uint32_t *a=c->args;
 if(c->id==GRX_CALL_grGlideInit){if(s->initialized)return GRX_BAD_STATE;s->initialized=1;return GRX_OK;}
 if(!s->initialized)return GRX_BAD_STATE;
 switch(c->id){
 case GRX_CALL_grGlideGetVersion:*reply=grx_guest_diagnostic(c,io);return GRX_OK;
 case GRX_CALL_grSstQueryHardware:case GRX_CALL_grSstQueryBoards:{
  unsigned char hw[148]={0};hw[0]=1;hw[8]=4;hw[12]=1;hw[16]=1;hw[24]=1;hw[28]=2;
  *reply=writeg(io,a[0],hw,sizeof(hw));return *reply?GRX_OK:GRX_BAD_ARGUMENT;
 }
 case GRX_CALL_grSstSelect:return a[0]?GRX_UNSUPPORTED:GRX_OK;
 case GRX_CALL_grSstWinOpen:{
  if(s->opened||a[1]!=7||a[3]||a[4]||a[5]!=2||a[6])return GRX_UNSUPPORTED;
  grx_config cfg={640,480,COLOR,0,0,0,GRX_UPPER_LEFT,GRX_OPAQUE,0,0,0};
  grx_result r=grx_draw_open(&s->stream,&cfg,s->backend.submit,s->backend.context);
  if(r==GRX_OK)s->opened=*reply=1;return r;
 }
 default:break;
 }
 if(!s->opened)return GRX_BAD_STATE;
 switch(c->id){
 case GRX_CALL_grGlideShutdown:case GRX_CALL_grSstWinClose:{grx_result r=flush(s);if(r==GRX_OK)s->opened=0;return r;}
 case GRX_CALL_guColorCombineFunction:if(a[0]!=2&&a[0]!=4)return GRX_UNSUPPORTED;s->colour=a[0];return GRX_OK;
 case GRX_CALL_grAlphaCombine:if(a[0]!=3||a[1]!=8||a[2]!=1||a[3]!=1||a[4])return GRX_UNSUPPORTED;s->alpha=1;return GRX_OK;
 case GRX_CALL_grAlphaTestReferenceValue:if(a[0]!=128)return GRX_UNSUPPORTED;s->alpha_ref=a[0];return GRX_OK;
 case GRX_CALL_grAlphaTestFunction:if(a[0]!=4)return GRX_UNSUPPORTED;s->alpha_func=a[0];return GRX_OK;
 case GRX_CALL_grTexMipMapMode:return a[0]||a[1]||a[2]?GRX_UNSUPPORTED:GRX_OK;
 case GRX_CALL_grTexCombineFunction:return a[0]||a[1]!=1?GRX_UNSUPPORTED:GRX_OK;
 case GRX_CALL_guTexMemQueryAvail:*reply=GRX_GTA_TEXTURE_BYTES-s->used;return GRX_OK;
 case GRX_CALL_guTexMemReset:{grx_result r=flush(s);if(r!=GRX_OK)return r;s->texture_count=s->used=s->selected=0;s->resident_end=TEX_START;return GRX_OK;}
 case GRX_CALL_guTexAllocateMemory:return allocate(s,a,reply);
 case GRX_CALL_guTexDownloadMipMap:return download(s,a,io);
 case GRX_CALL_guTexSource:if(!a[0]||a[0]>s->texture_count)return GRX_BAD_ARGUMENT;s->selected=a[0];return GRX_OK;
 case GRX_CALL_guDrawTriangleWithClip:case GRX_CALL_grDrawTriangle:return draw(s,c,io);
 case GRX_CALL_grLfbLock:return lock(s,a,io,reply);
 case GRX_CALL_grLfbUnlock:return unlock(s,a,io,reply);
 case GRX_CALL_grBufferClear:return clear(s,a);
 case GRX_CALL_grBufferSwap:{
  if(s->locked||a[0]>1)return GRX_UNSUPPORTED;
  grx_result r=flush(s);if(r!=GRX_OK)return r;
  if(!s->backend.present(s->backend.context,COLOR,640,480))return GRX_BAD_STATE;
  s->frames++;return GRX_OK;
 }
 default:return GRX_UNSUPPORTED;
 }
}
grx_result grx_gta_call(grx_gta *s,const grx_guest_call *c,const grx_guest_io *io,uint32_t *reply){
 *reply=0;if(s->failed)return GRX_BAD_STATE;
 if(!c->id||c->id>=GRX_CALL_COUNT||c->count>16||c->count*4!=grx_calls[c->id].bytes){s->failed=1;return GRX_BAD_ARGUMENT;}
 grx_result r=dispatch(s,c,io,reply);if(r!=GRX_OK)s->failed=1;return r;
}
