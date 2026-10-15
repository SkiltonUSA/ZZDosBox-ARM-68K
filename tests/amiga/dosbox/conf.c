/* Exercise real configuration parsing, precedence and executable selection. */
#include "../../../amiga/dosbox/launcher/conf.h"
#include <assert.h>
#include <unistd.h>
static struct dc_config c,old;
static char text[DL_TEXT],error[256];
static struct dl_profile base,p;
static void parse(const char*s){assert(dc_parse(s,&c,error,sizeof(error)));}
static void bad(const char*s){old=c;assert(!dc_parse(s,&c,error,sizeof(error)));assert(!memcmp(&old,&c,sizeof(c)));assert(strstr(error,"line"));}
static const char*runtime(void){return dc_runtime(&c,&p,0,0,0,error,sizeof(error));}
int main(int argc,char**argv){unsigned i;struct dl_session session,loaded;char session_path[]="/tmp/zzconf-XXXXXX";int fd=mkstemp(session_path);assert(fd>=0);close(fd);
 assert(dc_template(text,sizeof(text)));assert(dc_parse(text,&c,error,sizeof(error)));
 if(argc>1){FILE*f=fopen(argv[1],"wb");assert(f);assert(fputs(text,f)!=EOF);assert(!fclose(f));}
 dl_profile_default(&base);strcpy(base.title,"Test");strcpy(base.drive,"RAM:Test");
 assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(!memcmp(&base,&p,sizeof(base)));assert(!strcmp(runtime(),"ZZDosBox"));
 parse("[CPU]\r\n Core = DyNaMiC\r\ncycles = fixed 7500\r\n[dosbox]\nmemsize=8\nmachine=svga_s3\n[sdl]\nfullscreen=true\nwindowresolution=960x720\n[render]\nshow_fps=false\npresentation=adaptive\nscanlines=soft\n[midi]\nmpu401=intelligent\nmidiconfig=out.0\n");
 assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.memory==8&&p.machine==3&&p.display==1&&p.size==2&&p.scanlines==2&&p.midi==1&&p.presentation==1);
 assert(!strcmp(runtime(),"ZZDosBox-Adaptive"));assert(base.memory==2&&base.display==0);
 dl_defaults(&session);strcpy(session.drive,p.drive);session.cycles=dc_get(&c,DC_CYCLES,3000);session.memory=p.memory;assert(dl_session_write(session_path,&session));assert(dl_session_read(session_path,&loaded));assert(loaded.cycles==7500&&loaded.memory==8);
 parse("[cpu]\ncore=normal\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(!strcmp(runtime(),"ZZDosBox-Windows"));
 p.renderer=1;assert(!runtime());p.renderer=0;p.system=2;strcpy(p.image,"HDD.IMG");assert(!strcmp(runtime(),"ZZDosBox-Windows"));
 parse("[cpu]\ncore=dynamic\n");assert(!strcmp(runtime(),"ZZDosBox-Windows-Dynamic"));
 assert(!dc_runtime(&c,&p,1,0,0,error,sizeof(error)));assert(!dc_runtime(&c,&p,0,0,1,error,sizeof(error)));
 p.system=1;assert(!strcmp(runtime(),"ZZDosBox-Windows-Dynamic"));
 parse("[cpu]\ncore=auto\n");p.system=2;assert(!strcmp(runtime(),"ZZDosBox-Windows"));
 bad("[cpu]\ncore=typo\n");bad("[cpu]\ncore=auto\nCORE=normal\n");bad("[cpu]\ncycles=max\n");bad("[cpu]\ncycles=10001\n");bad("[cpu]\ncycles=499\n");bad("[cpu]\ncycles=-1\n");bad("[cpu]\ncycles=429496729600000\n");bad("[cpu]\ncputype=pentium\n");bad("[dos]\nint33=false\n");bad("[sblaster]\nsbtype=sb16\n");bad("[dosbox]\nmemsize=3\n");bad("[cpu] junk\ncore=normal\n");bad("core=dynamic\n");
 parse("[terminal]\nforeground=black\nbackground=black\n");assert(!dc_apply(&c,&base,&p,error,sizeof(error)));
 parse("[render]\nscanlines=classic\n");assert(!dc_apply(&c,&base,&p,error,sizeof(error)));
 parse("[render]\nrenderer=opengl\n");base.system=2;strcpy(base.image,"HDD.IMG");assert(!dc_apply(&c,&base,&p,error,sizeof(error)));base.system=0;*base.image=0;
 parse("[startup]\nautoexec=file\nautoexec_file=SD032G:My Files/TEST.BAT\n[cdrom]\nsource=iso\npath=SD032G:My Files/CD.iso\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.edited==2&&!strcmp(p.auto_file,"SD032G:My Files/TEST.BAT"));
 for(i=0;i<DC_COUNT;i++){const struct dc_key*k=&dc_keys[i];snprintf(text,sizeof(text),"[%s]\n%s=%s\n",k->section,k->name,k->example);parse(text);assert(c.set[i]);}
 for(i=0;i<2000;i++){unsigned j;for(j=0;j<sizeof(text)-1;j++)text[j]=(char)(32+(i*17+j*23)%96);text[sizeof(text)-1]=0;dc_parse(text,&c,error,sizeof(error));}
 unlink(session_path);puts("PASS dosbox.conf: template, all 29 keys, CRLF/case, strict errors, transactional parsing, exact cycles/session, profile isolation, core routing and incompatible combinations");return 0;
}
