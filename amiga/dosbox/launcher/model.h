/* GPL-3.0-or-later. Program profiles and editable DOS startup files. */
#ifndef DL_MODEL_H
#define DL_MODEL_H
#include "session.h"
#define DL_MAX 32
struct dl_profile {char title[64],drive[DL_PATH],program[128],autoexec[DL_TEXT],configsys[DL_TEXT],auto_file[DL_PATH],config_file[DL_PATH],cd_path[DL_PATH],midi_port[64];unsigned helper,edited,memory,machine,speed,fg,bg,live,display,fps,system,size,config_mode,cd_mode,midi;};
struct dl_library {unsigned count,last;struct dl_profile p[DL_MAX];};
static const char dl_default_autoexec[]="@echo off\npath C:\\\n";
static const char dl_default_config[]="REM CONFIG.SYS reference only: not executed by this core\nREM DEVICE=C:\\DOS\\HIMEM.SYS\nREM DEVICE=C:\\DOS\\EMM386.EXE NOEMS\nREM DOS=HIGH,UMB\n";
static void dl_profile_default(struct dl_profile*p){memset(p,0,sizeof(*p));strcpy(p->autoexec,dl_default_autoexec);strcpy(p->configsys,dl_default_config);p->memory=2;p->speed=1;p->fg=7;p->live=1;p->size=1;strcpy(p->midi_port,"out.0");}
static int dl_text_valid(const char*s);
static int dl_profile_valid(const struct dl_profile*p){return *p->title&&dl_plain(p->title)&&*p->drive&&dl_plain(p->drive)&&dl_plain(p->program)&&p->helper<=1&&p->edited<=2&&(p->memory==1||p->memory==2||p->memory==4||p->memory==8)&&p->machine<=2&&p->speed<3&&p->fg<8&&p->bg<8&&p->fg!=p->bg&&p->live<=1&&p->display<=1&&p->fps<=1&&p->system<=1&&p->size<3&&p->config_mode<=2&&p->midi<=2&&dl_plain(p->midi_port)&&(!p->midi||*p->midi_port)&&p->cd_mode<=2&&dl_plain(p->auto_file)&&dl_plain(p->config_file)&&dl_plain(p->cd_path)&&(!p->cd_mode||*p->cd_path)&&(p->edited!=2||*p->auto_file)&&(p->config_mode!=2||*p->config_file)&&dl_text_valid(p->autoexec)&&dl_text_valid(p->configsys);}
static int dl_hex_write(FILE*f,const char*s){static const char h[]="0123456789abcdef";for(;*s;s++){unsigned c=(unsigned char)*s;if(fputc(h[c>>4],f)==EOF||fputc(h[c&15],f)==EOF)return 0;}return fputc('\n',f)!=EOF;}
static int dl_hex_n(int c){return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;}
static int dl_hex_read(FILE*f,char*s,size_t cap){size_t n=0;int a,b;while((a=fgetc(f))!='\n'){if(a==EOF||n+1>=cap||(a=dl_hex_n(a))<0||(b=dl_hex_n(fgetc(f)))<0||!(a||b))return 0;s[n++]=(char)((a<<4)|b);}s[n]=0;return 1;}
static int dl_library_read(const char*path,struct dl_library*m){
 FILE*f=fopen(path,"r");char line[160],extra;unsigned i;int ok=0,v;if(!f)return 0;memset(m,0,sizeof(*m));
 if(!dl_line(f,line,sizeof(line)))goto done;
 v=!strcmp(line,"ZZDOSLIB5")?5:!strcmp(line,"ZZDOSLIB4")?4:!strcmp(line,"ZZDOSLIB3")?3:!strcmp(line,"ZZDOSLIB2")?2:!strcmp(line,"ZZDOSLIB1")?1:0;
 if(!v||!dl_line(f,line,sizeof(line))||sscanf(line,"%u %u %c",&m->count,&m->last,&extra)!=2||m->count>DL_MAX||m->last>=(m->count?m->count:1))goto done;
 for(i=0;i<m->count;i++){
  struct dl_profile*p=&m->p[i];ok=0;p->size=1;strcpy(p->midi_port,"out.0");
  if(!dl_hex_read(f,p->title,sizeof(p->title))||!dl_hex_read(f,p->drive,sizeof(p->drive))||!dl_hex_read(f,p->program,sizeof(p->program))||!dl_hex_read(f,p->autoexec,sizeof(p->autoexec))||!dl_hex_read(f,p->configsys,sizeof(p->configsys))||!dl_line(f,line,sizeof(line)))goto done;
  if(v==1){p->display=1;ok=sscanf(line,"%u %u %u %u %u %u %u %u %c",&p->helper,&p->edited,&p->memory,&p->machine,&p->speed,&p->fg,&p->bg,&p->live,&extra)==8;}
  else if(v==2)ok=sscanf(line,"%u %u %u %u %u %u %u %u %u %u %u %c",&p->helper,&p->edited,&p->memory,&p->machine,&p->speed,&p->fg,&p->bg,&p->live,&p->display,&p->fps,&p->system,&extra)==11;
  else ok=sscanf(line,"%u %u %u %u %u %u %u %u %u %u %u %u %c",&p->helper,&p->edited,&p->memory,&p->machine,&p->speed,&p->fg,&p->bg,&p->live,&p->display,&p->fps,&p->system,&p->size,&extra)==12;
  if(ok&&v>=4)ok=dl_hex_read(f,p->auto_file,sizeof(p->auto_file))&&dl_hex_read(f,p->config_file,sizeof(p->config_file))&&dl_hex_read(f,p->cd_path,sizeof(p->cd_path))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %c",&p->config_mode,&p->cd_mode,&extra)==2;
  if(ok&&v>=5)ok=dl_hex_read(f,p->midi_port,sizeof(p->midi_port))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&p->midi,&extra)==1;
  if(!ok||!dl_profile_valid(p)){ok=0;goto done;}
 }
 ok=fgetc(f)==EOF;
