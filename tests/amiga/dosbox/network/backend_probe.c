/* GPL-3.0-or-later. Original Ethernet/IP/TCP fixture, no guest OS required.
 * Checks the real NAT backend using a bounded local TCP/UDP echo endpoint. */
#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef __amigaos__
#include <proto/dos.h>
#else
#include <unistd.h>
#endif
static const unsigned char mac[6]={2,0x5a,0x5a,0,0,1},gw[6]={0x52,0x55,10,0,2,2};
static const unsigned char ip[4]={10,0,2,15};
static const char message[]="ZZDosBox guest network echo 0123456789";
static unsigned char large_message[4099];
static const unsigned char *tcp_message=(const unsigned char*)message;
static unsigned tcp_length=sizeof(message)-1;
static unsigned char target[4];
static unsigned port;
static unsigned rd16(const unsigned char*p){return (unsigned)p[0]*256+p[1];}
static uint32_t rd32(const unsigned char*p){return (uint32_t)rd16(p)<<16|rd16(p+2);}
static void wr16(unsigned char*p,unsigned v){p[0]=v>>8;p[1]=v;}
static void wr32(unsigned char*p,uint32_t v){wr16(p,v>>16);wr16(p+2,v);}
static unsigned sum(const unsigned char*p,unsigned n,unsigned s){while(n>1){s+=rd16(p);p+=2;n-=2;}if(n)s+=*p<<8;while(s>>16)s=(s&65535)+(s>>16);return s;}
static void pause_tick(void){
#ifdef __amigaos__
 Delay(1);
#else
 usleep(20000);
#endif
}
static void arp(void){
 unsigned char b[60]={0};memset(b,255,6);memcpy(b+6,mac,6);wr16(b+12,0x806);
 wr16(b+14,1);wr16(b+16,0x800);b[18]=6;b[19]=4;wr16(b+20,1);
 memcpy(b+22,mac,6);memcpy(b+28,ip,4);memcpy(b+38,"\x0a\0\2\2",4);
 if(!zz_net_input(b,sizeof b))exit(10);
}
static void send_ip(unsigned protocol,unsigned flags,uint32_t seq,uint32_t ack,const void *data,unsigned len){
 unsigned char b[1600]={0};unsigned h=protocol==6?20:8,n=20+h+len;
 if(len>1460){puts("FAIL fixture segment bounds");exit(12);}
 memcpy(b,gw,6);memcpy(b+6,mac,6);wr16(b+12,0x800);b[14]=0x45;wr16(b+16,n);b[22]=64;b[23]=protocol;
 memcpy(b+26,ip,4);memcpy(b+30,target,4);wr16(b+24,~sum(b+14,20,0));
 unsigned char*t=b+34;wr16(t,43210);wr16(t+2,port);
 if(protocol==6){wr32(t+4,seq);wr32(t+8,ack);t[12]=0x50;t[13]=flags;wr16(t+14,32768);}
 else wr16(t+4,h+len);
 memcpy(t+h,data,len);unsigned s=sum(b+26,8,protocol+h+len);s=sum(t,h+len,s);wr16(t+(protocol==6?16:6),~s);
 if(!zz_net_input(b,n+14))exit(11);
}
int main(int argc,char**argv){
 unsigned a,b,c,d;unsigned char pkt[ZZ_NET_MTU];unsigned udp=0,tcp=0,arp_ok=0,established=0,bytes=0;uint32_t serverseq=0;
 if((argc!=3&&argc!=4)||(argc==4&&strcmp(argv[3],"--large"))||sscanf(argv[1],"%u.%u.%u.%u",&a,&b,&c,&d)!=4||a>255||b>255||c>255||d>255||(port=strtoul(argv[2],NULL,10))<1024||port>65535)return 2;
 target[0]=a;target[1]=b;target[2]=c;target[3]=d;
 if(argc==4){for(unsigned i=0;i<sizeof(large_message);i++)large_message[i]=(unsigned char)(i%251);tcp_message=large_message;tcp_length=sizeof(large_message);}
 printf("Starting NAT probe, TCP payload=%u\n",tcp_length);fflush(stdout);
 if(!zz_net_open(NULL)){puts("FAIL network open");return 3;}
 arp();send_ip(17,0,0,0,message,sizeof(message)-1);send_ip(6,2,1000,0,"",0);
 for(unsigned tick=0;tick<750&&!(udp&&tcp&&arp_ok);tick++){
  if(tick&&tick%50==0&&!udp)send_ip(17,0,0,0,message,sizeof(message)-1);
  if(!zz_net_poll()){puts("FAIL network poll");break;}
  unsigned n;
  while((n=zz_net_receive(pkt,sizeof pkt))){
   if(n>=42&&rd16(pkt+12)==0x806&&rd16(pkt+20)==2){arp_ok=1;continue;}
   if(n<42||rd16(pkt+12)!=0x800||pkt[14]!=0x45||memcmp(pkt+30,ip,4))continue;
   unsigned total=rd16(pkt+16);if(total<28||total>n-14||sum(pkt+14,20,0)!=65535){puts("FAIL IP checksum/length");goto done;}
   unsigned char*t=pkt+34;unsigned plen=total-20;if(rd16(t)!=port||rd16(t+2)!=43210)continue;
   if(pkt[23]==17){if(plen==8+sizeof(message)-1&&rd16(t+4)==plen&&!memcmp(t+8,message,sizeof(message)-1))udp=1;else goto done;}
   if(pkt[23]==6){
    if(plen<20||sum(t,plen,sum(pkt+26,8,6+plen))!=65535){puts("FAIL TCP checksum");goto done;}
    unsigned hdr=(t[12]>>4)*4;if(hdr<20||hdr>plen)goto done;
    if((t[13]&0x12)==0x12&&!established){if(rd32(t+8)!=1001)goto done;serverseq=rd32(t+4)+1;established=1;send_ip(6,0x10,1001,serverseq,"",0);for(unsigned off=0;off<tcp_length;){unsigned chunk=tcp_length-off;if(chunk>512)chunk=512;send_ip(6,0x18,1001+off,serverseq,tcp_message+off,chunk);off+=chunk;}}
    if(established&&plen>hdr){unsigned data=plen-hdr;if(rd32(t+4)!=serverseq+bytes||data>tcp_length-bytes||memcmp(t+hdr,tcp_message+bytes,data))goto done;bytes+=data;send_ip(6,0x10,1001+tcp_length,serverseq+bytes,"",0);if(bytes==tcp_length){tcp=1;send_ip(6,0x14,1001+bytes,serverseq+bytes,"",0);}}
   }
  }
  pause_tick();
 }
done:;
 struct zz_net_stats s=zz_net_statistics();zz_net_close();
 printf("NAT ARP=%u UDP=%u TCP=%u bytes=%u tx=%lu rx=%lu dropped=%lu errors=%lu\n",arp_ok,udp,tcp,bytes,(unsigned long)s.sent,(unsigned long)s.received,(unsigned long)s.dropped,(unsigned long)s.errors);
 if(!(arp_ok&&udp&&tcp)||s.errors)return 5;
 if(!zz_net_open(NULL))return 6;zz_net_close();puts("PASS guest Ethernet -> NAT -> host sockets; close/reopen");return 0;
}
