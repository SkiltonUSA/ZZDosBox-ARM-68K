/* GPL-3.0-or-later. DOSBox IPX packets use the existing bounded ARM queues. */
#include "SDL_net.h"
#include "ipx_packet.h"
#include "net_wire.h"
#include <string.h>
extern "C" volatile uint8_t *db_shared();
extern void db_net_send(const uint8_t*,unsigned);
extern unsigned db_net_receive(uint8_t*);
static bool open_socket;
static uint8_t packet[NET_MTU];
bool db_ipx_enabled(){return ad_get(db_shared(),NET_ENABLED)==NET_MAGIC&&ad_get(db_shared(),NET_PROTOCOL)==1;}
void db_ipx_connected(unsigned on){ad_put(db_shared(),NET_IPX_CONNECTED,on);__asm__ volatile("dsb sy":::"memory");}
int SDLNet_ResolveHost(IPaddress*out,const char*,Uint16){
 if(!db_ipx_enabled())return -1;
 SDLNet_Write32(ad_get(db_shared(),NET_SERVER),&out->host);SDLNet_Write16(ad_get(db_shared(),NET_PORT),&out->port);return 0;
}
UDPsocket SDLNet_UDP_Open(Uint16){if(!db_ipx_enabled())return 0;for(unsigned i=0;i<NET_COUNT;i++)if(!db_net_receive(packet))break;open_socket=true;return 1;}
void SDLNet_UDP_Close(UDPsocket){open_socket=false;db_ipx_connected(0);}
int SDLNet_UDP_Bind(UDPsocket s,int,const IPaddress*){return s&&open_socket?0:-1;}
int SDLNet_UDP_Send(UDPsocket s,int,UDPpacket*p){
 if(!s||!open_socket||!p||p->len<0||p->len>p->maxlen||!zz_ipx_valid(p->data,p->len))return 0;
 db_net_send(p->data,p->len);return 1;
}
int SDLNet_UDP_Recv(UDPsocket s,UDPpacket*p){
 if(!s||!open_socket||!p||!p->data)return 0;
 unsigned n=db_net_receive(packet);if(!n||!zz_ipx_valid(packet,n)||p->maxlen<0||n>(unsigned)p->maxlen)return 0;
 memcpy(p->data,packet,n);p->len=n;p->channel=0;SDLNet_ResolveHost(&p->address,"",0);return 1;
}
