/* GPL-3.0-or-later. Program profiles and editable DOS startup files. */
#ifndef DL_MODEL_H
#define DL_MODEL_H
#include "session.h"
#include "../network/config.h"
#define DL_MAX 256
enum {DL_GAMES,DL_APPLICATIONS,DL_OS,DL_CATEGORIES};
#include "../pc/joystick_plan.h"
#include "../pc/serial_config.h"
struct dl_profile {struct dn_config network;struct db_serial_config serial;char serial_host[16],title[64],drive[DL_PATH],program[128],autoexec[DL_TEXT],configsys[DL_TEXT],auto_file[DL_PATH],config_file[DL_PATH],cd_path[DL_PATH],midi_port[64],image[13];unsigned joy_buttons,joy_keys[4],paged_jit,serial_mode,serial_port,expanded,cpu_model,vcpi,umb,joystick,extra_sound,cycle_mode,ems,sound,speaker,helper,edited,memory,machine,speed,fg,bg,live,display,fps,system,size,config_mode,cd_mode,midi,scanlines,presentation,monochrome,renderer,category;};
struct dl_library {unsigned count,last;struct dl_profile p[DL_MAX];};
enum {DL_SPEED_COUNT=6};
static inline unsigned dl_profile_cycles(const struct dl_profile*p){static const unsigned cycles[]={1000,3000,6000,10000,60000,100000};return p->speed<DL_SPEED_COUNT?cycles[p->speed]:0;}
static const char dl_default_autoexec[]="@echo off\npath C:\\\n";
static const char dl_default_config[]="REM CONFIG.SYS reference only: not executed by this core\nREM DEVICE=C:\\DOS\\HIMEM.SYS\nREM DEVICE=C:\\DOS\\EMM386.EXE NOEMS\nREM DOS=HIGH,UMB\n";
static void dl_profile_default(struct dl_profile*p){memset(p,0,sizeof(*p));dn_defaults(&p->network);db_serial_defaults(&p->serial);p->joy_buttons=2;p->joy_keys[0]=1;p->joy_keys[1]=2;p->joy_keys[2]=5;p->joy_keys[3]=6;p->serial_port=23;strcpy(p->serial_host,"127.0.0.1");strcpy(p->autoexec,dl_default_autoexec);strcpy(p->configsys,dl_default_config);p->category=DL_APPLICATIONS;p->memory=2;p->speed=1;p->fg=7;p->live=1;p->size=1;strcpy(p->midi_port,"out.0");}
/* Software scanlines: the verified path excludes window and FPS overlay. */
static int dl_scanlines_available(unsigned display,unsigned fps){return display==1&&fps==0;}
static int dl_scanlines_active(const struct dl_profile*p){return p->scanlines>=1&&p->scanlines<=3&&dl_scanlines_available(p->display,p->fps);}
static const char *dl_scanline_args(const struct dl_profile*p){
 static const char *args[]={""," --scanlines classic"," --scanlines soft"," --scanlines gradient"};
 return dl_scanlines_active(p)?args[p->scanlines]:"";
}
/* Normal keeps the existing player; Adaptive also supports scanline styles. */
static const char *dl_player_name(unsigned presentation,unsigned masked){return presentation==1?"ZZDosBox-Adaptive":masked?"ZZDosBox-Scanlines":"ZZDosBox";}
static const char *dl_presentation_args(unsigned presentation){return presentation==1?" --presentation adaptive --present-hz 60":"";}
static int dl_profile_expanded(const struct dl_profile*p){return dl_profile_cycles(p)>10000||p->paged_jit||p->serial_mode||p->expanded||p->memory>8||p->cpu_model||p->vcpi||p->umb||p->joystick||p->extra_sound||p->cycle_mode;}
static const char *dl_hardware_args(const struct dl_profile*p){
 static char args[640];static const char*cpu[]={"386","486","pentium"},*sound[]={"off","gus","cms","tandy"},*cycles[]={"fixed","auto","max"};
 if(!dl_profile_expanded(p))return "";
 snprintf(args,sizeof(args)," --cpu %s --vcpi %s --umb %s --joystick %s --extra-sound %s --cycle-mode %s --serial %s --serial-host %s --serial-port %u --paged-jit %s",cpu[p->cpu_model],p->vcpi?"on":"off",p->umb?"on":"off",p->joystick?"2axis":"off",sound[p->extra_sound],cycles[p->cycle_mode],p->serial_mode==3?"device":p->serial_mode==2?"modem":p->serial_mode?"tcp":"off",p->serial_host,p->serial_port,p->paged_jit?"on":"off");if(p->joystick)snprintf(args+strlen(args),sizeof(args)-strlen(args)," --joy-buttons %u --joy-keys %u,%u,%u,%u",p->joy_buttons,p->joy_keys[0],p->joy_keys[1],p->joy_keys[2],p->joy_keys[3]);if(p->serial_mode==3)snprintf(args+strlen(args),sizeof(args)-strlen(args)," --serial-device %s --serial-line %u,%u,%u,%u,%u,%u",p->serial.device,p->serial.unit,p->serial.baud,p->serial.bits,p->serial.parity,p->serial.stop,p->serial.flow);return args;
}
static const char *dl_profile_player(const struct dl_profile*p){return dl_profile_expanded(p)?"ZZDosBox-Expanded":(p->ems||p->sound||p->speaker)?"ZZDosBox-Compat":p->renderer?"ZZDosBox-OpenGL":dl_player_name(p->presentation,dl_scanlines_active(p));}
static const char *dl_compat_args(const struct dl_profile*p){
 static char args[80];static const char *cards[]={"off","sbpro2","sb16"};
 if(!dl_profile_expanded(p)&&!p->ems&&!p->sound&&!p->speaker)return "";
 snprintf(args,sizeof(args)," --ems %s --sound %s --speaker %s",p->ems?"on":"off",cards[p->sound],p->speaker?"on":"off");return args;
}
static inline const char *dl_mono_args(unsigned n){static const char *args[]={" --monochrome white"," --monochrome amber"," --monochrome green"," --monochrome paperwhite"};return n<4?args[n]:"";}
static int dl_text_valid(const char*s);
static int dl_profile_valid(const struct dl_profile*p){uint32_t address;return db_joy_config_valid(p->joy_buttons,p->joy_keys)&&p->paged_jit<=1&&p->serial_mode<=3&&db_serial_config_valid(&p->serial)&&p->serial_port>=1&&p->serial_port<=65535&&dn_ipv4(p->serial_host,&address)&& p->cpu_model<DB_CPU_COUNT&&p->vcpi<=1&&p->umb<=1&&p->joystick<=1&&p->extra_sound<=3&&p->cycle_mode<=2&&p->ems<=1&&p->sound<=2&&p->speaker<=1&&!dn_error(&p->network)&&*p->title&&dl_plain(p->title)&&*p->drive&&dl_plain(p->drive)&&dl_plain(p->program)&&p->helper<=1&&p->edited<=2&&db_memory_valid(p->memory,32)&&p->machine<PC_ADAPTER_COUNT&&p->speed<DL_SPEED_COUNT&&p->fg<8&&p->bg<8&&p->fg!=p->bg&&p->live<=1&&p->display<=1&&p->fps<=1&&p->system<=3&&((p->system>=2)?dl_image_valid(p->image):!*p->image)&&p->size<3&&p->config_mode<=2&&p->scanlines<=3&&p->presentation<=1&&p->monochrome<4&&p->renderer<2&&p->category<DL_CATEGORIES&&p->midi<=2&&dl_plain(p->midi_port)&&(!p->midi||*p->midi_port)&&p->cd_mode<=2&&dl_plain(p->auto_file)&&dl_plain(p->config_file)&&dl_plain(p->cd_path)&&(!p->cd_mode||*p->cd_path)&&(p->edited!=2||*p->auto_file)&&(p->config_mode!=2||*p->config_file)&&dl_text_valid(p->autoexec)&&dl_text_valid(p->configsys);}
static int dl_hex_write(FILE*f,const char*s){static const char h[]="0123456789abcdef";for(;*s;s++){unsigned c=(unsigned char)*s;if(fputc(h[c>>4],f)==EOF||fputc(h[c&15],f)==EOF)return 0;}return fputc('\n',f)!=EOF;}
static int dl_hex_n(int c){return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;}
static int dl_hex_read(FILE*f,char*s,size_t cap){size_t n=0;int a,b;while((a=fgetc(f))!='\n'){if(a==EOF||n+1>=cap||(a=dl_hex_n(a))<0||(b=dl_hex_n(fgetc(f)))<0||!(a||b))return 0;s[n++]=(char)((a<<4)|b);}s[n]=0;return 1;}
/* Category filtering keeps stable profile indices, including after retagging. */
static int dl_category_index(const struct dl_library*m,unsigned category,int row){
 unsigned i;if(category>=DL_CATEGORIES||row<0)return -1;
 for(i=0;i<m->count;i++)if(m->p[i].category==category&&row--==0)return (int)i;
 return -1;
}
static unsigned dl_legacy_category(const struct dl_profile*p){
 if(p->system||!*p->program)return DL_OS;
 /* Existing launchers primarily contained games; unknown programs remain editable. */
 if(strstr(p->drive,"Games")||strstr(p->drive,"GAMES")||
    !strcmp(p->program,"KEEN6C.EXE")||!strcmp(p->program,"VETTE.EXE")||
    !strcmp(p->program,"ELITE.EXE")||!strcmp(p->program,"DOOM.EXE"))return DL_GAMES;
 return DL_APPLICATIONS;
}
static int dl_library_read(const char*path,struct dl_library*m){
 FILE*f=fopen(path,"r");char line[160],extra;unsigned i;int ok=0,v;if(!f)return 0;memset(m,0,sizeof(*m));
 if(!dl_line(f,line,sizeof(line)))goto done;
 v=!strcmp(line,"ZZDOSLIB19")?19:!strcmp(line,"ZZDOSLIB18")?18:!strcmp(line,"ZZDOSLIB17")?17:!strcmp(line,"ZZDOSLIB16")?16:!strcmp(line,"ZZDOSLIB15")?15:!strcmp(line,"ZZDOSLIB14")?14:!strcmp(line,"ZZDOSLIB13")?13:!strcmp(line,"ZZDOSLIB12")?12:!strcmp(line,"ZZDOSLIB11")?11:!strcmp(line,"ZZDOSLIB10")?10:!strcmp(line,"ZZDOSLIB9")?9:!strcmp(line,"ZZDOSLIB8")?8:!strcmp(line,"ZZDOSLIB7")?7:!strcmp(line,"ZZDOSLIB6")?6:!strcmp(line,"ZZDOSLIB5")?5:!strcmp(line,"ZZDOSLIB4")?4:!strcmp(line,"ZZDOSLIB3")?3:!strcmp(line,"ZZDOSLIB2")?2:!strcmp(line,"ZZDOSLIB1")?1:0;
 if(!v||!dl_line(f,line,sizeof(line))||sscanf(line,"%u %u %c",&m->count,&m->last,&extra)!=2||m->count>DL_MAX||m->last>=(m->count?m->count:1))goto done;
 for(i=0;i<m->count;i++){
  struct dl_profile*p=&m->p[i];ok=0;dn_defaults(&p->network);db_serial_defaults(&p->serial);p->joy_buttons=2;p->joy_keys[0]=1;p->joy_keys[1]=2;p->joy_keys[2]=5;p->joy_keys[3]=6;p->serial_port=23;strcpy(p->serial_host,"127.0.0.1");p->size=1;strcpy(p->midi_port,"out.0");
  if(!dl_hex_read(f,p->title,sizeof(p->title))||!dl_hex_read(f,p->drive,sizeof(p->drive))||!dl_hex_read(f,p->program,sizeof(p->program))||!dl_hex_read(f,p->autoexec,sizeof(p->autoexec))||!dl_hex_read(f,p->configsys,sizeof(p->configsys))||!dl_line(f,line,sizeof(line)))goto done;
  if(v==1){p->display=1;ok=sscanf(line,"%u %u %u %u %u %u %u %u %c",&p->helper,&p->edited,&p->memory,&p->machine,&p->speed,&p->fg,&p->bg,&p->live,&extra)==8;}
  else if(v==2)ok=sscanf(line,"%u %u %u %u %u %u %u %u %u %u %u %c",&p->helper,&p->edited,&p->memory,&p->machine,&p->speed,&p->fg,&p->bg,&p->live,&p->display,&p->fps,&p->system,&extra)==11;
  else ok=sscanf(line,"%u %u %u %u %u %u %u %u %u %u %u %u %c",&p->helper,&p->edited,&p->memory,&p->machine,&p->speed,&p->fg,&p->bg,&p->live,&p->display,&p->fps,&p->system,&p->size,&extra)==12;
  if(ok&&v>=4)ok=dl_hex_read(f,p->auto_file,sizeof(p->auto_file))&&dl_hex_read(f,p->config_file,sizeof(p->config_file))&&dl_hex_read(f,p->cd_path,sizeof(p->cd_path))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %c",&p->config_mode,&p->cd_mode,&extra)==2;
  if(ok&&v>=5)ok=dl_hex_read(f,p->midi_port,sizeof(p->midi_port))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->midi,&extra)==1;
  if(ok&&v>=6)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->scanlines,&extra)==1;
  if(ok&&v>=7)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->presentation,&extra)==1;
  if(ok&&v>=8)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->monochrome,&extra)==1;
  if(ok&&v>=9)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->renderer,&extra)==1;
  if(ok&&v>=10)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->category,&extra)==1;
  else if(ok)p->category=dl_legacy_category(p);
  if(ok&&v>=11)ok=dl_hex_read(f,p->image,sizeof(p->image));
  if(ok&&v>=12)ok=dn_read(f,&p->network);
  if(ok&&v>=15)ok=dl_line(f,line,sizeof(line))&&(v>=16?sscanf(line,"%u %u %u %c",&p->ems,&p->sound,&p->speaker,&extra)==3:sscanf(line,"%u %u %c",&p->ems,&p->sound,&extra)==2);
  if(ok&&v>=17)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %u %u %u %u %c",&p->cpu_model,&p->vcpi,&p->umb,&p->joystick,&p->extra_sound,&p->cycle_mode,&extra)==6;
  if(ok&&v>=17)ok=dl_line(f,p->serial_host,sizeof(p->serial_host))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %u %c",&p->serial_mode,&p->serial_port,&p->paged_jit,&extra)==3;
  if(ok&&v>=18)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %u %u %u %c",&p->joy_buttons,&p->joy_keys[0],&p->joy_keys[1],&p->joy_keys[2],&p->joy_keys[3],&extra)==5;
  if(ok&&v>=19)ok=dl_hex_read(f,p->serial.device,sizeof(p->serial.device))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %u %u %u %u %c",&p->serial.unit,&p->serial.baud,&p->serial.bits,&p->serial.parity,&p->serial.stop,&p->serial.flow,&extra)==6;
  if(!ok||!dl_profile_valid(p)){ok=0;goto done;}
 }
 ok=fgetc(f)==EOF;
