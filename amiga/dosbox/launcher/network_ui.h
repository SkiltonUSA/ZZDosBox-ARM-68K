/* GPL-3.0-or-later. Guest network setup; never modifies Roadshow or a host NIC. */
#include <exec/execbase.h>
#include "../network/control.h"
static Object *net_protocol,*net_server,*net_port,*net_tcp_group,*net_ipx_group,*net_dns_button,*net_nic_label;
static const char *net_protocols[]={"TCP/IP","IPX wrapper (UDP)",NULL};
static Object *net_enabled,*net_mode,*net_ip,*net_mask,*net_gateway,*net_dns,*net_name;
static Object *net_mtu,*net_retries,*net_timeout,*net_target,*net_status,*net_details,*net_renew,*net_release,*net_disconnect;
static char net_arguments[DL_PATH*2+48];
static const char *net_modes[]={"DHCP","Manual IPv4",NULL};
static const char *net_tabs[]={"Setup","Connection","Diagnostics",NULL};
enum {ID_NET_MODE=600,ID_NET_REFRESH,ID_NET_RENEW,ID_NET_RELEASE,ID_NET_CONNECT,ID_NET_PING,ID_NET_DNS,ID_NET_LOG,ID_NET_DEFAULT,ID_NET_DISCONNECT,ID_NET_GUIDE};
static void network_guide(void){open_document("Network.guide","Network.guide is missing from the launcher drawer.");}
static void network_controls(void);
static void network_refresh(void);
static struct MsgPort *net_replies;
static struct dn_message net_message;
static unsigned net_ticks,net_stop;
static char net_live_text[180];
static void net_send(unsigned op){struct MsgPort*server;if(network_busy||!net_replies)return;
 memset(&net_message,0,sizeof(net_message));net_message.msg.mn_Length=sizeof(net_message);net_message.msg.mn_ReplyPort=net_replies;net_message.magic=DN_CONTROL_MAGIC;net_message.op=op;
 Forbid();server=FindPort(DN_PORT);if(server)PutMsg(server,&net_message.msg);Permit();if(server)network_busy=1;
}
static int network_live_begin(void){network_refresh();net_replies=CreateMsgPort();if(!net_replies){note("Cannot create network reply port.");return 0;}network_running=1;net_ticks=net_stop=0;set(net_disconnect,MUIA_Disabled,FALSE);set(net_renew,MUIA_Disabled,TRUE);set(net_release,MUIA_Disabled,TRUE);return 1;}
static void network_live_end(void){if(net_replies&&!network_busy){DeleteMsgPort(net_replies);net_replies=NULL;}network_running=0;set(net_disconnect,MUIA_Disabled,TRUE);network_controls();}
static void network_poll(void){if(!network_running)return;
 if(net_replies&&GetMsg(net_replies)){network_busy=0;snprintf(net_live_text,sizeof(net_live_text),"Network %s  |  TX %lu  RX %lu  dropped %lu  errors %lu",net_message.op==DN_STOP?"stopping":"active",(unsigned long)net_message.tx,(unsigned long)net_message.rx,(unsigned long)net_message.dropped,(unsigned long)net_message.errors);set(net_status,MUIA_Text_Contents,(ULONG)net_live_text);}
 if(!network_busy&&(net_stop||++net_ticks>=50)){net_ticks=0;net_send(net_stop?DN_STOP:DN_STATUS);}
}
static void network_live_event(unsigned id){
 if(id==ID_NET_GUIDE){network_guide();return;}
 if(id==ID_NET_DISCONNECT||id==(ULONG)MUIV_Application_ReturnID_Quit){net_stop=1;net_send(DN_STOP);note("Stopping DOS and releasing the network connection...");}
 else if(id==ID_NET_REFRESH)net_send(DN_STATUS);
 else if(id)note("Disconnect the DOS session before changing network settings.");
}
static int net_number(Object*o,unsigned*out){const char*s=(char*)val(o,MUIA_String_Contents);unsigned n=0;if(!*s)return 0;for(;*s;s++){if(*s<'0'||*s>'9'||n>10000)return 0;n=n*10+*s-'0';}*out=n;return 1;}
static void network_controls(void){unsigned ipx=val(net_protocol,MUIA_Cycle_Active);
 set(net_tcp_group,MUIA_Disabled,ipx);set(net_ipx_group,MUIA_Disabled,!ipx);
 set(net_ip,MUIA_Disabled,ipx||!val(net_mode,MUIA_Cycle_Active));
 set(net_renew,MUIA_Disabled,ipx||val(net_mode,MUIA_Cycle_Active));set(net_release,MUIA_Disabled,ipx||val(net_mode,MUIA_Cycle_Active));
 set(net_dns_button,MUIA_Disabled,ipx);set(net_target,MUIA_Disabled,ipx);
 set(net_nic_label,MUIA_Text_Contents,(ULONG)(ipx?"DOSBox IPX/UDP client  |  configured relay server":"NE2000  |  I/O 0x300  |  IRQ 3  |  MAC 02:5A:5A:00:00:01"));
}
static int network_store(struct dn_config*c){struct dn_config n;const char*error;dn_defaults(&n);
 n.protocol=val(net_protocol,MUIA_Cycle_Active);
 snprintf(n.ipx_server,sizeof(n.ipx_server),"%s",(char*)val(net_server,MUIA_String_Contents));
 if(!net_number(net_port,&n.ipx_port)){note("Enter an IPX UDP port (1-65535).");return 0;}
 n.enabled=val(net_enabled,MUIA_Selected);n.manual=val(net_mode,MUIA_Cycle_Active);
 snprintf(n.ip,sizeof(n.ip),"%s",(char*)val(net_ip,MUIA_String_Contents));snprintf(n.mask,sizeof(n.mask),"%s",(char*)val(net_mask,MUIA_String_Contents));
 snprintf(n.gateway,sizeof(n.gateway),"%s",(char*)val(net_gateway,MUIA_String_Contents));snprintf(n.dns,sizeof(n.dns),"%s",(char*)val(net_dns,MUIA_String_Contents));snprintf(n.hostname,sizeof(n.hostname),"%s",(char*)val(net_name,MUIA_String_Contents));
 if(!net_number(net_mtu,&n.mtu)||!net_number(net_retries,&n.retries)||!net_number(net_timeout,&n.timeout)){note("Enter numeric MTU, retries and timeout.");return 0;}
 error=dn_error(&n);if(error){note(error);return 0;}*c=n;return 1;
}
static void net_num_show(Object*o,unsigned n){char s[16];snprintf(s,sizeof(s),"%u",n);set(o,MUIA_String_Contents,(ULONG)s);}
static void network_show(const struct dn_config*c){
 set(net_protocol,MUIA_Cycle_Active,c->protocol);set(net_server,MUIA_String_Contents,(ULONG)c->ipx_server);net_num_show(net_port,c->ipx_port);
 set(net_enabled,MUIA_Selected,c->enabled);set(net_mode,MUIA_Cycle_Active,c->manual);
 set(net_ip,MUIA_String_Contents,(ULONG)c->ip);set(net_mask,MUIA_String_Contents,(ULONG)c->mask);set(net_gateway,MUIA_String_Contents,(ULONG)c->gateway);set(net_dns,MUIA_String_Contents,(ULONG)c->dns);set(net_name,MUIA_String_Contents,(ULONG)c->hostname);
 net_num_show(net_mtu,c->mtu);net_num_show(net_retries,c->retries);net_num_show(net_timeout,c->timeout);network_controls();
 DoMethod(net_details,MUIM_List_Clear);set(net_status,MUIA_Text_Contents,(ULONG)"Idle - refresh for saved lease details.");
}
static Object *net_button(const char*text,unsigned id){Object*o=ui_button(text);set(o,MUIA_UserData,id);return o;}
static Object *network_page(void){return VGroup,Child,RegisterGroup(net_tabs),MUIA_Font,ui_font(),
 Child,VGroup,UI_PAGE,
 Child,HGroup,Child,net_enabled=CheckMark(FALSE),Child,label("Enable for this program (experimental)"),End,
 Child,HGroup,Child,fieldlabel("Protocol"),Child,net_protocol=cycle(net_protocols),End,
 Child,net_ipx_group=ColGroup(4),UI_PANEL("IPX relay"),Child,fieldlabel("IPX server IPv4"),Child,net_server=str(16),Child,fieldlabel("UDP port"),Child,net_port=str(6),End,
 Child,net_tcp_group=ColGroup(4),UI_PANEL("TCP/IP"),
 Child,fieldlabel("Addressing"),Child,net_mode=cycle(net_modes),Child,fieldlabel("Hostname"),Child,net_name=str(32),
 Child,fieldlabel("Manual IP"),Child,net_ip=str(16),Child,fieldlabel("Subnet mask"),Child,net_mask=str(16),
 Child,fieldlabel("NAT gateway"),Child,net_gateway=str(16),Child,fieldlabel("Upstream DNS"),Child,net_dns=str(16),
 Child,fieldlabel("MTU"),Child,net_mtu=str(5),Child,fieldlabel("DHCP retries"),Child,net_retries=str(3),
 Child,fieldlabel("Timeout (s)"),Child,net_timeout=str(3),Child,label(""),Child,net_button("Defaults",ID_NET_DEFAULT),End,
 Child,label("Uses Roadshow. IPX requires a DOSBox IPXNET relay server."),
 Child,HGroup,Child,net_button("Open network terminal",ID_NET_CONNECT),Child,net_renew=net_button("Renew DHCP",ID_NET_RENEW),Child,net_release=net_button("Release saved lease",ID_NET_RELEASE),End,Child,VSpace(0),End,
 Child,VGroup,Child,net_nic_label=label("NE2000  |  I/O 0x300  |  IRQ 3  |  MAC 02:5A:5A:00:00:01"),
 Child,net_status=label("Idle"),Child,ListviewObject,MUIA_Listview_List,net_details=ListObject,MUIA_Font,ui_font(),ReadListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
 Child,HGroup,Child,net_button("Refresh",ID_NET_REFRESH),Child,net_disconnect=net_button("Disconnect",ID_NET_DISCONNECT),End,End,
 Child,VGroup,Child,ColGroup(2),Child,fieldlabel("Host / IPv4"),Child,net_target=str(64),End,
 Child,HGroup,Child,net_button("Ping",ID_NET_PING),Child,net_dns_button=net_button("DNS lookup",ID_NET_DNS),Child,net_button("Open log",ID_NET_LOG),End,
 Child,label("Diagnostics run inside DOS. Close the terminal with F10."),Child,VSpace(0),End,
 End,
 Child,HGroup,Child,HSpace(0),Child,net_button("Network guide",ID_NET_GUIDE),End,End;
}
static void network_init(void){struct dn_config c;dn_defaults(&c);network_show(&c);
 set(net_disconnect,MUIA_Disabled,TRUE);
 notify(net_mode,MUIA_Cycle_Active,MUIV_EveryTime,ID_NET_MODE);notify(net_protocol,MUIA_Cycle_Active,MUIV_EveryTime,ID_NET_MODE);
 set(net_target,MUIA_String_Contents,(ULONG)"10.0.2.2");
 set(net_dns,MUIA_ShortHelp,(ULONG)"IPv4 address of your LAN or public DNS resolver. Forwarded by the Amiga; no Mac proxy.");
 set(net_mask,MUIA_ShortHelp,(ULONG)"Private virtual subnet, /24 or larger. Do not overlap your physical LAN.");
 set(net_renew,MUIA_ShortHelp,(ULONG)"Starts a short DOS session to obtain a fresh DHCP lease; does not change Roadshow.");
 set(net_release,MUIA_ShortHelp,(ULONG)"Clears this program's saved mTCP lease and disables networking. NAT leases end when DOS closes; no live DHCPRELEASE is sent.");
}
static int net_owned(const char*root,int create){char dir[DL_PATH],file[DL_PATH],text[64];BPTR l;FILE*f;int ok;
 if(!dl_join(dir,sizeof(dir),root,"ZZNET")||!dl_join(file,sizeof(file),dir,"ZZNET.TAG"))return 0;
 if(exists(dir)){f=fopen(file,"r");if(!f)return 0;ok=fgets(text,sizeof(text),f)&&!strcmp(text,"ZZDosBox network files v1\n");fclose(f);return ok;}
 if(!create)return 0;l=CreateDir(dir);if(!l)return 0;UnLock(l);f=fopen(file,"w");if(!f)return 0;ok=fputs("ZZDosBox network files v1\n",f)!=EOF;if(fclose(f))ok=0;return ok;
}
static int net_replace(const char*tmp,const char*dest){char bak[DL_PATH];int n=snprintf(bak,sizeof(bak),"%s.bak",dest);if(n<0||(size_t)n>=sizeof(bak))return 0;
 if(exists(dest)){if(exists(bak)&&!DeleteFile(bak))return 0;if(!Rename(dest,bak))return 0;}
 if(Rename(tmp,dest))return 1;if(exists(bak))Rename(bak,dest);return 0;
}
static int net_copy(const char*root,const char*name){char src[DL_PATH],dst[DL_PATH],tmp[DL_PATH],relative[64];unsigned char data[1024];FILE*a,*b;size_t n;int ok=1;
 snprintf(relative,sizeof(relative),"Network/%s",name);path(src,relative);snprintf(relative,sizeof(relative),"ZZNET/%s",name);
 if(!dl_join(dst,sizeof(dst),root,relative)||!dl_join(tmp,sizeof(tmp),root,"ZZNET/COPY.TMP"))return 0;
 a=fopen(src,"rb");if(!a)return 0;b=fopen(tmp,"wb");if(!b){fclose(a);return 0;}
 while((n=fread(data,1,sizeof(data),a))!=0)if(fwrite(data,1,n,b)!=n){ok=0;break;}if(ferror(a))ok=0;fclose(a);if(fclose(b))ok=0;
 if(ok)ok=net_replace(tmp,dst);if(!ok)DeleteFile(tmp);return ok;
}
static int network_prepare(const char*root,const struct dn_config*c){
 static const char*files[]={"NE2000.COM","DHCP.EXE","PING.EXE","DNSTEST.EXE","HTGET.EXE","COPYING.TXT","SOURCES.TXT"};
 char file[DL_PATH],tmp[DL_PATH];FILE*f;unsigned i;int ok;
 if(!net_owned(root,1)){note("Cannot create ZZNET, or that drawer belongs to another application.");return 0;}
 if(c->protocol)return 1;
 for(i=0;i<sizeof(files)/sizeof(files[0]);i++)if(!net_copy(root,files[i])){note("Network tools missing or destination is not writable.");return 0;}
 if(!dl_join(file,sizeof(file),root,"ZZNET/MTCP.CFG")||!dl_join(tmp,sizeof(tmp),root,"ZZNET/MTCP.TMP"))return 0;
 f=fopen(tmp,"wb");if(!f)return 0;ok=dn_mtcp(f,c);if(fclose(f))ok=0;if(!ok||!net_replace(tmp,file)){note("Cannot write guest network settings.");return 0;}
 return 1;
}
static int network_options(const struct dl_profile*p){char file[DL_PATH],q[DL_PATH*2+3];FILE*f;int ok;
 net_arguments[0]=0;network_requested=p->network.enabled;if(!p->network.enabled)return 1;
 /* dc_runtime validates the complete profile and honours explicit core choice. */
 path(file,active_runtime);if(!exists(file)){note("Selected network runtime is missing.");return 0;}
 path(file,"Network.session");f=fopen(file,"w");if(!f){note("Cannot write network session.");return 0;}ok=dn_write(f,&p->network);if(fclose(f))ok=0;if(!ok||!dl_quote(q,sizeof(q),file))return 0;
 snprintf(net_arguments,sizeof(net_arguments)," --network-config %s",q);return 1;
}
static void net_detail(const char*s){DoMethod(net_details,MUIM_List_InsertSingle,(ULONG)s,MUIV_List_Insert_Bottom);}
static void network_refresh(void){struct dn_config c;char b[256],ip[16],dns[16],file[DL_PATH];FILE*f;unsigned lines=0;struct Library*socket;
 if(!network_store(&c))return;DoMethod(net_details,MUIM_List_Clear);
 Forbid();socket=(struct Library*)FindName(&SysBase->LibList,"bsdsocket.library");snprintf(b,sizeof(b),"Roadshow socket library: %s",socket?"present (route not tested)":"not running");Permit();net_detail(b);
 if(c.protocol){snprintf(b,sizeof(b),"IPX relay: %s  UDP %u",*c.ipx_server?c.ipx_server:"not configured",c.ipx_port);net_detail(b);net_detail("DOSBox-compatible IPX over UDP through Roadshow");net_detail("Client only; no NE2000, DHCP, mTCP or Windows IPX driver");set(net_status,MUIA_Text_Contents,(ULONG)"Idle - IPX session closed.");return;}
 dn_address(ip,dn_network(&c)+15);dn_address(dns,dn_network(&c)+3);
 snprintf(b,sizeof(b),"Configured: %s  %s / %s",c.manual?"Manual":"DHCP",c.manual?c.ip:ip,c.mask);net_detail(b);
 snprintf(b,sizeof(b),"Gateway: %s   DNS proxy: %s   MTU: %u",c.gateway,*c.dns?dns:"off",c.mtu);net_detail(b);
 snprintf(b,sizeof(b),"Upstream DNS: %s",*c.dns?c.dns:"not configured");net_detail(b);
 net_detail("Transport: IPv4 NAT through the Amiga's current route");net_detail("Core auto/dynamic: ARM JIT; core normal: interpreter");
 net_detail("JIT supports EMS and Sound Blaster; bridged Ethernet unavailable");
 if(selected>=0&&net_owned(library.p[selected].drive,0)&&dl_join(file,sizeof(file),library.p[selected].drive,"ZZNET/MTCP.CFG")&&(f=fopen(file,"r"))){
  net_detail("Last saved DOS settings (not a live connection):");
  while(lines++<100&&fgets(b,sizeof(b),f)){size_t n=strlen(b);while(n&&(b[n-1]=='\r'||b[n-1]=='\n'))b[--n]=0;
   if(!strncmp(b,"IPADDR ",7)||!strncmp(b,"NETMASK ",8)||!strncmp(b,"GATEWAY ",8)||!strncmp(b,"NAMESERVER ",11)||!strncmp(b,"LEASE_TIME ",11)||!strncmp(b,"TIMESTAMP ",10)){unsigned j;for(j=0;j<n;j++)if((unsigned char)b[j]<32||(unsigned char)b[j]>126)b[j]='?';net_detail(b);}
  }fclose(f);
 }
 set(net_status,MUIA_Text_Contents,(ULONG)"Idle - NAT runs only while a network DOS session is open.");
}
static int net_target_valid(const char*s){unsigned n=0;if(!*s||*s=='-')return 0;for(;*s;s++,n++)if(n>=63||!((*s>='a'&&*s<='z')||(*s>='A'&&*s<='Z')||(*s>='0'&&*s<='9')||*s=='.'||*s=='-'))return 0;return 1;}
static void network_action(unsigned id){struct dl_profile p;struct dl_session s;char file[DL_PATH],session[DL_PATH],runtime[DL_PATH],qp[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*4+1200],target[64];FILE*f;BPTR log;int ok;LONG rc;
 if(id==ID_NET_GUIDE){network_guide();return;}
 if(id==ID_NET_DISCONNECT){note("No network session is running.");return;}
 if(id==ID_NET_MODE){network_controls();return;}
 if(id==ID_NET_DEFAULT){struct dn_config c;dn_defaults(&c);network_show(&c);return;}
 if(id==ID_NET_LOG){last_log="LastNetwork.log";developer_log();return;}
 if(id==ID_NET_REFRESH){network_refresh();return;}
 if(selected<0){note("Select a program first.");return;}
 if(FindPort("Sixies.ARM.Debug.Owner")){note("Close the running ARM application first.");return;}
 if(!store_selection())return;p=library.p[selected];
 if(id==ID_NET_RELEASE){
  if(p.network.protocol){note("IPX has no DHCP lease.");return;}
  if(p.network.manual){note("Manual addressing has no DHCP lease.");return;}
  if(net_owned(p.drive,0)&&dl_join(file,sizeof(file),p.drive,"ZZNET/MTCP.CFG")&&exists(file)&&!DeleteFile(file)){note("Cannot remove saved lease.");return;}
  set(net_enabled,MUIA_Selected,FALSE);library.p[selected].network.enabled=0;
  if(!save()){note("Lease cleared, but settings could not be saved.");return;}
  network_refresh();note("Saved lease released; networking disabled for this program.");return;
 }
 if(p.network.protocol&&(id==ID_NET_RENEW||id==ID_NET_DNS)){note("This operation applies to TCP/IP only.");return;}
 if(id==ID_NET_RENEW&&p.network.manual){note("Select DHCP to renew.");return;}
 snprintf(target,sizeof(target),"%s",(char*)val(net_target,MUIA_String_Contents));
 if(!p.network.protocol&&(id==ID_NET_PING||id==ID_NET_DNS)&&!net_target_valid(target)){note("Enter a hostname or IPv4 address.");return;}
 if(id==ID_NET_DNS&&!*p.network.dns){note("Set an upstream DNS server first.");return;}
 if(p.system){note("Use an embedded DOS profile; Windows needs its own network driver setup.");return;}
 if(!developer_options())return;p.network.enabled=1;
 if(!conf_profile(&p))return;p=effective_profile;
 if(!network_prepare(p.drive,&p.network))return;
 dl_join(file,sizeof(file),p.drive,"ZZNET/ACTION.BAT");f=fopen(file,"wb");if(!f){note("Cannot write network action.");return;}
 ok=fputs("@echo off\r\n",f)!=EOF&&dn_start(f,&p.network);
 if(id==ID_NET_PING&&p.network.protocol)ok=ok&&fputs("IPXNET PING\r\n",f)!=EOF;
 if(id==ID_NET_PING&&!p.network.protocol)ok=ok&&fprintf(f,"C:\\ZZNET\\PING.EXE -count 4 -timeout 2 %s\r\nif errorlevel 1 goto ZZNETFAIL\r\n",target)>0;
 if(id==ID_NET_DNS)ok=ok&&fprintf(f,"C:\\ZZNET\\DNSTEST.EXE -name %s -timeout 5\r\nif errorlevel 1 goto ZZNETFAIL\r\n",target)>0;
 if(!p.network.protocol)ok=ok&&fputs("type C:\\ZZNET\\MTCP.CFG\r\n",f)!=EOF;
 ok=ok&&fputs("echo Network command completed\r\ngoto ZZNETEND\r\n:ZZNETFAIL\r\necho Network command failed\r\n:ZZNETEND\r\n",f)!=EOF;if(fclose(f))ok=0;
 if(!ok){note("Cannot write network action.");return;}
 dl_defaults(&s);strcpy(s.drive,p.drive);strcpy(s.command,"C:\\ZZNET\\ACTION.BAT");s.memory=p.memory;s.cycles=dc_get(&conf,DC_CYCLES,dl_profile_cycles(&p));s.machine=p.machine;s.live=1;s.terminal=id==ID_NET_CONNECT;s.display=0;s.size=p.size;
 path(session,"NetworkRun.session");path(runtime,active_runtime);if(!dl_session_write(session,&s)||!dl_quote(qp,sizeof(qp),runtime)||!dl_quote(qs,sizeof(qs),session))return;
 snprintf(cmd,sizeof(cmd),"%s --session %s%s%s",qp,qs,dl_compat_args(&p),net_arguments);
 path(file,"LastNetwork.log");log=Open(file,MODE_NEWFILE);if(!log){note("Cannot open network log.");return;}
 last_log="LastNetwork.log";rc=player_run(cmd,log);Close(log);DeleteFile(session);set(win,MUIA_Window_Open,TRUE);
 network_refresh();note(rc?"Network session failed; see log.":"Network session ended; see log for command result.");
}
