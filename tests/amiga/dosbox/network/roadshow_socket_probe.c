/* Standalone IPv4 UDP diagnostic: no libslirp, ARM worker or shared memory. */
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/bsdsocket.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct Library *SocketBase;
static char message[]="ZZDosBox Roadshow UDP diagnostic";
int main(int argc,char **argv){
 unsigned a,b,c,d;char extra;long port;int fd=-1,rc=5;
 if(argc!=3||sscanf(argv[1],"%u.%u.%u.%u%c",&a,&b,&c,&d,&extra)!=4||a>255||b>255||c>255||d>255)return 2;
 char *end;port=strtol(argv[2],&end,10);if(*end||port<1024||port>65535)return 2;
 Forbid();int online=FindName(&SysBase->LibList,(CONST_STRPTR)"bsdsocket.library")!=NULL;Permit();
 if(!online){puts("Roadshow is not running");return 3;}
 SocketBase=OpenLibrary("bsdsocket.library",4);if(!SocketBase)return 3;
 SetErrnoPtr(&errno,sizeof(errno));
 fd=socket(AF_INET,SOCK_DGRAM,0);printf("UDP socket fd=%d errno=%d\n",fd,errno);fflush(stdout);
 if(fd<0)goto done;
 unsigned long nonblock=1;
 if(IoctlSocket(fd,FIONBIO,(char*)&nonblock)<0){printf("FIONBIO errno=%d\n",errno);goto done;}
 struct sockaddr_in dest;memset(&dest,0,sizeof(dest));dest.sin_len=sizeof(dest);dest.sin_family=AF_INET;
 dest.sin_port=htons((unsigned short)port);dest.sin_addr.s_addr=htonl((a<<24)|(b<<16)|(c<<8)|d);
 int n=sendto(fd,message,sizeof(message)-1,0,(struct sockaddr*)&dest,sizeof(dest));
 printf("UDP send=%d expected=%u errno=%d sockaddr_bytes=%u family=%u\n",n,(unsigned)(sizeof(message)-1),errno,(unsigned)sizeof(dest),(unsigned)dest.sin_family);fflush(stdout);
 if(n!=(int)sizeof(message)-1)goto done;
 for(unsigned tick=0;tick<250;tick++){
  if(SetSignal(0,0)&SIGBREAKF_CTRL_C){puts("Cancelled");break;}
  char reply[128];struct sockaddr_in from;memset(&from,0,sizeof(from));socklen_t size=sizeof(from);
  n=recvfrom(fd,reply,sizeof(reply),0,(struct sockaddr*)&from,&size);
  if(n>=0){
   printf("UDP receive=%d peer=%08lx port=%u\n",n,(unsigned long)ntohl(from.sin_addr.s_addr),(unsigned)ntohs(from.sin_port));
   if(n==(int)sizeof(message)-1&&!memcmp(reply,message,n)&&from.sin_addr.s_addr==dest.sin_addr.s_addr&&from.sin_port==dest.sin_port){puts("PASS exact UDP echo");rc=0;}
   break;
  }
  if(errno!=EWOULDBLOCK&&errno!=EAGAIN){printf("UDP recvfrom errno=%d\n",errno);break;}
  Delay(1);
 }
done:
 if(fd>=0){int closed=CloseSocket(fd);printf("UDP CloseSocket=%d errno=%d\n",closed,errno);if(closed<0)rc=6;}
 CloseLibrary(SocketBase);SocketBase=NULL;printf("Socket library closed; result=%d\n",rc);return rc;
}
