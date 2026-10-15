/* GPL-3.0-or-later. ARM-side frame handoff; the host owns Roadshow sockets. */
#include <stdint.h>
#include "net_wire.h"
#include "dosbox.h"
extern "C" volatile uint8_t *db_shared();
bool db_net_enabled(){return ad_get(db_shared(),NET_ENABLED)==NET_MAGIC;}
void db_net_send(const uint8_t *p,unsigned n){
 volatile uint8_t*b=db_shared();unsigned w=ad_get(b,NET_TX_WRITE),r=ad_get(b,NET_TX_READ);
 __asm__ volatile("dsb sy":::"memory");int pending=net_pending(w,r);
 if(pending<0)E_Exit("Network TX ownership");
 if(n<14||n>NET_MTU||pending==NET_COUNT)return; /* Ethernet may drop, never block. */
 unsigned o=net_slot(NET_TX_DATA,w);ad_put(b,o,n);for(unsigned i=0;i<n;i++)b[o+4+i]=p[i];
 __asm__ volatile("dsb sy":::"memory");ad_put(b,NET_TX_WRITE,w+1);__asm__ volatile("dsb sy":::"memory");
}
unsigned db_net_receive(uint8_t*p){
 volatile uint8_t*b=db_shared();unsigned w=ad_get(b,NET_RX_WRITE),r=ad_get(b,NET_RX_READ);
 __asm__ volatile("dsb sy":::"memory");int pending=net_pending(w,r);
 if(pending<0)E_Exit("Network RX ownership");if(!pending)return 0;
 unsigned o=net_slot(NET_RX_DATA,r),n=ad_get(b,o);if(n<14||n>NET_MTU)E_Exit("Network RX size");
 for(unsigned i=0;i<n;i++)p[i]=b[o+4+i];
 __asm__ volatile("dsb sy":::"memory");ad_put(b,NET_RX_READ,r+1);__asm__ volatile("dsb sy":::"memory");return n;
}
