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
 /* The MUI line editor must retain the complete generated configuration. */
 {unsigned rows=0,width=0;const char*s=text;for(;*s;s++){if(*s=='\n'){rows++;width=0;}else{width++;assert(width<256);}}if(width)rows++;assert(rows<=128);}
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
 bad("[cpu]\ncore=typo\n");bad("[cpu]\ncore=auto\nCORE=normal\n");bad("[cpu]\ncycles=100001\n");bad("[cpu]\ncycles=499\n");bad("[cpu]\ncycles=-1\n");bad("[cpu]\ncycles=429496729600000\n");bad("[cpu]\ncputype=pentium4\n");bad("[dos]\nint33=false\n");bad("[sblaster]\nsbtype=unknown\n");bad("[dosbox]\nmemsize=3\n");bad("[cpu] junk\ncore=normal\n");bad("core=dynamic\n");
 parse("[terminal]\nforeground=black\nbackground=black\n");assert(!dc_apply(&c,&base,&p,error,sizeof(error)));
 parse("[render]\nscanlines=classic\n");assert(!dc_apply(&c,&base,&p,error,sizeof(error)));
 parse("[render]\nrenderer=opengl\n");base.system=2;strcpy(base.image,"HDD.IMG");assert(!dc_apply(&c,&base,&p,error,sizeof(error)));base.system=0;*base.image=0;
 parse("[startup]\nautoexec=file\nautoexec_file=SD032G:My Files/TEST.BAT\n[cdrom]\nsource=iso\npath=SD032G:My Files/CD.iso\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.edited==2&&!strcmp(p.auto_file,"SD032G:My Files/TEST.BAT"));
 parse("[dos]\nems=true\n[sblaster]\nsbtype=sbpro2\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.ems&&p.sound);assert(!strcmp(runtime(),"ZZDosBox-Compat"));
 assert(!dc_runtime(&c,&p,1,0,0,error,sizeof(error)));assert(!dc_runtime(&c,&p,0,1,0,error,sizeof(error)));assert(!dc_runtime(&c,&p,0,0,1,error,sizeof(error)));
 p.system=2;assert(!runtime());p.system=0;p.renderer=1;assert(!runtime());p.renderer=0;
 parse("[cpu]\ncore=normal\n");assert(!runtime());
 parse("[dos]\nems=false\n[sblaster]\nsbtype=none\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(!p.ems&&!p.sound);assert(!strcmp(runtime(),"ZZDosBox"));
 parse("[sblaster]\nsbtype=sb16\n[speaker]\npcspeaker=true\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.sound==2&&p.speaker==1);assert(!strcmp(runtime(),"ZZDosBox-Compat"));
 parse("[sblaster]\nsbtype=none\n[speaker]\npcspeaker=true\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(!p.sound&&p.speaker);assert(!strcmp(runtime(),"ZZDosBox-Compat"));
 parse("[cpu]\ncputype=pentium\ncycle_mode=max\n[dosbox]\nmemsize=32\n[dos]\nems=true\nvcpi=true\numb=true\n[joystick]\njoysticktype=2axis\n[sound]\nextra_device=gus\n");
 assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.cpu_model==2&&p.memory==32&&p.vcpi&&p.umb&&p.joystick&&p.extra_sound==1&&p.cycle_mode==2);assert(!strcmp(runtime(),"ZZDosBox-Expanded"));
 assert(strstr(dl_hardware_args(&p),"--cpu pentium"));p.ems=0;assert(!runtime());p.ems=1;p.extra_sound=2;p.sound=1;assert(!runtime());
 parse("[cpu]\ncycles=25000\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.memory==base.memory&&p.expanded);assert(!strcmp(runtime(),"ZZDosBox-Expanded"));assert(strstr(dl_compat_args(&p),"--ems off"));
 parse("[serial]\nserial1=modem\nhost=10.0.0.21\nport=2323\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.serial_mode==2&&p.serial_port==2323);assert(!strcmp(runtime(),"ZZDosBox-Expanded"));
 parse("[serial]\nserial1=device\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.serial_mode==3&&!strcmp(runtime(),"ZZDosBox-Expanded"));p.midi=1;assert(!runtime()&&strstr(error,"MIDI"));p.midi=0;
 /* Combined networking + JIT + EMS/PCM, preserving explicit normal core. */
 parse("[cpu]\ncore=dynamic\n[sblaster]\nsbtype=sb16\n[dos]\nems=true\n");
 assert(dc_apply(&c,&base,&p,error,sizeof(error)));p.network.enabled=1;
 assert(!strcmp(runtime(),"ZZDosBox-Net-JIT"));
 p.sound=1;assert(!strcmp(runtime(),"ZZDosBox-Net-JIT"));
 p.speaker=1;assert(!strcmp(runtime(),"ZZDosBox-Net-JIT"));
 p.network.protocol=1;strcpy(p.network.ipx_server,"10.0.0.21");assert(!strcmp(runtime(),"ZZDosBox-Net-JIT"));
 parse("[cpu]\ncore=normal\n");assert(!runtime());p.ems=p.sound=p.speaker=0;
 assert(!strcmp(runtime(),"ZZDosBox-Net"));
 parse("[cpu]\ncore=auto\n");assert(!strcmp(runtime(),"ZZDosBox-Net-JIT"));
 p.serial_mode=1;assert(!runtime());p.serial_mode=0;
 p.memory=16;assert(!runtime());p.memory=8;
 p.system=2;assert(!runtime());p.system=0;
 p.presentation=1;assert(!runtime());p.presentation=0;
 assert(!dc_runtime(&c,&p,0,0,1,error,sizeof(error)));
 bad("[speaker]\npcspeaker=maybe\n");
 bad("[dos]\nems=maybe\n");
 for(i=0;i<DC_COUNT;i++){const struct dc_key*k=&dc_keys[i];snprintf(text,sizeof(text),"[%s]\n%s=%s\n",k->section,k->name,k->example);parse(text);assert(c.set[i]);}
 for(i=0;i<2000;i++){unsigned j;for(j=0;j<sizeof(text)-1;j++)text[j]=(char)(32+(i*17+j*23)%96);text[sizeof(text)-1]=0;dc_parse(text,&c,error,sizeof(error));}
 parse("[cpu]\ncycles=max\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.cycle_mode==2&&dc_get(&c,DC_CYCLES,0)==3000);
 parse("[cpu]\ncycles=auto\n");assert(dc_apply(&c,&base,&p,error,sizeof(error)));assert(p.cycle_mode==1);
 bad("[cpu]\ncycles=max\ncycle_mode=auto\n");bad("[cpu]\ncycle_mode=auto\ncycles=max\n");
 unlink(session_path);puts("PASS dosbox.conf: template, all supported keys, CRLF/case, strict errors, transactional parsing, exact cycles/session, profile isolation, core routing and incompatible combinations");return 0;
}
