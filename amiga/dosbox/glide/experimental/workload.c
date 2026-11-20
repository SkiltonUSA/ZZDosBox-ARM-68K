#include <string.h>
#include "workload.h"
#include "draw_stream.h"
#include "guest_calls.h"
unsigned grx_workload_width(unsigned p){return p==1?320:p==2?640:0;}
unsigned grx_workload_height(unsigned p){return p==1?240:p==2?480:0;}
static void word(grx_packet *p,unsigned i,uint32_t w)
{for(unsigned b=0;b<4;b++)p->bytes[i*4+b]=(uint8_t)(w>>(24-b*8));}
static int read_guest(void *p,uint32_t a,void *dst,unsigned n)
{if(a>180||n>180-a)return 0;memcpy(dst,(unsigned char*)p+a,n);return 1;}
static void put_float(unsigned char *p,float f)
{uint32_t w;memcpy(&w,&f,4);for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(w>>(i*8));}
static void vertex(unsigned char *p,float x,float y,float s,float t)
{
 put_float(p,x);put_float(p+4,y);
 put_float(p+12,255);put_float(p+16,255);put_float(p+20,255);put_float(p+28,255);
 put_float(p+32,1);put_float(p+36,s);put_float(p+40,t);
}
struct capture {grx_packet *packet;unsigned count;};
static int capture(void *p,const grx_packet *packet)
{struct capture *c=p;if(c->count++)return 0;*c->packet=*packet;return 1;}
int grx_workload_packet(grx_packet *packet,unsigned profile,unsigned batch)
{
 unsigned w=grx_workload_width(profile),h=grx_workload_height(profile);
 if(!w||batch>=GRX_WORKLOAD_PACKETS)return 0;
 grx_config config={w,h,GRX_WORKLOAD_COLOR,16,16,GRX_WORKLOAD_TEXTURE,GRX_LOWER_LEFT,GRX_OPAQUE,0};
 if(!batch){
  config.texture_width=config.texture_height=config.texture_address=0;
  if(grx_begin(packet,&config)!=GRX_OK)return 0;
  word(packet,packet->words++,0x10000001);word(packet,packet->words++,0);
  word(packet,packet->words++,0x20000012u|((w*h)<<8));
  return grx_end(packet)==GRX_OK;
 }
 grx_draw_stream stream;struct capture out={packet,0};
 if(grx_draw_open(&stream,&config,capture,&out)!=GRX_OK)return 0;
 unsigned char guest[180]={0};grx_guest_io io={read_guest,0,guest,0};
 grx_guest_call call={GRX_CALL_grDrawTriangle,3,{0,60,120}};
 for(unsigned n=0;n<5;n++){
  unsigned i=(batch-1)*5+n;float x=(float)((i%20)*(w/20)+1),y=(float)((i/20)*(h/16)+1);
  vertex(guest,x,y,0,0);vertex(guest+60,x+w/20-3,y,256,0);
  vertex(guest+120,x,y+h/16-3,0,256);
  if(grx_draw_call(&stream,&call,&io,0)!=GRX_OK)return 0;
 }
 return grx_draw_flush(&stream)==GRX_OK&&out.count==1;
}
static unsigned sample(const volatile uint16_t *pixels,unsigned w,unsigned h,unsigned x,unsigned y)
{
 uint16_t value=pixels[(h-1-y)*w+x];
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
 value=__builtin_bswap16(value);
#endif
 return value;
}
unsigned grx_workload_verify(const volatile uint16_t *pixels,unsigned profile,unsigned *checked)
{
 unsigned w=grx_workload_width(profile),h=grx_workload_height(profile),bad=0;*checked=0;
 if(!w)return 1;
 for(unsigned row=0;row<16;row++)for(unsigned col=0;col<20;col++){
  unsigned x=col*(w/20),y=row*(h/16);
  bad+=sample(pixels,w,h,x+2,y+2)!=0xf800;
  bad+=sample(pixels,w,h,x+1+3*(w/20-3)/4,y+2)!=0x001f;
  bad+=sample(pixels,w,h,x+w/20-1,y+h/16-1)!=0;
  *checked+=3;
 }
 return bad;
}
