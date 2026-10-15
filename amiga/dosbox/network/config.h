/* GPL-3.0-or-later. Validated private IPv4 configuration shared by MUI and NAT. */
#ifndef ZZ_NETWORK_CONFIG_H
#define ZZ_NETWORK_CONFIG_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct dn_config {
 unsigned enabled,manual,mtu,retries,timeout;
 char ip[16],mask[16],gateway[16],dns[16],hostname[32];
};
static inline void dn_defaults(struct dn_config*c){
 memset(c,0,sizeof(*c));c->mtu=1500;c->retries=3;c->timeout=10;
 strcpy(c->ip,"10.0.2.15");strcpy(c->mask,"255.255.255.0");strcpy(c->gateway,"10.0.2.2");strcpy(c->hostname,"zzdosbox");
}
static inline int dn_ipv4(const char*s,uint32_t*out){
 uint32_t v=0;unsigned i;
 for(i=0;i<4;i++){unsigned n=0,k=0;const char*start=s;
  while(*s>='0'&&*s<='9'){n=n*10+*s++-'0';if(++k>3||n>255)return 0;}
  if(!k||(k>1&&*start=='0'))return 0;
  v=(v<<8)|n;
  if(i<3){if(*s++!='.')return 0;}else if(*s)return 0;
 }*out=v;return 1;
}
static inline void dn_address(char*out,uint32_t n){snprintf(out,16,"%u.%u.%u.%u",(unsigned)(n>>24),(unsigned)((n>>16)&255),(unsigned)((n>>8)&255),(unsigned)(n&255));}
static inline int dn_host(const char*s){unsigned n=0;if(!*s||*s=='-')return 0;for(;*s;s++,n++)if(n>=31||!((*s>='a'&&*s<='z')||(*s>='A'&&*s<='Z')||(*s>='0'&&*s<='9')||*s=='-'))return 0;return s[-1]!='-';}
static inline int dn_unicast(uint32_t n){return (n>>24)!=0&&(n>>24)!=127&&(n>>24)<224;}
static inline const char*dn_error(const struct dn_config*c){
 uint32_t ip,mask,gw,dns,net,end,inv;
 if(c->enabled>1||c->manual>1)return "Invalid network mode.";
 if(!dn_ipv4(c->ip,&ip)||!dn_ipv4(c->mask,&mask)||!dn_ipv4(c->gateway,&gw))return "Enter dotted IPv4 addresses and a subnet mask.";
 inv=~mask;if(!mask||(inv&(inv+1))||inv<255)return "Use a contiguous subnet mask, /24 or larger network.";
 net=gw&mask;end=net|inv;
 if(!((net>=0x0a000000u&&end<=0x0affffffu)||(net>=0xac100000u&&end<=0xac1fffffu)||(net>=0xc0a80000u&&end<=0xc0a8ffffu)))return "Use a private IPv4 subnet (10, 172.16-31 or 192.168).";
 if(gw==net||gw==end||gw==net+3||(gw>=net+15&&gw<=net+30))return "Gateway conflicts with a reserved NAT/DHCP address.";
 if(c->manual&&((ip&mask)!=net||ip==net||ip==end||ip==gw||ip==net+3))return "Manual IP must be a free host address in the NAT subnet.";
 if(*c->dns&&(!dn_ipv4(c->dns,&dns)||!dn_unicast(dns)||(dns&mask)==net))return "DNS must be an upstream IPv4 address outside the guest subnet.";
 if(!dn_host(c->hostname))return "Hostname: 1-31 letters, digits or internal hyphens.";
 if(c->mtu<576||c->mtu>1500)return "MTU must be 576-1500.";
 if(c->retries<1||c->retries>10||c->timeout<5||c->timeout>30)return "DHCP: 1-10 retries, 5-30 seconds per attempt.";
 return NULL;
}
static inline uint32_t dn_network(const struct dn_config*c){uint32_t g=0,m=0;dn_ipv4(c->gateway,&g);dn_ipv4(c->mask,&m);return g&m;}
static inline int dn_write(FILE*f,const struct dn_config*c){
 if(dn_error(c))return 0;
 return fprintf(f,"ZZNET1\n%u %u %u %u %u\n%s\n%s\n%s\n%s\n%s\n",c->enabled,c->manual,c->mtu,c->retries,c->timeout,c->ip,c->mask,c->gateway,c->dns,c->hostname)>0;
}
static inline int dn_line(FILE*f,char*s,size_t cap){size_t n;if(!fgets(s,cap,f))return 0;n=strlen(s);if(!n||s[n-1]!='\n')return 0;s[--n]=0;if(n&&s[n-1]=='\r')s[n-1]=0;return 1;}
/* Reads one record; the enclosing profile reader owns its EOF check. */
static inline int dn_read(FILE*f,struct dn_config*c){char b[96],extra;struct dn_config next;
 dn_defaults(&next);
 if(!dn_line(f,b,sizeof(b))||strcmp(b,"ZZNET1")||!dn_line(f,b,sizeof(b))||sscanf(b,"%u %u %u %u %u %c",&next.enabled,&next.manual,&next.mtu,&next.retries,&next.timeout,&extra)!=5)return 0;
 if(!dn_line(f,next.ip,sizeof(next.ip))||!dn_line(f,next.mask,sizeof(next.mask))||!dn_line(f,next.gateway,sizeof(next.gateway))||!dn_line(f,next.dns,sizeof(next.dns))||!dn_line(f,next.hostname,sizeof(next.hostname))||dn_error(&next))return 0;
 *c=next;return 1;
}
static inline int dn_file_read(const char*path,struct dn_config*c){FILE*f=fopen(path,"r");int ok;if(!f)return 0;ok=dn_read(f,c)&&fgetc(f)==EOF;fclose(f);return ok;}
static inline int dn_mtcp(FILE*f,const struct dn_config*c){char dns[16];if(dn_error(c))return 0;dn_address(dns,dn_network(c)+3);
 if(fprintf(f,"# ZZDosBox generated network settings\r\npacketint 0x60\r\nhostname %s\r\nmtu %u\r\n",c->hostname,c->mtu)<0)return 0;
 if(c->manual&&fprintf(f,"IPADDR %s\r\nNETMASK %s\r\nGATEWAY %s\r\nNAMESERVER %s\r\n",c->ip,c->mask,c->gateway,*c->dns?dns:"0.0.0.0")<0)return 0;
 return !ferror(f);
}
/* Commands always use our managed subdirectory; no user text enters a shell. */
static inline int dn_start(FILE*f,const struct dn_config*c){if(dn_error(c))return 0;
 if(fputs("C:\\ZZNET\\NE2000.COM 0x60 3 0x300\r\nif errorlevel 1 goto ZZNETFAIL\r\nset MTCPCFG=C:\\ZZNET\\MTCP.CFG\r\n",f)==EOF)return 0;
 if(!c->manual&&fprintf(f,"C:\\ZZNET\\DHCP.EXE -retries %u -timeout %u\r\nif errorlevel 1 goto ZZNETFAIL\r\n",c->retries,c->timeout)<0)return 0;
 return !ferror(f);
}
#endif
