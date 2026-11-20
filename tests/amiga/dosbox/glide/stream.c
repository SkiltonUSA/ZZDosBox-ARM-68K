#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stream.h"
static grx_stream_channel c;static grx_stream s;static unsigned polls,uploads,submits,pushed;
static void pull(void *p,void *v,unsigned n){(void)p;(void)v;assert(n&&n%64==0);}
static void push(void *p,void *v,unsigned n){pull(p,v,n);pushed++;}
static int upload(void *p,uint32_t a,const void *v,unsigned n){(void)p;assert(a==GRX_STREAM_BASE&&n==8&&((unsigned char*)v)[0]==42);uploads++;return 1;}
static int readback(void *p,uint32_t a,void *v,unsigned n){(void)p;assert(a==GRX_STREAM_BASE&&n==8);memset(v,99,n);return 1;}
static int submit(void *p,const void *v,unsigned n,uint32_t nonce,uint32_t seq){(void)p;(void)v;assert(n==12&&nonce==42&&seq==3);submits++;return 1;}
static int poll(void *p){(void)p;return ++polls>=3?1:0;}
int main(void){
 grx_stream_io io={0,pull,push,upload,readback,submit,poll};
 assert(grx_stream_open(&s,&c,&io,42));assert(!grx_stream_open(&s,&c,&io,43));
 c.data[0]=42;c.request=(grx_stream_request){.sequence=1,.nonce=42,.op=GRX_STREAM_UPLOAD,.address=GRX_STREAM_BASE,.bytes=8};
 grx_stream_poll(&s);assert(uploads==1&&c.result.sequence==1&&!c.result.error);
 grx_stream_poll(&s);assert(uploads==1);
 c.request=(grx_stream_request){.sequence=2,.nonce=42,.op=GRX_STREAM_READ,.address=GRX_STREAM_BASE,.bytes=8};
 grx_stream_poll(&s);assert(c.result.sequence==2&&c.data[0]==99);
 c.request=(grx_stream_request){.sequence=3,.nonce=42,.op=GRX_STREAM_SUBMIT,.bytes=12};
 grx_stream_poll(&s);assert(submits==1&&s.working&&c.result.sequence==2&&!grx_stream_close(&s,42));
 grx_stream_poll(&s);grx_stream_poll(&s);assert(c.result.sequence==2);
 grx_stream_poll(&s);assert(c.result.sequence==3&&!s.working&&grx_stream_close(&s,42));
 memset(&s,0,sizeof(s));assert(grx_stream_open(&s,&c,&io,43));
 c.request=(grx_stream_request){.sequence=1,.nonce=43,.op=GRX_STREAM_UPLOAD,.address=GRX_STREAM_END-4,.bytes=8};
 grx_stream_poll(&s);assert(s.failed&&c.result.error==GRX_STREAM_REQUEST&&uploads==1);
 assert(!grx_stream_close(&s,43));
 puts("PASS GPU stream: bounded spans, no duplicate request, retirement before reply, failed lease retained");
}
