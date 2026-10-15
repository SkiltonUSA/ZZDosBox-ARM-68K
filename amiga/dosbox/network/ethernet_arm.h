/* GPL-3.0-or-later. Minimal frame interface for the borrowed NE2000 device. */
#pragma once
#include <stdint.h>
extern bool db_net_enabled();
extern void db_net_send(const uint8_t *,unsigned);
extern unsigned db_net_receive(uint8_t *);
class EthernetConnection {
public:
 void SendPacket(const uint8_t*p,int n){if(n>0)db_net_send(p,(unsigned)n);}
 template<class F> void GetPackets(F receive){uint8_t packet[1518];for(unsigned i=0;i<4;i++){unsigned n=db_net_receive(packet);if(!n)break;receive(packet,(int)n);}}
};
