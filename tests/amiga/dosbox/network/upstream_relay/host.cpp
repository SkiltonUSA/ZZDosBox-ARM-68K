/* Test-only POSIX socket/timer adapter for the UNMODIFIED pinned IPX server. */
#include "dosbox.h"
#include "ipxserver.h"
#include "ipx.h"
#include "timer.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
static TIMER_TickHandler*tick;static const char*bind_ip;
void TIMER_AddTickHandler(TIMER_TickHandler*p){tick=p;}
void TIMER_DelTickHandler(TIMER_TickHandler*p){if(tick==p)tick=nullptr;}
void UnpackIP(PackedIP p,IPaddress*a){a->host=p.host;a->port=p.port;}
void PackIP(IPaddress a,PackedIP*p){p->host=a.host;p->port=a.port;}
int SDLNet_ResolveHost(IPaddress*p,const char*s,Uint16 port){p->port=htons(port);return inet_aton(s?s:bind_ip,(in_addr*)&p->host)?0:-1;}
UDPsocket SDLNet_UDP_Open(Uint16 port){int fd=socket(AF_INET,SOCK_DGRAM,0);if(fd<0)return 0;sockaddr_in a={};a.sin_family=AF_INET;a.sin_port=htons(port);inet_aton(bind_ip,&a.sin_addr);if(bind(fd,(sockaddr*)&a,sizeof(a))||fcntl(fd,F_SETFL,O_NONBLOCK)){close(fd);return 0;}return fd;}
void SDLNet_UDP_Close(UDPsocket s){close(s);}
int SDLNet_UDP_Send(UDPsocket s,int,UDPpacket*p){sockaddr_in a={};a.sin_family=AF_INET;a.sin_addr.s_addr=p->address.host;a.sin_port=p->address.port;return sendto(s,p->data,p->len,0,(sockaddr*)&a,sizeof(a))==p->len;}
int SDLNet_UDP_Recv(UDPsocket s,UDPpacket*p){sockaddr_in a={};socklen_t len=sizeof(a);int n=recvfrom(s,p->data,p->maxlen,0,(sockaddr*)&a,&len);if(n<0)return 0;p->len=n;p->address.host=a.sin_addr.s_addr;p->address.port=a.sin_port;return 1;}
int main(int argc,char**argv){if(argc!=4)return 2;bind_ip=argv[1];unsigned port=atoi(argv[2]),seconds=atoi(argv[3]);if(!port||port>65535||seconds>3600||!IPX_StartServer(port))return 3;
 puts("Pinned upstream DOSBox IPX relay ready");fflush(stdout);
 auto stop=std::chrono::steady_clock::now()+std::chrono::seconds(seconds);
 while(std::chrono::steady_clock::now()<stop){if(tick)tick();usleep(1000);}IPX_StopServer();return 0;}
