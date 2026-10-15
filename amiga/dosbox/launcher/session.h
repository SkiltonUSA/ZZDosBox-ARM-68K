#include "../pc/adapters.h"
/* GPL-3.0-or-later. Shared, bounded launcher/host configuration. */
#ifndef DB_SESSION_H
#define DB_SESSION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DL_PATH 512
#define DL_TEXT 4096
struct dl_session {char drive[DL_PATH],command[160],cd_path[DL_PATH],midi_port[64],image[13];unsigned memory,machine,cycles,fg,bg,live,terminal,display,fps,system,size,cd_mode,midi;};
static inline void dl_defaults(struct dl_session *s){memset(s,0,sizeof(*s));s->memory=2;s->cycles=3000;s->fg=7;s->live=1;s->size=1;strcpy(s->midi_port,"out.0");}
static inline int dl_plain(const char *s){for(;*s;s++)if((unsigned char)*s<32||*s==127)return 0;return 1;}
static inline int dl_image_valid(const char*s){unsigned n=0,dots=0,dot=0;if(!*s)return 0;for(;*s;s++,n++){if(*s=='.'){if(!n||n>8||dots++)return 0;dot=n;}else if(!((*s>='A'&&*s<='Z')||(*s>='0'&&*s<='9')||*s=='_'||*s=='-'))return 0;if(n>=12)return 0;}return dots==1&&n>dot+1&&n-dot<=4;}
static inline int dl_valid(const struct dl_session*s){return *s->drive&&dl_plain(s->drive)&&dl_plain(s->command)&&(s->memory==1||s->memory==2||s->memory==4||s->memory==8)&&s->machine<PC_ADAPTER_COUNT&&s->cycles>=500&&s->cycles<=10000&&s->fg<8&&s->bg<8&&s->fg!=s->bg&&s->live<=1&&s->terminal<=1&&s->display<=1&&s->fps<=1&&s->system<=2&&((s->system==2)?dl_image_valid(s->image):!*s->image)&&s->size<3&&s->midi<=2&&dl_plain(s->midi_port)&&(!s->midi||*s->midi_port)&&s->cd_mode<=2&&dl_plain(s->cd_path)&&(!s->cd_mode||*s->cd_path);}
static inline int dl_session_write(const char *p,const struct dl_session*s){FILE*f;int ok;if(!dl_valid(s)||(f=fopen(p,"w"))==NULL)return 0;ok=fprintf(f,"ZZDOSSESSION%u\n%s\n%s\n%u %u %u %u %u %u %u %u %u %u %u\n",s->system==2?6:5,s->drive,s->command,s->memory,s->machine,s->cycles,s->fg,s->bg,s->live,s->terminal,s->display,s->fps,s->system,s->size)>0;if(ok)ok=fprintf(f,"%u\n%s\n",s->cd_mode,s->cd_path)>0;if(ok)ok=fprintf(f,"%u\n%s\n",s->midi,s->midi_port)>0;if(ok&&s->system==2)ok=fprintf(f,"%s\n",s->image)>0;return fclose(f)==0&&ok;}
static inline int dl_line(FILE*f,char*p,size_t n){size_t k;if(!fgets(p,n,f))return 0;k=strlen(p);if(!k||p[k-1]!='\n')return 0;p[--k]=0;if(k&&p[k-1]=='\r')p[k-1]=0;return dl_plain(p);}
static inline int dl_session_read(const char *p,struct dl_session*s){
 FILE*f=fopen(p,"r");char line[160],tail;int ok=0,v;if(!f)return 0;dl_defaults(s);
 if(!dl_line(f,line,sizeof(line)))goto done;
 v=!strcmp(line,"ZZDOSSESSION6")?6:!strcmp(line,"ZZDOSSESSION5")?5:!strcmp(line,"ZZDOSSESSION4")?4:!strcmp(line,"ZZDOSSESSION3")?3:!strcmp(line,"ZZDOSSESSION2")?2:!strcmp(line,"ZZDOSSESSION1")?1:0;
 if(!v||!dl_line(f,s->drive,sizeof(s->drive))||!dl_line(f,s->command,sizeof(s->command))||!dl_line(f,line,sizeof(line)))goto done;
 if(v==1){s->display=1;ok=sscanf(line,"%u %u %u %u %u %u %u %c",&s->memory,&s->machine,&s->cycles,&s->fg,&s->bg,&s->live,&s->terminal,&tail)==7;}
 else if(v==2)ok=sscanf(line,"%u %u %u %u %u %u %u %u %u %u %c",&s->memory,&s->machine,&s->cycles,&s->fg,&s->bg,&s->live,&s->terminal,&s->display,&s->fps,&s->system,&tail)==10;
 else ok=sscanf(line,"%u %u %u %u %u %u %u %u %u %u %u %c",&s->memory,&s->machine,&s->cycles,&s->fg,&s->bg,&s->live,&s->terminal,&s->display,&s->fps,&s->system,&s->size,&tail)==11;
 if(ok&&v>=4)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&s->cd_mode,&tail)==1&&dl_line(f,s->cd_path,sizeof(s->cd_path));
 if(ok&&v>=5)ok=dl_line(f,line,sizeof(line))&&sscanf(line,"%u %c",&s->midi,&tail)==1&&dl_line(f,s->midi_port,sizeof(s->midi_port));
 if(ok&&v>=6){ok=dl_line(f,line,sizeof(line))&&strlen(line)<sizeof(s->image);if(ok)strcpy(s->image,line);}
 ok=ok&&fgetc(f)==EOF&&dl_valid(s);
done:fclose(f);return ok;
}
/* Only the session path enters AmigaDOS; all DOS commands stay inside it. */
static inline int dl_quote(char*out,size_t cap,const char*in){size_t n=0;if(cap<3)return 0;out[n++]='"';for(;*in;in++){if((unsigned char)*in<32)return 0;if(*in=='*'||*in=='"'){if(n+3>=cap)return 0;out[n++]='*';}if(n+2>=cap)return 0;out[n++]=*in;}out[n++]='"';out[n]=0;return 1;}
#endif
