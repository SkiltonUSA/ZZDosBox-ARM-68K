#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../../amiga/dosbox/pc/serial_host.h"
#include "../../../amiga/dosbox/pc/serial_wire.h"
static uint8_t memory[0x1c0000];
static void sync_range(volatile void*p,size_t n,void*o){(void)p;(void)n;(void)o;}
static void fence(void*o){(void)o;}
static const struct ad_io io={sync_range,sync_range,fence,fence,NULL};
static unsigned now=1;
static void poll(void){assert(db_serial_host_poll(now));now+=1000;}
static void send_guest(const char*s){unsigned w=ad_get(memory,PC_SERIAL_TX_WRITE);for(;*s;s++)memory[PC_SERIAL_TX_DATA+(w++&(PC_SERIAL_SIZE-1))]=*s;ad_put(memory,PC_SERIAL_TX_WRITE,w);}
static unsigned read_guest(char*s){unsigned r=ad_get(memory,PC_SERIAL_RX_READ),w=ad_get(memory,PC_SERIAL_RX_WRITE),n=0;while(r!=w)s[n++]=memory[PC_SERIAL_RX_DATA+(r++&(PC_SERIAL_SIZE-1))];s[n]=0;ad_put(memory,PC_SERIAL_RX_READ,r);return n;}
int main(void){
 struct sockaddr_in address={0};socklen_t len=sizeof(address);char received[8192];
 int server=socket(AF_INET,SOCK_STREAM,0);assert(server>=0);address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
 assert(!bind(server,(struct sockaddr*)&address,sizeof(address)));assert(!getsockname(server,(struct sockaddr*)&address,&len));assert(!listen(server,1));
 for(unsigned mode=1;mode<=2;mode++){
  assert(db_serial_host_open(memory,&io,mode,"127.0.0.1",ntohs(address.sin_port),NULL));
  /* Cross both wire-index and physical ring wrap with actual TCP bytes. */
  ad_put(memory,PC_SERIAL_TX_WRITE,0xfffffff8);ad_put(memory,PC_SERIAL_TX_READ,0xfffffff8);
  ad_put(memory,PC_SERIAL_RX_WRITE,0xfffffff8);ad_put(memory,PC_SERIAL_RX_READ,0xfffffff8);
  if(mode==2){send_guest("ATE0\r");poll();assert(read_guest(received)&&strstr(received,"OK"));send_guest("ATD\r");poll();}
  int client=accept(server,NULL,NULL);assert(client>=0);poll();poll();
  assert(ad_get(memory,PC_SERIAL_STATUS)&1);
  if(mode==2){read_guest(received);assert(strstr(received,"CONNECT"));}
  send_guest("SERIAL TCP ROUND TRIP\r\n");poll();
  int n=recv(client,received,sizeof(received),0);assert(n==23&&!memcmp(received,"SERIAL TCP ROUND TRIP\r\n",23));
  assert(send(client,received,n,0)==n);{unsigned used=0;for(unsigned retry=0;retry<100&&used<23;retry++){poll();used+=read_guest(received+used);usleep(1000);}assert(used==23&&!memcmp(received,"SERIAL TCP ROUND TRIP\r\n",23));}
  if(mode==2){ad_put(memory,PC_SERIAL_DTR,1);poll();ad_put(memory,PC_SERIAL_DTR,0);poll();assert(!(ad_get(memory,PC_SERIAL_STATUS)&1));read_guest(received);assert(strstr(received,"NO CARRIER"));}
  close(client);db_serial_host_close();
 }
 assert(db_serial_host_open(memory,&io,2,"127.0.0.1",23,NULL));ad_put(memory,PC_SERIAL_TX_WRITE,PC_SERIAL_SIZE+1);assert(!db_serial_host_poll(now));db_serial_host_close();close(server);
 puts("PASS COM1 TCP/modem: AT replies, real socket round trip, index/ring wrap, DTR hangup, invalid queue rejection");
}
