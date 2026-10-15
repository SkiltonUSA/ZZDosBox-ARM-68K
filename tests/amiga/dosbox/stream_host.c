/* Separate CPU views catch missing sync and publication before payload. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stream_host.h"
static uint8_t host[DB_TX_DATA+DB_TX_SIZE],arm[sizeof(host)];
static unsigned pulls,pushes,payload_pull,payload_push,barriers;
static uint32_t expected_write;
static uint8_t expected[DB_RX_SIZE];
static unsigned expected_n;
static void pull(volatile void *p,size_t n,void *user){
 size_t o=(volatile uint8_t*)p-host;assert(user==arm&&o+n<=sizeof(host));
 assert(n);pulls++;if(o>=DB_TX_DATA)payload_pull+=n;
 memcpy(host+o,arm+o,n);
}
static void push(volatile void *p,size_t n,void *user){
 size_t o=(volatile uint8_t*)p-host;assert(user==arm&&o+n<=sizeof(host));
 assert(n);pushes++;if(o>=DB_RX_DATA&&o<DB_RX_DATA+DB_RX_SIZE)payload_push+=n;
 if(o==DB_RX_WRITE){
  for(unsigned i=0;i<expected_n;i++)assert(arm[DB_RX_DATA+((expected_write+i)&255)]==expected[i]);
 }
 memcpy(arm+o,host+o,n);
}
static void barrier(void *user){assert(user==arm);barriers++;}
static const struct ad_io io={pull,push,barrier,NULL,arm};
static void begin(void){
 memset(host,0xa5,sizeof(host));memset(arm,0x5a,sizeof(arm));
 pulls=pushes=payload_pull=payload_push=barriers=0;
}
int main(void){
 static uint8_t out[DB_TX_SIZE];
 const uint32_t starts[]={0,1,127,255,4095,UINT32_MAX-3};
 for(unsigned s=0;s<sizeof(starts)/sizeof(*starts);s++){
  uint32_t start=starts[s];
  for(unsigned n=0;n<=DB_TX_SIZE;n++){
   begin();ad_put(host,DB_TX_READ,start);ad_put(arm,DB_TX_WRITE,start+n);
   for(unsigned i=0;i<n;i++)arm[DB_TX_DATA+((start+i)&4095)]=(uint8_t)i;
   assert(db_stream_peek(host,&io,out)==(int)n);
   for(unsigned i=0;i<n;i++)assert(out[i]==(uint8_t)i);
   assert(payload_pull==n&&pushes==0&&ad_get(host,DB_TX_READ)==start);
   /* Re-peek before consume: output failure cannot lose queued bytes. */
   assert(db_stream_peek(host,&io,out)==(int)n);
   db_stream_consume(host,&io,n);
   assert(pushes==(n?1u:0u)&&barriers==(n?1u:0u));
   assert(ad_get(host,DB_TX_READ)==start+n);
  }
  for(unsigned n=0;n<=DB_RX_SIZE;n++){
   begin();ad_put(host,DB_RX_WRITE,start);ad_put(arm,DB_RX_READ,start);
   expected_write=start;expected_n=n;
   for(unsigned i=0;i<n;i++)expected[i]=(uint8_t)(i+17);
   assert(db_stream_input(host,&io,expected,n));
   assert(payload_push==n&&ad_get(host,DB_RX_WRITE)==start+n);
   assert(barriers==(n?1u:0u));
   if(n)assert(ad_get(arm,DB_RX_WRITE)==start+n);
  }
 }
 begin();ad_put(host,DB_TX_READ,0);ad_put(arm,DB_TX_WRITE,DB_TX_SIZE+1);
 assert(db_stream_peek(host,&io,out)==-1&&payload_pull==0&&pushes==0);
 begin();ad_put(host,DB_RX_WRITE,DB_RX_SIZE);ad_put(arm,DB_RX_READ,0);
 assert(!db_stream_input(host,&io,expected,1)&&pushes==0);
 ad_put(host,DB_RX_WRITE,DB_RX_SIZE+1);
 assert(!db_stream_input(host,&io,expected,0)&&pushes==0);
 ad_put(host,DB_RX_WRITE,0);
 assert(!db_stream_input(host,&io,expected,DB_RX_SIZE+1)&&pushes==0);
 puts("PASS console rings: empty/full, exact sync spans, wrap, publication ordering, corrupt indices, output retry");
}