done:fclose(f);return ok;
}
static int dl_library_write(const char*path,const struct dl_library*m){FILE*f;unsigned i;int ok=1;if(m->count>DL_MAX||m->last>=(m->count?m->count:1))return 0;for(i=0;i<m->count;i++)if(!dl_profile_valid(&m->p[i]))return 0;f=fopen(path,"w");if(!f)return 0;if(fprintf(f,"ZZDOSLIB5\n%u %u\n",m->count,m->last)<0)ok=0;for(i=0;i<m->count&&ok;i++){const struct dl_profile*p=&m->p[i];ok=dl_hex_write(f,p->title)&&dl_hex_write(f,p->drive)&&dl_hex_write(f,p->program)&&dl_hex_write(f,p->autoexec)&&dl_hex_write(f,p->configsys)&&fprintf(f,"%u %u %u %u %u %u %u %u %u %u %u %u\n",p->helper,p->edited,p->memory,p->machine,p->speed,p->fg,p->bg,p->live,p->display,p->fps,p->system,p->size)>0;if(ok)ok=dl_hex_write(f,p->auto_file)&&dl_hex_write(f,p->config_file)&&dl_hex_write(f,p->cd_path)&&fprintf(f,"%u %u\n",p->config_mode,p->cd_mode)>0;if(ok)ok=dl_hex_write(f,p->midi_port)&&fprintf(f,"%u\n",p->midi)>0;}return fclose(f)==0&&ok;}
static int dl_text_valid(const char*s){size_t n=0,col=0,lines=0;for(;*s;s++,n++){if(n>=DL_TEXT-1||((unsigned char)*s<32&&*s!='\n'&&*s!='\r'&&*s!='\t')||(unsigned char)*s>=127)return 0;if(*s=='\n'){col=0;if(++lines>128)return 0;}else if(*s!='\r'&&++col>255)return 0;}return lines<128||!col;}

/* Save-as creates CRLF files usable by DOS editors too. */
static int dl_dos_text(FILE*f,const char*s){if(!dl_text_valid(s))return 0;for(;*s;s++){if(*s=='\r')continue;if(*s=='\n'&&fputc('\r',f)==EOF)return 0;if(fputc(*s,f)==EOF)return 0;}return 1;}
static int dl_boot(FILE*f,const struct dl_profile*p,const char*command,int use_auto,int helper){const char*s=use_auto?(p->edited?p->autoexec:dl_default_autoexec):"@echo off\n";if(!dl_text_valid(s)||!dl_plain(command)||strlen(command)>127)return 0;if(!dl_dos_text(f,s)||fputs("\r\n",f)==EOF)return 0;if(*command&&fprintf(f,"%s%s\r\n",helper?"LOADFIX ":"",command)<0)return 0;return !ferror(f);}
#endif
