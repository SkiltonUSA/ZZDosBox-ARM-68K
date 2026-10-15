/* GPL-3.0-or-later. Socket ownership stays on the Amiga task; no blocking I/O. */
#include "backend.h"
#include "ipx_packet.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "libslirp.h"
#ifdef __amigaos__
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/bsdsocket.h>
#undef if_mtu
#define TIMER_BASE_NAME NetTimerBase
#include <proto/timer.h>
struct Library *SocketBase;
static struct Device *NetTimerBase;
static struct MsgPort *timer_port;
static struct timerequest *timer_io;
#else
#include <unistd.h>
#include <fcntl.h>
#endif
struct in_addr zz_net_dns;
static Slirp *net;
static int ipx_socket=-1;
static uint8_t ipx_packet[ZZ_NET_MTU+1];
static struct dn_config configuration;
static int configured;
int zz_net_configure(const struct dn_config *c){if(net||ipx_socket>=0||!c||dn_error(c))return 0;configuration=*c;configured=1;return 1;}
void zz_net_configuration(struct dn_config *c){if(!configured)dn_defaults(&configuration);*c=configuration;}
static struct zz_net_stats stats;
static unsigned head,tail,polls,fault;
static uint8_t frames[ZZ_NET_QUEUE][ZZ_NET_MTU];
static unsigned lengths[ZZ_NET_QUEUE];
static struct { int fd,events; } sockets[64];
static fd_set rd,wr,ex;
static int maxfd;
static int64_t net_time(void *unused) {
 (void)unused;
#ifdef __amigaos__
 struct timeval t;GetSysTime(&t);return (int64_t)t.tv_secs*1000000000LL+(int64_t)t.tv_micro*1000;
#else
 struct timeval t;gettimeofday(&t,NULL);return (int64_t)t.tv_sec*1000000000LL+(int64_t)t.tv_usec*1000;
#endif
}
static slirp_ssize_t output(const void *buf,size_t len,void *unused) {
 (void)unused;
 if(len<14||len>ZZ_NET_MTU||head-tail>=ZZ_NET_QUEUE){stats.dropped++;return len;}
 unsigned slot=head%ZZ_NET_QUEUE;memcpy(frames[slot],buf,len);lengths[slot]=len;head++;stats.received++;return len;
}
static void guest_error(const char *msg,void *unused){(void)msg;(void)unused;stats.errors++;}
static void notice(void *unused){(void)unused;}
static void register_socket(slirp_os_socket fd,void *unused){(void)unused;if(fd<0||fd>=FD_SETSIZE)fault=1;}
static void unregister_socket(slirp_os_socket fd,void *unused){(void)fd;(void)unused;}
static int add_poll(slirp_os_socket fd,int events,void *unused) {
 (void)unused;if(fd<0||fd>=FD_SETSIZE||polls>=64){fault=1;return -1;}
 unsigned i=polls++;sockets[i].fd=fd;sockets[i].events=events;
 if(events&SLIRP_POLL_IN)FD_SET(fd,&rd);if(events&SLIRP_POLL_OUT)FD_SET(fd,&wr);if(events&SLIRP_POLL_PRI)FD_SET(fd,&ex);
 if(fd>maxfd)maxfd=fd;return i;
}
static int get_events(int i,void *unused) {
 (void)unused;if(i<0||(unsigned)i>=polls)return 0;int fd=sockets[i].fd,r=0;
 if(FD_ISSET(fd,&rd))r|=SLIRP_POLL_IN;if(FD_ISSET(fd,&wr))r|=SLIRP_POLL_OUT;if(FD_ISSET(fd,&ex))r|=SLIRP_POLL_PRI;return r;
}
static const SlirpCb callbacks={.send_packet=output,.guest_error=guest_error,.clock_get_ns=net_time,.notify=notice,.register_poll_socket=register_socket,.unregister_poll_socket=unregister_socket};
int zz_net_open(const char *dns) {
 zz_net_close();memset(&stats,0,sizeof(stats));head=tail=polls=fault=0;
 if(!configured)dn_defaults(&configuration);
#ifdef __amigaos__
 Forbid();int online=FindName(&SysBase->LibList,(CONST_STRPTR)"bsdsocket.library")!=NULL;Permit();if(!online)return 0;
 SocketBase=OpenLibrary("bsdsocket.library",4);if(!SocketBase)goto bad;
 SetErrnoPtr(&errno,sizeof(errno));
 timer_port=CreateMsgPort();if(!timer_port)goto bad;
 timer_io=(struct timerequest*)CreateIORequest(timer_port,sizeof(*timer_io));if(!timer_io)goto bad;
 if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)timer_io,0))goto bad;
 NetTimerBase=timer_io->tr_node.io_Device;
