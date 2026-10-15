/* Native sanitizer checks for untrusted UI/config input and generated DOS files. */
#include <assert.h>
#include "model.h"
static void readall(FILE*f,char*b){size_t n;rewind(f);n=fread(b,1,4095,f);b[n]=0;}
int main(void){struct dn_config c,d;uint32_t n;FILE*f;char b[4096];
 dn_defaults(&c);assert(!dn_error(&c));assert(!c.enabled&&!c.manual);
 assert(dn_ipv4("10.0.2.15",&n)&&n==0x0a00020f);
 const char*bad[]={"10.0.2","10.0.2.256","010.0.2.15","10.0.2.15x","1..2.3","-1.2.3.4","1.2.3.4\n","9999999999.1.2.3"};
 for(unsigned i=0;i<sizeof(bad)/sizeof(bad[0]);i++)assert(!dn_ipv4(bad[i],&n));
 strcpy(c.dns,"10.0.0.1");c.enabled=1;c.manual=1;strcpy(c.ip,"10.0.2.42");
 f=tmpfile();assert(f&&dn_write(f,&c));rewind(f);assert(dn_read(f,&d)&&!memcmp(&c,&d,sizeof(c)));fclose(f);
 f=tmpfile();assert(dn_mtcp(f,&c));readall(f,b);assert(strstr(b,"IPADDR 10.0.2.42\r\n")&&strstr(b,"NAMESERVER 10.0.2.3\r\n"));assert(!strstr(b,"LEASE_TIME"));fclose(f);
 c.manual=0;f=tmpfile();assert(dn_mtcp(f,&c));readall(f,b);assert(!strstr(b,"IPADDR"));fclose(f);
 f=tmpfile();assert(dn_start(f,&c));readall(f,b);assert(strstr(b,"DHCP.EXE -retries 3 -timeout 10")&&strstr(b,"if errorlevel 1 goto ZZNETFAIL"));fclose(f);
 c.manual=1;f=tmpfile();assert(dn_start(f,&c));readall(f,b);assert(!strstr(b,"DHCP.EXE"));fclose(f);
 strcpy(c.ip,"10.0.0.40");assert(dn_error(&c));strcpy(c.ip,"10.0.2.3");assert(dn_error(&c));strcpy(c.ip,"10.0.2.42");
 strcpy(c.mask,"255.0.255.0");assert(dn_error(&c));strcpy(c.mask,"255.255.255.128");assert(dn_error(&c));strcpy(c.mask,"255.255.255.0");
 strcpy(c.gateway,"10.0.2.15");assert(dn_error(&c));strcpy(c.gateway,"10.0.2.2");
 strcpy(c.dns,"127.0.0.1");assert(dn_error(&c));strcpy(c.dns,"10.0.2.3");assert(dn_error(&c));strcpy(c.dns,"8.8.8.8");assert(!dn_error(&c));
 strcpy(c.hostname,"bad\nrun evil");assert(dn_error(&c));strcpy(c.hostname,"-bad");assert(dn_error(&c));strcpy(c.hostname,"good-host");
 c.mtu=1501;assert(dn_error(&c));c.mtu=575;assert(dn_error(&c));c.mtu=576;assert(!dn_error(&c));c.timeout=31;assert(dn_error(&c));c.timeout=10;c.retries=0;assert(dn_error(&c));c.retries=3;
 strcpy(c.ip,"192.168.77.42");strcpy(c.gateway,"192.168.77.2");assert(!dn_error(&c));assert(dn_network(&c)==0xc0a84d00u);
 strcpy(c.gateway,"8.8.8.2");assert(dn_error(&c));dn_defaults(&c);c.enabled=1;
 f=tmpfile();assert(dn_write(f,&c));fputs("extra\n",f);rewind(f);assert(dn_read(f,&d)&&fgetc(f)!=EOF);fclose(f);
 f=tmpfile();fputs("ZZNET1\n1 0 1500 3 10\n10.0.2.15\n255.255.255.0\n10.0.2.2\n\n",f);rewind(f);assert(!dn_read(f,&d));fclose(f);
 puts("PASS IPv4/subnet/DNS/MTU validation, config roundtrip, DHCP/static generation and failure branches");return 0;
}
