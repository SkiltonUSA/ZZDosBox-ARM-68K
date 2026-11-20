#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "draw_stream.h"
#include "guest_vertex.h"
#include "guest_calls.h"
#include "clip.h"
static unsigned char memory[512];
static grx_packet actual[512],expected[512];
static unsigned captured,wanted,calls,fail_at;
uint32_t draw_digest=2166136261u;
#ifdef GRX_CHECK_FIRMWARE
extern int grx_check_firmware(const grx_packet *);
#endif
static int read_guest(void *unused,uint32_t a,void *out,unsigned n)
{(void)unused;if(a>sizeof(memory)||n>sizeof(memory)-a)return 0;memcpy(out,memory+a,n);return 1;}
static int submit(void *unused,const grx_packet *p)
{
 (void)unused;++calls;
 assert(!p->open&&p->words<=GRX_PACKET_WORDS);
 if(calls==fail_at)return 0;
 #ifdef GRX_CHECK_FIRMWARE
 assert(grx_check_firmware(p));
#endif
 for(unsigned i=0;i<p->words*4;i++)draw_digest=(draw_digest^p->bytes[i])*16777619u;
 assert(captured<512);actual[captured++]=*p;return 1;
}
static void put_float(unsigned a,float f)
{uint32_t w;memcpy(&w,&f,4);for(unsigned i=0;i<4;i++)memory[a+i]=(unsigned char)(w>>(i*8));}
static const uint32_t pointers[3]={31,97,165};
static grx_guest_io io={read_guest,0,0,19};
static grx_guest_call draw;
static void vertices(const grx_vertex v[3])
{
 for(unsigned i=0;i<3;i++){
  unsigned a=io.data_base+pointers[i];
  put_float(a,v[i].x);put_float(a+4,v[i].y);
  put_float(a+12,v[i].r);put_float(a+16,v[i].g);put_float(a+20,v[i].b);
  put_float(a+28,v[i].a);put_float(a+32,v[i].oow);
  put_float(a+36,v[i].sow);put_float(a+40,v[i].tow);put_float(a+44,v[i].oow*2);
 }
}
static grx_config config(unsigned texture,unsigned origin,unsigned blend)
{return (grx_config){640,480,0x06600000,texture?256:0,texture?256:0,texture?0x066a0000:0,origin,blend,0};}
static void reset(void){captured=wanted=calls=fail_at=0;}
static void compare(void)
{
 assert(captured==wanted);
 for(unsigned i=0;i<wanted;i++){
  assert(actual[i].words==expected[i].words);
  assert(!memcmp(actual[i].bytes,expected[i].bytes,actual[i].words*4));
 }
}
static void expected_flush(grx_packet *p)
{assert(grx_end(p)==GRX_OK);expected[wanted++]=*p;grx_config c=p->config;assert(grx_begin(p,&c)==GRX_OK);}
static void fixture(void)
{
 for(unsigned t=0;t<2;t++)for(unsigned origin=0;origin<2;origin++)for(unsigned blend=0;blend<3;blend++){
  reset();grx_config c=config(t,origin,blend);grx_draw_stream stream;grx_packet p;
  assert(grx_draw_open(&stream,&c,submit,0)==GRX_OK);assert(grx_begin(&p,&c)==GRX_OK);
  unsigned pending=0;
  for(unsigned i=0;i<320;i++){
   float x=(float)(i%20)*30,y=(float)(i/20)*28;
   grx_vertex v[3]={{x,y,255,30,50,255,0,0,1},{x+23,y,20,255,70,128,128,0,.5f},{x,y+21,70,30,255,255,0,64,.25f}};
   vertices(v);assert(grx_draw_call(&stream,&draw,&io,0)==GRX_OK);
   grx_result rc=grx_triangle(&p,v);
   if(rc==GRX_FULL){expected_flush(&p);pending=0;rc=grx_triangle(&p,v);}
   assert(rc==GRX_OK);pending++;
  }
  if(pending)expected_flush(&p);
  assert(grx_draw_flush(&stream)==GRX_OK);compare();
  unsigned before=calls;assert(grx_draw_flush(&stream)==GRX_EMPTY);assert(calls==before);
 }
 puts("PASS guest draw: 320 triangles, texture/origin/blend variants, ordered packet rollover");
}
static void clipped_and_state(void)
{
 reset();grx_config c=config(1,GRX_UPPER_LEFT,GRX_ALPHA);grx_draw_stream s;
 assert(grx_draw_open(&s,&c,submit,0)==GRX_OK);
 grx_vertex v[3]={{-200,80,255,0,0,255,0,0,1},{300,-200,0,255,0,128,128,0,.5f},{700,600,0,0,255,255,0,64,.25f}};
 vertices(v);assert(grx_draw_call(&s,&draw,&io,1)==GRX_OK);
 /* Independent expected fan assembly, including TMU-specific reciprocal W. */
 for(unsigned i=0;i<3;i++)v[i].oow*=2;
 grx_vertex polygon[GRX_CLIP_VERTICES];unsigned n;
 assert(grx_clip_triangle(v,c.width,c.height,polygon,&n)==GRX_OK&&n>3);
 grx_packet p;assert(grx_begin(&p,&c)==GRX_OK);
 for(unsigned i=1;i+1<n;i++){
  grx_vertex tri[3]={polygon[0],polygon[i],polygon[i+1]};grx_result rc=grx_triangle(&p,tri);
  if(rc==GRX_FULL){expected_flush(&p);rc=grx_triangle(&p,tri);}
  assert(rc==GRX_OK);
 }
 expected_flush(&p);
 grx_config next=config(0,GRX_LOWER_LEFT,GRX_ADDITIVE);
 assert(grx_draw_configure(&s,&next)==GRX_OK);compare();assert(s.packet.config.blend==GRX_ADDITIVE);
 assert(grx_draw_flush(&s)==GRX_EMPTY);
 puts("PASS clipped fan crosses packets, TMU W and state boundary preserve ordering");
}
static void rollover_failure(void)
{
 reset();grx_config c=config(1,GRX_UPPER_LEFT,GRX_OPAQUE);grx_draw_stream s;
 assert(grx_draw_open(&s,&c,submit,0)==GRX_OK);
 grx_vertex v[3]={{2,2,255,0,0,255,0,0,1},{40,2,0,255,0,255,0,0,1},{2,40,0,0,255,255,0,0,1}};
 vertices(v);fail_at=2;unsigned i=0;
 while(grx_draw_call(&s,&draw,&io,0)==GRX_OK){assert(++i<20);}
 assert(calls==2&&captured==1&&s.failed);
 assert(grx_draw_flush(&s)==GRX_BAD_STATE&&calls==2);
 assert(grx_draw_call(&s,&draw,&io,0)==GRX_BAD_STATE&&calls==2);
 puts("PASS mid-stream retirement failure stops after accepted prefix without replay");
}
static void boundaries(void)
{
 reset();grx_config c=config(0,GRX_UPPER_LEFT,GRX_OPAQUE);grx_draw_stream s;
 assert(grx_draw_open(&s,&c,submit,0)==GRX_OK);
 grx_vertex v[3]={{2,2,255,0,0,255,0,0,1},{40,2,0,255,0,255,0,0,1},{2,40,0,0,255,255,0,0,1}};
 vertices(v);assert(grx_draw_call(&s,&draw,&io,0)==GRX_OK);grx_draw_stream before=s;
 grx_guest_call bad=draw;bad.args[2]=500;assert(grx_draw_call(&s,&bad,&io,0)==GRX_BAD_ARGUMENT);
 bad=draw;bad.count=2;assert(grx_draw_call(&s,&bad,&io,0)==GRX_BAD_ARGUMENT);
 bad=draw;bad.id=GRX_CALL_grSstWinOpen;assert(grx_draw_call(&s,&bad,&io,0)==GRX_UNSUPPORTED);
 grx_guest_io overflow=io;overflow.data_base=0xfffffff0u;assert(grx_draw_call(&s,&draw,&overflow,0)==GRX_BAD_ARGUMENT);
 v[0].x=NAN;vertices(v);assert(grx_draw_call(&s,&draw,&io,0)==GRX_BAD_ARGUMENT);
 for(unsigned i=0;i<3;i++)v[i].x=-10-(float)i;
 vertices(v);assert(grx_draw_call(&s,&draw,&io,0)==GRX_EMPTY);
 grx_config bad_config=c;bad_config.depth_test=1;
 assert(grx_draw_configure(&s,&bad_config)==GRX_UNSUPPORTED);
 assert(!memcmp(&s,&before,sizeof(s))&&calls==0);
 assert(grx_draw_flush(&s)==GRX_OK&&calls==1);
 /* A failed retirement must not permit retry, state change, or more writes. */
 v[0].x=2;v[1].x=40;v[2].x=2;vertices(v);
 assert(grx_draw_call(&s,&draw,&io,0)==GRX_OK);fail_at=2;
 assert(grx_draw_flush(&s)==GRX_BAD_STATE);
 assert(grx_draw_flush(&s)==GRX_BAD_STATE&&calls==2);
 assert(grx_draw_call(&s,&draw,&io,0)==GRX_BAD_STATE&&calls==2);
 assert(grx_draw_configure(&s,&c)==GRX_BAD_STATE&&calls==2);
 puts("PASS bad guest pointers/state rejected, empty draws suppressed, failed transport stops stream");
}
int main(void)
{
 draw.id=GRX_CALL_grDrawTriangle;draw.count=3;memcpy(draw.args,pointers,sizeof(pointers));
 fixture();clipped_and_state();boundaries();rollover_failure();
 printf("DRAW_DIGEST=%08x\n",draw_digest);return 0;
}