#endif
 if(configuration.protocol){
  struct sockaddr_in peer;memset(&peer,0,sizeof(peer));peer.sin_family=AF_INET;
#ifdef __amigaos__
  peer.sin_len=sizeof(peer);
#endif
  peer.sin_port=htons(configuration.ipx_port);
  if(!inet_aton(configuration.ipx_server,&peer.sin_addr))goto bad;
  ipx_socket=socket(AF_INET,SOCK_DGRAM,0);if(ipx_socket<0||ipx_socket>=FD_SETSIZE)goto bad;
#ifdef __amigaos__
  {unsigned long on=1;if(IoctlSocket(ipx_socket,FIONBIO,(char*)&on))goto bad;}
#else
  {int flags=fcntl(ipx_socket,F_GETFL,0);if(flags<0||fcntl(ipx_socket,F_SETFL,flags|O_NONBLOCK))goto bad;}
#endif
  if(connect(ipx_socket,(struct sockaddr*)&peer,sizeof(peer)))goto bad;
  return 1;
 }
 SlirpConfig c;memset(&c,0,sizeof(c));c.version=6;c.in_enabled=true;c.disable_host_loopback=true;
 if(!dns)dns=configuration.dns;
 if(dns&&*dns){uint32_t address;if(!dn_ipv4(dns,&address)||!dn_unicast(address)||!inet_aton(dns,&zz_net_dns))goto bad;}else zz_net_dns.s_addr=0;
 c.disable_dns=!zz_net_dns.s_addr;
 {char address[16];uint32_t subnet=dn_network(&configuration);
  dn_address(address,subnet);inet_aton(address,&c.vnetwork);inet_aton(configuration.mask,&c.vnetmask);inet_aton(configuration.gateway,&c.vhost);
  dn_address(address,subnet+15);inet_aton(address,&c.vdhcp_start);dn_address(address,subnet+3);inet_aton(address,&c.vnameserver);
 }
 c.vhostname=configuration.hostname;c.if_mtu=c.if_mru=configuration.mtu;
 net=slirp_new(&c,&callbacks,NULL);if(!net)goto bad;
 /* Seed libslirp's timers before a first packet creates a UDP mapping. */
 if(!zz_net_poll())goto bad;return 1;
bad:zz_net_close();return 0;
}
void zz_net_close(void) {
 if(ipx_socket>=0){
#ifdef __amigaos__
  CloseSocket(ipx_socket);
#else
  close(ipx_socket);
#endif
  ipx_socket=-1;
 }
 if(net){slirp_cleanup(net);net=NULL;}
#ifdef __amigaos__
 if(NetTimerBase){CloseDevice((struct IORequest*)timer_io);NetTimerBase=NULL;}
 if(timer_io){DeleteIORequest((struct IORequest*)timer_io);timer_io=NULL;}
 if(timer_port){DeleteMsgPort(timer_port);timer_port=NULL;}
 if(SocketBase){CloseLibrary(SocketBase);SocketBase=NULL;}
#endif
 head=tail=0;
}
int zz_net_input(const uint8_t *frame,unsigned size) {
 if(ipx_socket>=0){
  if(!zz_ipx_valid(frame,size)){stats.dropped++;return 1;}
  int n=send(ipx_socket,(const void*)frame,size,0);
  if(n==(int)size){stats.sent++;return 1;}
  if(n<0&&(errno==EWOULDBLOCK||errno==EAGAIN||errno==EINTR)){stats.dropped++;return 1;}
  stats.errors++;return 0;
 }
 if(!net||fault||!frame||size<14||size>ZZ_NET_MTU)return 0;
 /* IPv4 and ARP only; IPv6/NC-SI/VLAN are not exposed by this backend. */
 if(!(frame[12]==8&&(frame[13]==0||frame[13]==6))){stats.dropped++;return 1;}
 slirp_input(net,frame,size);stats.sent++;return !fault;
}
int zz_net_poll(void) {
 if(ipx_socket>=0){
  for(unsigned i=0;i<ZZ_NET_QUEUE;i++){
   int n=recv(ipx_socket,(void*)ipx_packet,sizeof(ipx_packet),0);
   if(n<0){if(errno==EWOULDBLOCK||errno==EAGAIN||errno==EINTR)return 1;stats.errors++;return 0;}
   if(!zz_ipx_valid(ipx_packet,(unsigned)n)){stats.dropped++;continue;}
   output(ipx_packet,(unsigned)n,NULL);
  }return 1;
 }
 if(!net||fault)return 0;uint32_t timeout=0;polls=0;maxfd=-1;FD_ZERO(&rd);FD_ZERO(&wr);FD_ZERO(&ex);
 slirp_pollfds_fill_socket(net,&timeout,add_poll,NULL);if(fault)return 0;
 struct timeval t={0,0};
#ifdef __amigaos__
 int n=WaitSelect(maxfd+1,&rd,&wr,&ex,&t,NULL);
#else
 int n=select(maxfd+1,&rd,&wr,&ex,&t);
#endif
 slirp_pollfds_poll(net,n<0,get_events,NULL);if(n<0&&errno!=EINTR){stats.errors++;return 0;}return !fault;
}
unsigned zz_net_receive(uint8_t *frame,unsigned capacity) {
 if(!frame)return 0;
 while(head!=tail){
  unsigned i=tail%ZZ_NET_QUEUE,n=lengths[i];
  tail++;
  if(n>capacity){stats.dropped++;continue;}
  memcpy(frame,frames[i],n);
  return n;
 }
 return 0;
}
struct zz_net_stats zz_net_statistics(void){return stats;}