done:fclose(f);return ok;
}
static int dl_library_write(const char*path,const struct dl_library*m){FILE*f;unsigned i;int ok=1;if(m->count>DL_MAX||m->last>=(m->count?m->count:1))return 0;for(i=0;i<m->count;i++)if(!dl_profile_valid(&m->p[i]))return 0;f=fopen(path,"w");if(!f)return 0;if(fprintf(f,"ZZDOSLIB19\n%u %u\n",m->count,m->last)<0)ok=0;for(i=0;i<m->count&&ok;i++){const struct dl_profile*p=&m->p[i];ok=dl_hex_write(f,p->title)&&dl_hex_write(f,p->drive)&&dl_hex_write(f,p->program)&&dl_hex_write(f,p->autoexec)&&dl_hex_write(f,p->configsys)&&fprintf(f,"%u %u %u %u %u %u %u %u %u %u %u %u\n",p->helper,p->edited,p->memory,p->machine,p->speed,p->fg,p->bg,p->live,p->display,p->fps,p->system,p->size)>0;if(ok)ok=dl_hex_write(f,p->auto_file)&&dl_hex_write(f,p->config_file)&&dl_hex_write(f,p->cd_path)&&fprintf(f,"%u %u\n",p->config_mode,p->cd_mode)>0;if(ok)ok=dl_hex_write(f,p->midi_port)&&fprintf(f,"%u\n",p->midi)>0;if(ok)ok=fprintf(f,"%u\n%u\n%u\n%u\n",p->scanlines,p->presentation,p->monochrome,p->renderer)>0;if(ok)ok=fprintf(f,"%u\n",p->category)>0;if(ok)ok=dl_hex_write(f,p->image);if(ok)ok=dn_write(f,&p->network);if(ok)ok=fprintf(f,"%u %u %u\n",p->ems,p->sound,p->speaker)>0;if(ok)ok=fprintf(f,"%u %u %u %u %u %u\n",p->cpu_model,p->vcpi,p->umb,p->joystick,p->extra_sound,p->cycle_mode)>0;if(ok)ok=fprintf(f,"%s\n%u %u %u\n",p->serial_host,p->serial_mode,p->serial_port,p->paged_jit)>0;if(ok)ok=fprintf(f,"%u %u %u %u %u\n",p->joy_buttons,p->joy_keys[0],p->joy_keys[1],p->joy_keys[2],p->joy_keys[3])>0;if(ok)ok=dl_hex_write(f,p->serial.device)&&fprintf(f,"%u %u %u %u %u %u\n",p->serial.unit,p->serial.baud,p->serial.bits,p->serial.parity,p->serial.stop,p->serial.flow)>0;}return fclose(f)==0&&ok;}
static int dl_text_valid(const char*s){size_t n=0,col=0,lines=0;for(;*s;s++,n++){if(n>=DL_TEXT-1||((unsigned char)*s<32&&*s!='\n'&&*s!='\r'&&*s!='\t')||(unsigned char)*s>=127)return 0;if(*s=='\n'){col=0;if(++lines>128)return 0;}else if(*s!='\r'&&++col>255)return 0;}return lines<128||!col;}

/* Save-as creates CRLF files usable by DOS editors too. */
static int dl_dos_text(FILE*f,const char*s){if(!dl_text_valid(s))return 0;for(;*s;s++){if(*s=='\r')continue;if(*s=='\n'&&fputc('\r',f)==EOF)return 0;if(fputc(*s,f)==EOF)return 0;}return 1;}
static int dl_boot(FILE*f,const struct dl_profile*p,const char*command,int use_auto,int helper){const char*s=use_auto?(p->edited?p->autoexec:dl_default_autoexec):"@echo off\n";if(!dl_text_valid(s)||!dl_plain(command)||strlen(command)>127)return 0;if(!dl_dos_text(f,s)||fputs("\r\n",f)==EOF)return 0;if(*command&&fprintf(f,"%s%s\r\n",helper?"LOADFIX ":"",command)<0)return 0;return !ferror(f);}
#endif
