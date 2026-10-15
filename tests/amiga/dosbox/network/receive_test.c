/* Exercise production queue insertion and receive without sockets. */
#include <assert.h>
#include "backend.c"
int main(void){
 uint8_t packet[ZZ_NET_MTU],buffer[ZZ_NET_MTU];
 memset(packet,0x93,sizeof(packet));memset(buffer,0x61,sizeof(buffer));
 head=tail=UINT32_MAX-1;
 output(packet,1518,NULL);packet[0]=0x42;output(packet,60,NULL);
 assert(zz_net_receive(NULL,sizeof(buffer))==0&&stats.dropped==0);
 assert(zz_net_receive(buffer,60)==60&&buffer[0]==0x42&&buffer[60]==0x61);
 assert(head==tail&&stats.dropped==1);
 assert(!zz_net_receive(buffer,sizeof(buffer)));
 output(packet,1518,NULL);output(packet,1518,NULL);
 assert(!zz_net_receive(buffer,60)&&head==tail&&stats.dropped==3);
 output(packet,60,NULL);
 assert(zz_net_receive(buffer,sizeof(buffer))==60&&buffer[0]==0x42);
 output(packet,60,NULL);assert(!zz_net_receive(buffer,0)&&stats.dropped==4);
 for(unsigned i=0;i<ZZ_NET_QUEUE;i++)output(packet,1518,NULL);
 output(packet,60,NULL);assert(stats.dropped==5);
 for(unsigned i=0;i<ZZ_NET_QUEUE;i++)assert(zz_net_receive(buffer,sizeof(buffer))==1518);
 assert(head==tail);
 puts("PASS network receive: oversized drop, next packet, no truncation, full queue, uint32 wrap, NULL and zero capacity");
 return 0;
}
