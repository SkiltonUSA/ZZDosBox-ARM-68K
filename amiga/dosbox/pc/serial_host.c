/* GPL-3.0-or-later. COM1 TCP, Hayes TCP and experimental physical serial. */
#include "serial_host.h"
#include "serial_wire.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef __amigaos__
#include <proto/exec.h>
#include <proto/bsdsocket.h>
struct Library *SocketBase;
#define socket_close CloseSocket
#else
#include <unistd.h>
#include <fcntl.h>
#define socket_close close
#endif
static volatile uint8_t *shared;
static const struct ad_io *io;
static int fd=-1,connecting,carrier;
static unsigned mode,echo,last_dtr;
static uint32_t connect_started;
static struct sockaddr_in peer;
static char command[96];static unsigned command_length;
static uint8_t reply[512];static unsigned reply_read,reply_write;
static uint32_t get(unsigned at){io->pull(shared+at,4,NULL);return ad_get(shared,at);}
static void put(unsigned at,uint32_t v){ad_put(shared,at,v);io->push(shared+at,4,NULL);io->barrier(NULL);}
static void sync_ring(unsigned data,unsigned index,unsigned count,int push){
 unsigned offset=index&(PC_SERIAL_SIZE-1),first=PC_SERIAL_SIZE-offset;
 if(first>count)first=count;
 if(!count)return;
 if(push)io->push(shared+data+offset,first,NULL);else io->pull(shared+data+offset,first,NULL);
 if(count>first){if(push)io->push(shared+data,count-first,NULL);else io->pull(shared+data,count-first,NULL);}
}
#ifdef __amigaos__
#include "serial_device.h"
#endif
static int blocked(void){return errno==EWOULDBLOCK||errno==EAGAIN||errno==EINPROGRESS;}
static void status(void){put(PC_SERIAL_STATUS,2u|(carrier?1u:0u));}
static void disconnect(void){if(fd>=0)socket_close(fd);fd=-1;connecting=carrier=0;status();}
static int answer(const char*s){unsigned n=strlen(s);if(n>sizeof(reply)-(reply_write-reply_read))return 0;while(*s)reply[reply_write++%sizeof(reply)]=(uint8_t)*s++;return 1;}
static int connect_peer(void){
 int result;fd=socket(AF_INET,SOCK_STREAM,0);if(fd<0||fd>=FD_SETSIZE){disconnect();return 0;}
#ifdef __amigaos__
 {unsigned long on=1;if(IoctlSocket(fd,FIONBIO,(char*)&on)){disconnect();return 0;}}
#else
 {int flags=fcntl(fd,F_GETFL,0);if(flags<0||fcntl(fd,F_SETFL,flags|O_NONBLOCK)){disconnect();return 0;}}
#endif
 result=connect(fd,(struct sockaddr*)&peer,sizeof(peer));
 if(result&& !blocked()){disconnect();return 0;}
 connect_started=0;connecting=result!=0;carrier=!connecting;status();
 return 1;
}
static int modem_byte(uint8_t value){
 char c=(char)value;
 if(echo){char text[2]={c,0};if(!answer(text))return 0;}
 if(c==10)return 1;
 if(c==8){if(command_length)command_length--;return 1;}
 if(c!=13){if(command_length+1>=sizeof(command))return answer("\r\nERROR\r\n");command[command_length++]=toupper((unsigned char)c);return 1;}
 command[command_length]=0;command_length=0;
 if(!strcmp(command,"AT")||!strcmp(command,"ATV1"))return answer("\r\nOK\r\n");
 if(!strcmp(command,"ATZ")||!strcmp(command,"AT&F")){disconnect();echo=1;return answer("\r\nOK\r\n");}
 if(!strcmp(command,"ATE0")||!strcmp(command,"ATE1")){echo=command[3]=='1';return answer("\r\nOK\r\n");}
 if(!strcmp(command,"ATH")||!strcmp(command,"ATH0")){disconnect();return answer("\r\nOK\r\n");}
 /* The configured endpoint owns routing; ATD/ATDT dial that endpoint. */
 if(!strcmp(command,"ATD")||!strcmp(command,"ATDT")||!strcmp(command,"ATDP")){
  if(fd>=0)disconnect();
  if(!connect_peer())return answer("\r\nNO CARRIER\r\n");
  return connecting?1:answer("\r\nCONNECT\r\n");
 }
 return answer("\r\nERROR\r\n");
}
int db_serial_host_open(volatile uint8_t*b,const struct ad_io*sync,unsigned selected,const char*address,unsigned port,const struct db_serial_config*physical){
 shared=b;io=sync;mode=selected;echo=1;last_dtr=command_length=reply_read=reply_write=0;
 if(mode>3||!port||port>65535)return 0;
 memset(&peer,0,sizeof(peer));peer.sin_family=AF_INET;peer.sin_port=htons(port);
#ifdef __amigaos__
 peer.sin_len=sizeof(peer);
#endif
 put(PC_SERIAL_MODE,mode);put(PC_SERIAL_TX_WRITE,0);put(PC_SERIAL_TX_READ,0);put(PC_SERIAL_RX_WRITE,0);put(PC_SERIAL_RX_READ,0);put(PC_SERIAL_DTR,0);put(PC_SERIAL_STATUS,0);put(PC_SERIAL_CONFIG,12u|(3u<<16));put(PC_SERIAL_BREAK,0);
 if(!mode)return 1;
 if(mode==3){
#ifdef __amigaos__
  return ser_open(physical);
#else
  (void)physical;return 0;
#endif
 }
#ifdef __amigaos__
 SocketBase=OpenLibrary("bsdsocket.library",4);if(!SocketBase)return 0;
 SetErrnoPtr(&errno,sizeof(errno));
#endif
 #ifdef __amigaos__
 if(!inet_aton((unsigned char*)address,&peer.sin_addr))return 0;
#else
 if(!inet_aton(address,&peer.sin_addr))return 0;
#endif
 status();return mode==2||connect_peer();
}
int db_serial_host_poll(uint32_t now){
 uint8_t bytes[256];unsigned w,r,n,i,dtr;int result;
 if(!mode)return 1;
#ifdef __amigaos__
 if(mode==3)return ser_poll();
#endif
 dtr=get(PC_SERIAL_DTR);if(last_dtr&&!dtr&&mode==2){disconnect();if(!answer("\r\nNO CARRIER\r\n"))return 0;}last_dtr=dtr;
 if(connecting&&!connect_started)connect_started=now;
 if(connecting&&now-connect_started>10000000u){disconnect();if(mode==2&&!answer("\r\nNO CARRIER\r\n"))return 0;}
 if(connecting){
  fd_set write_set;struct timeval timeout;memset(&timeout,0,sizeof(timeout));FD_ZERO(&write_set);FD_SET(fd,&write_set);
#ifdef __amigaos__
  result=WaitSelect(fd+1,NULL,&write_set,NULL,&timeout,NULL);
#else
  result=select(fd+1,NULL,&write_set,NULL,&timeout);
#endif
  if(result<0)return 0;
  if(result){int error=0;
#ifdef __amigaos__
   unsigned long length=sizeof(error);
#else
   socklen_t length=sizeof(error);
#endif
   if(getsockopt(fd,SOL_SOCKET,SO_ERROR,&error,&length)||error){disconnect();if(mode==2&&!answer("\r\nNO CARRIER\r\n"))return 0;}
   else{connecting=0;carrier=1;status();if(mode==2&&!answer("\r\nCONNECT\r\n"))return 0;}
  }
 }
 w=get(PC_SERIAL_TX_WRITE);r=get(PC_SERIAL_TX_READ);n=w-r;if(n>PC_SERIAL_SIZE)return 0;if(n>sizeof(bytes))n=sizeof(bytes);
 sync_ring(PC_SERIAL_TX_DATA,r,n,0);for(i=0;i<n;i++)bytes[i]=shared[PC_SERIAL_TX_DATA+((r+i)&(PC_SERIAL_SIZE-1))];
 if(n){
  if(carrier){result=send(fd,bytes,n,0);if(result<0){if(!blocked())disconnect();result=0;}}
  else if(mode==2&&!connecting){for(i=0;i<n;i++){if(!modem_byte(bytes[i]))return 0;if(connecting||carrier){i++;break;}}result=i;}
  else result=0;
  if(result)put(PC_SERIAL_TX_READ,r+result);
 }
 w=get(PC_SERIAL_RX_WRITE);r=get(PC_SERIAL_RX_READ);n=w-r;if(n>PC_SERIAL_SIZE)return 0;n=PC_SERIAL_SIZE-n;if(n>sizeof(bytes))n=sizeof(bytes);
 result=0;
 if(n&&reply_write!=reply_read){while((unsigned)result<n&&reply_read!=reply_write)bytes[result++]=reply[reply_read++%sizeof(reply)];}
 else if(n&&carrier){result=recv(fd,bytes,n,0);if(result<=0){if(!result||!blocked()){disconnect();if(mode==2&&!answer("\r\nNO CARRIER\r\n"))return 0;}result=0;}}
 for(i=0;i<(unsigned)result;i++){unsigned at=PC_SERIAL_RX_DATA+((w+i)&(PC_SERIAL_SIZE-1));shared[at]=bytes[i];}
 if(result){sync_ring(PC_SERIAL_RX_DATA,w,result,1);io->barrier(NULL);put(PC_SERIAL_RX_WRITE,w+result);}
 return 1;
}
void db_serial_host_close(void){
#ifdef __amigaos__
 ser_close();
#endif
 if(fd>=0)socket_close(fd);fd=-1;carrier=connecting=mode=0;
#ifdef __amigaos__
 if(SocketBase){CloseLibrary(SocketBase);SocketBase=NULL;}
#endif
}
