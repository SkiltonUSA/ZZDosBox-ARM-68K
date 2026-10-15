/* Exercise the actual host service with noncoherent host/ARM memory views. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "host.h"
static uint8_t host[DB_PROGRAM],arm[DB_PROGRAM];
static unsigned sent,received,available;static int open_ok=1,poll_ok=1,input_ok=1;
int zz_net_open(const char*dns){(void)dns;return open_ok;}
void zz_net_close(void){}
int zz_net_input(const uint8_t*p,unsigned n){assert(n==64);for(unsigned j=0;j<n;j++)assert(p[j]==(uint8_t)(sent+j));sent++;return input_ok;}
int zz_net_poll(void){return poll_ok;}
unsigned zz_net_receive(uint8_t*p,unsigned n){assert(n>=64);if(!available)return 0;available--;for(unsigned j=0;j<64;j++)p[j]=(uint8_t)(received+j);received++;return 64;}
struct zz_net_stats zz_net_statistics(void){struct zz_net_stats s={0};return s;}
static void pull(volatile void*p,size_t n,void*unused){(void)unused;size_t o=(volatile uint8_t*)p-host;assert(o>=NET_CONTROL&&o+n<=NET_RX_DATA+NET_COUNT*NET_STRIDE);memcpy(host+o,arm+o,n);}
static void push(volatile void*p,size_t n,void*unused){(void)unused;size_t o=(volatile uint8_t*)p-host;assert(o>=NET_CONTROL&&o+n<=NET_RX_DATA+NET_COUNT*NET_STRIDE);memcpy(arm+o,host+o,n);}
static void barrier(void*unused){(void)unused;}
static const struct ad_io io={pull,push,barrier,NULL,NULL};
static void begin(unsigned seq){
 memset(host,0xa5,sizeof(host));memset(arm,0x5a,sizeof(arm));sent=received=available=0;open_ok=poll_ok=input_ok=1;
 assert(db_net_host_open(host,&io,1,NULL));assert(ad_get(arm,NET_ENABLED)==NET_MAGIC);
 unsigned indices[]={NET_TX_WRITE,NET_TX_READ,NET_RX_WRITE,NET_RX_READ};
 for(unsigned i=0;i<4;i++){ad_put(host,indices[i],seq);ad_put(arm,indices[i],seq);}
}
static void enqueue(unsigned n){unsigned w=ad_get(arm,NET_TX_WRITE);for(unsigned i=0;i<n;i++){unsigned o=net_slot(NET_TX_DATA,w+i);ad_put(arm,o,64);for(unsigned j=0;j<64;j++)arm[o+4+j]=(uint8_t)(sent+i+j);}ad_put(arm,NET_TX_WRITE,w+n);}
void zz_net_configuration(struct dn_config*c){dn_defaults(c);}
int main(void){
 assert(net_pending(0,UINT32_MAX)==1);assert(net_pending(0,1)==-1);
 begin(UINT32_MAX-1);enqueue(4);available=9;
 assert(db_net_host_service());assert(sent==4&&received==4&&available==5);
 assert(ad_get(arm,NET_TX_READ)==2&&ad_get(arm,NET_RX_WRITE)==2);
 for(unsigned i=0;i<4;i++){unsigned o=net_slot(NET_RX_DATA,UINT32_MAX-1+i);assert(ad_get(arm,o)==64);for(unsigned j=0;j<64;j++)assert(arm[o+4+j]==(uint8_t)(i+j));}
 /* Full RX queue must never overwrite unread frames or consume the backend. */
 assert(db_net_host_service());assert(received==4&&available==5);
 ad_put(arm,NET_RX_READ,2);assert(db_net_host_service());assert(received==8&&available==1);
 db_net_host_close();assert(db_net_host_service());
 begin(0);ad_put(arm,NET_TX_WRITE,5);assert(!db_net_host_service()&&sent==0);db_net_host_close();
 begin(0);enqueue(1);ad_put(arm,NET_TX_DATA,NET_MTU+1);assert(!db_net_host_service()&&sent==0);db_net_host_close();
 begin(0);ad_put(arm,NET_RX_READ,1);assert(!db_net_host_service());db_net_host_close();
 begin(0);enqueue(1);input_ok=0;assert(!db_net_host_service());assert(ad_get(arm,NET_TX_READ)==0);db_net_host_close();
 begin(0);poll_ok=0;assert(!db_net_host_service());db_net_host_close();
 open_ok=0;assert(!db_net_host_open(host,&io,1,NULL));assert(ad_get(arm,NET_ENABLED)==0);
 assert(db_net_host_open(host,&io,0,NULL));assert(db_net_host_service());db_net_host_close();
 puts("PASS network queues: separate cache views, packet bytes, wrap, backpressure, corrupt indices/length, backend failures, disabled state");
}
