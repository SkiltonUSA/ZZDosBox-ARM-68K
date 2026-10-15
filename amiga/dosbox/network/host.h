/* GPL-3.0-or-later. Cooperative bounded service, never wait for the network. */
#pragma once
#include "net_wire.h"
#include "backend.h"
static volatile uint8_t *net_shared;
static const struct ad_io *net_io;
static int net_active;
static uint8_t net_packet[NET_MTU];
static int db_net_host_open(volatile uint8_t*b,const struct ad_io *io,int enabled,const char*dns){
 net_shared=b;net_io=io;net_active=0;
 for(unsigned o=NET_CONTROL;o<NET_TX_DATA;o+=4)ad_put(b,o,0);
 io->push(b+NET_CONTROL,NET_TX_DATA-NET_CONTROL,NULL);io->barrier(NULL);
 if(!enabled)return 1;
 if(!zz_net_open(dns))return 0;
 net_active=1;
 {struct dn_config c;uint32_t address=0;zz_net_configuration(&c);dn_ipv4(c.ipx_server,&address);ad_put(b,NET_PROTOCOL,c.protocol);ad_put(b,NET_SERVER,address);ad_put(b,NET_PORT,c.ipx_port);io->push(b+NET_PROTOCOL,12,NULL);io->barrier(NULL);}
 ad_put(b,NET_ENABLED,NET_MAGIC);io->push(b+NET_ENABLED,4,NULL);io->barrier(NULL);return 1;
}
static int db_net_host_service(void){
 if(!net_active)return 1;
 volatile uint8_t*b=net_shared;
 net_io->pull(b+NET_TX_WRITE,4,NULL);unsigned w=ad_get(b,NET_TX_WRITE),r=ad_get(b,NET_TX_READ);int n=net_pending(w,r);if(n<0)return 0;
 for(int i=0;i<n;i++,r++){
  unsigned o=net_slot(NET_TX_DATA,r);net_io->pull(b+o,4,NULL);unsigned len=ad_get(b,o);if(len<14||len>NET_MTU)return 0;
  net_io->pull(b+o+4,len,NULL);for(unsigned j=0;j<len;j++)net_packet[j]=b[o+4+j];
  if(!zz_net_input(net_packet,len))return 0;
  ad_put(b,NET_TX_READ,r+1);net_io->push(b+NET_TX_READ,4,NULL);net_io->barrier(NULL);
 }
 if(!zz_net_poll())return 0;
 net_io->pull(b+NET_RX_READ,4,NULL);w=ad_get(b,NET_RX_WRITE);r=ad_get(b,NET_RX_READ);n=net_pending(w,r);if(n<0)return 0;
 for(;(unsigned)n<NET_COUNT;n++,w++){
  unsigned len=zz_net_receive(net_packet,sizeof(net_packet));if(!len)break;unsigned o=net_slot(NET_RX_DATA,w);
  ad_put(b,o,len);for(unsigned j=0;j<len;j++)b[o+4+j]=net_packet[j];net_io->push(b+o,len+4,NULL);net_io->barrier(NULL);
  ad_put(b,NET_RX_WRITE,w+1);net_io->push(b+NET_RX_WRITE,4,NULL);net_io->barrier(NULL);
 }
 return 1;
}
static void db_net_host_close(void){if(net_active){struct zz_net_stats s=zz_net_statistics();printf("NETWORK frames_tx=%lu frames_rx=%lu dropped=%lu errors=%lu closed\n",(unsigned long)s.sent,(unsigned long)s.received,(unsigned long)s.dropped,(unsigned long)s.errors);}zz_net_close();net_active=0;}
