/* GPL-3.0-or-later. Shared, bounded launcher/host configuration. */
#ifndef DB_SESSION_H
#define DB_SESSION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DL_PATH 512
#define DL_TEXT 4096
struct dl_session {char drive[DL_PATH],command[160];unsigned memory,machine,cycles,fg,bg,live,terminal;};
static inline void dl_defaults(struct dl_session *s){memset(s,0,sizeof(*s));s->memory=2;s->cycles=3000;s->fg=7;s->live=1;}
static inline int dl_plain(const char *s){for(;*s;s++)if((unsigned char)*s<32||*s==127)return 0;return 1;}
static inline int dl_valid(const struct dl_session*s){return *s->drive&&dl_plain(s->drive)&&dl_plain(s->command)&&(s->memory==1||s->memory==2||s->memory==4||s->memory==8)&&s->machine<=2&&s->cycles>=500&&s->cycles<=10000&&s->fg<8&&s->bg<8&&s->fg!=s->bg&&s->live<=1&&s->terminal<=1;}
static inline int dl_session_write(const char *p,const struct dl_session*s){FILE*f;int ok;if(!dl_valid(s)||(f=fopen(p,"w"))==NULL)return 0;ok=fprintf(f,"ZZDOSSESSION1\n%s\n%s\n%u %u %u %u %u %u %u\n",s->drive,s->command,s->memory,s->machine,s->cycles,s->fg,s->bg,s->live,s->terminal)>0;return fclose(f)==0&&ok;}
static inline int dl_line(FILE*f,char*p,size_t n){size_t k;if(!fgets(p,n,f))return 0;k=strlen(p);if(!k||p[k-1]!='\n')return 0;p[--k]=0;if(k&&p[k-1]=='\r')p[k-1]=0;return dl_plain(p);}
static inline int dl_session_read(const char *p,struct dl_session*s){FILE*f=fopen(p,"r");char line[160],tail;int ok=0;if(!f)return 0;dl_defaults(s);if(dl_line(f,line,sizeof(line))&&!strcmp(line,"ZZDOSSESSION1")&&dl_line(f,s->drive,sizeof(s->drive))&&dl_line(f,s->command,sizeof(s->command))&&dl_line(f,line,sizeof(line))&&sscanf(line,"%u %u %u %u %u %u %u %c",&s->memory,&s->machine,&s->cycles,&s->fg,&s->bg,&s->live,&s->terminal,&tail)==7&&fgetc(f)==EOF&&dl_valid(s))ok=1;fclose(f);return ok;}
/* Only the session path enters AmigaDOS; all DOS commands stay inside it. */
static inline int dl_quote(char*out,size_t cap,const char*in){size_t n=0;if(cap<3)return 0;out[n++]='"';for(;*in;in++){if((unsigned char)*in<32)return 0;if(*in=='*'||*in=='"'){if(n+3>=cap)return 0;out[n++]='*';}if(n+2>=cap)return 0;out[n++]=*in;}out[n++]='"';out[n]=0;return 1;}
#endif
