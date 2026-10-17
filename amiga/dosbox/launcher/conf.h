/* GPL-3.0-or-later. Strict launcher-side dosbox.conf overrides.
 * This port does not consume arbitrary upstream DOSBox configuration. */
#ifndef DL_CONF_H
#define DL_CONF_H
#include "model.h"
#include "developer.h"
#include <stddef.h>
#include <ctype.h>
enum {DC_CORE,DC_CYCLES,DC_MEMORY,DC_MACHINE,DC_FULLSCREEN,DC_SIZE,DC_FPS,
 DC_RENDERER,DC_PRESENTATION,DC_SCANLINES,DC_MONO,DC_MIDI,DC_PORT,DC_CD,DC_CDPATH,
 DC_LOADFIX,DC_FG,DC_BG,DC_LIVE,DC_AUTO_MODE,DC_AUTO_FILE,DC_CONFIG_MODE,DC_CONFIG_FILE,
 DC_TRACE,DC_PROFILE,DC_BREAK,DC_FRAME,DC_HANDOFF,DC_POLL,DC_EMS,DC_SOUND,DC_SPEAKER,DC_CPUTYPE,DC_VCPI,DC_UMB,DC_JOYSTICK,DC_EXTRA_SOUND,DC_CYCLE_MODE,DC_SERIAL,DC_SERIAL_HOST,DC_SERIAL_PORT,DC_PAGED_JIT,DC_COUNT};
struct dc_key {const char *section,*name,*choices,*example;int offset;unsigned cap,min,max;};
#define DC_U(section,name,choices,example,field) {section,name,choices,example,offsetof(struct dl_profile,field),0,0,0}
#define DC_S(section,name,example,field) {section,name,NULL,example,offsetof(struct dl_profile,field),sizeof(((struct dl_profile*)0)->field),0,0}
static const struct dc_key dc_keys[DC_COUNT]={
 {"cpu","core","auto|normal|dynamic","auto",-1,0,0,0},
 {"cpu","cycles",NULL,"3000",-1,0,500,100000},
 {"dosbox","memsize",NULL,"8",offsetof(struct dl_profile,memory),0,1,32},
 DC_U("dosbox","machine","vga|ega|cga|svga_s3|tandy|pcjr|hercules","vga",machine),
 DC_U("sdl","fullscreen","false|true","false",display),
 DC_U("sdl","windowresolution","320x240|640x480|960x720","640x480",size),
 DC_U("render","show_fps","false|true","false",fps),
 DC_U("render","renderer","direct|opengl","direct",renderer),
 DC_U("render","presentation","normal|adaptive","normal",presentation),
 DC_U("render","scanlines","off|classic|soft|gradient","off",scanlines),
 DC_U("render","monochrome","white|amber|green|paperwhite","white",monochrome),
 DC_U("midi","mpu401","none|intelligent|uart","none",midi),
 DC_S("midi","midiconfig","out.0",midi_port),
 DC_U("cdrom","source","none|iso|device","none",cd_mode),
 DC_S("cdrom","path","UMSD:",cd_path),
 DC_U("dos","loadfix","false|true","false",helper),
 DC_U("terminal","foreground","black|red|green|yellow|blue|magenta|cyan|white","white",fg),
 DC_U("terminal","background","black|red|green|yellow|blue|magenta|cyan|white","black",bg),
 DC_U("terminal","live","false|true","true",live),
 DC_U("startup","autoexec","default|edited|file","default",edited),
 DC_S("startup","autoexec_file","RAM:MYAUTO.BAT",auto_file),
 DC_U("startup","configsys","default|edited|file","default",config_mode),
 DC_S("startup","configsys_file","RAM:MYCONFIG.SYS",config_file),
 {"developer","trace","false|true","false",-1,0,0,0},
 {"developer","profile","false|true","false",-1,0,0,0},
 {"developer","breakpoint","off|before|after","off",-1,0,0,0},
 {"developer","frame",NULL,"1",-1,0,1,10000000},
 {"developer","handoff","default|serial|pipelined","default",-1,0,0,0},
 {"developer","poll_ms","default|1|4|20","default",-1,0,0,0},
 DC_U("dos","ems","false|true","true",ems),
 DC_U("sblaster","sbtype","none|sbpro2|sb16","sbpro2",sound),
 DC_U("speaker","pcspeaker","false|true","false",speaker),
 DC_U("cpu","cputype","386|486|pentium","386",cpu_model),
 DC_U("dos","vcpi","false|true","false",vcpi),
 DC_U("dos","umb","false|true","false",umb),
 DC_U("joystick","joysticktype","none|2axis","none",joystick),
 DC_U("sound","extra_device","none|gus|cms|tandy","none",extra_sound),
 DC_U("cpu","cycle_mode","fixed|auto|max","fixed",cycle_mode),
 DC_U("serial","serial1","disabled|tcp|modem|device","disabled",serial_mode),
 DC_S("serial","host","127.0.0.1",serial_host),
 {"serial","port",NULL,"23",offsetof(struct dl_profile,serial_port),0,1,65535},
 DC_U("cpu","paged_register_jit","false|true","false",paged_jit)
};
#undef DC_U
#undef DC_S
struct dc_config {unsigned value[DC_COUNT];unsigned char set[DC_COUNT];char text[DC_COUNT][256];};
static int dc_equal(const char*a,const char*b){while(*a&&*b){if(tolower((unsigned char)*a++)!=tolower((unsigned char)*b++))return 0;}return !*a&&!*b;}
static char *dc_trim(char*s){char*e;while(*s==' '||*s=='\t')s++;e=s+strlen(s);while(e>s&&(e[-1]==' '||e[-1]=='\t'||e[-1]=='\r'))*--e=0;return s;}
static int dc_error(char*out,size_t cap,unsigned line,const char*reason){snprintf(out,cap,"dosbox.conf line %u: %s",line,reason);return 0;}
static int dc_choice(const char*list,const char*value,unsigned*out){unsigned n=0;char item[32];while(*list){unsigned i=0;while(*list&&*list!='|'){if(i+1>=sizeof(item))return 0;item[i++]=*list++;}item[i]=0;if(dc_equal(item,value)){*out=n;return 1;}if(*list)list++;n++;}return 0;}
static int dc_number(const char*s,unsigned min,unsigned max,unsigned*out){unsigned n=0;if(!*s)return 0;while(*s){if(*s<'0'||*s>'9'||n>max/10)return 0;n=n*10+(*s++-'0');if(n>max)return 0;}if(n<min)return 0;*out=n;return 1;}
static int dc_parse(const char*text,struct dc_config*out,char*error,size_t cap){
 struct dc_config next;char line[256],section[32]="";unsigned lineno=0;
 memset(&next,0,sizeof(next));if(!dl_text_valid(text))return dc_error(error,cap,0,"file too large or not plain text");
 while(*text){char *s,*eq,*value;unsigned n=0,k;lineno++;
  while(*text&&*text!='\n'){if(n+1>=sizeof(line))return dc_error(error,cap,lineno,"line too long");line[n++]=*text++;}if(*text)text++;line[n]=0;s=dc_trim(line);
  if(!*s||*s=='#'||*s==';')continue;
  if(*s=='['){char*end=strchr(s,']');int known=0;if(!end||*dc_trim(end+1))return dc_error(error,cap,lineno,"invalid section");*end=0;s=dc_trim(s+1);if(strlen(s)>=sizeof(section))return dc_error(error,cap,lineno,"section too long");
   for(k=0;k<DC_COUNT;k++)if(dc_equal(s,dc_keys[k].section))known=1;
   if(!known)return dc_error(error,cap,lineno,"unsupported section");strcpy(section,s);continue;
  }
  eq=strchr(s,'=');if(!eq)return dc_error(error,cap,lineno,"expected key=value");*eq=0;s=dc_trim(s);value=dc_trim(eq+1);
  for(k=0;k<DC_COUNT;k++)if(dc_equal(section,dc_keys[k].section)&&dc_equal(s,dc_keys[k].name))break;
  if(k==DC_COUNT)return dc_error(error,cap,lineno,"unsupported setting");if(next.set[k])return dc_error(error,cap,lineno,"duplicate setting");
  if(k==DC_CYCLES&&(dc_equal(value,"auto")||dc_equal(value,"max"))){
   if(next.set[DC_CYCLE_MODE])return dc_error(error,cap,lineno,"cycle mode specified twice");
   next.set[DC_CYCLE_MODE]=1;next.value[DC_CYCLE_MODE]=dc_equal(value,"max")?2:1;next.value[k]=3000;
  }
  else if(dc_keys[k].cap){if(strlen(value)>=dc_keys[k].cap||strlen(value)>=sizeof(next.text[k]))return dc_error(error,cap,lineno,"value too long");strcpy(next.text[k],value);}
  else if(dc_keys[k].choices){if(!dc_choice(dc_keys[k].choices,value,&next.value[k]))return dc_error(error,cap,lineno,"unsupported value");}
  else {if(k==DC_CYCLES&&!strncmp(value,"fixed ",6))value=dc_trim(value+6);if(!dc_number(value,dc_keys[k].min,dc_keys[k].max,&next.value[k])||(k==DC_MEMORY&&(next.value[k]&(next.value[k]-1))))return dc_error(error,cap,lineno,"number outside supported range");}
  next.set[k]=1;
 }
 *out=next;*error=0;return 1;
}
static int dc_template(char*out,size_t cap){unsigned k;size_t used=0;const char*section="";int n;
 n=snprintf(out,cap,"# ZZDosBox overrides; commented settings keep program preferences.\n# Save settings applies this file. core=dynamic uses guarded JIT for images.\n# Paging defaults to the interpreter; register-only JIT is experimental.\n# x87 is in the Compat runtime. Auto/max use wall-time feedback; int33 is not configurable.\n");if(n<0||(size_t)n>=cap)return 0;used=n;
 for(k=0;k<DC_COUNT;k++){const struct dc_key*d=&dc_keys[k];if(strcmp(section,d->section)){n=snprintf(out+used,cap-used,"\n[%s]\n",d->section);if(n<0||(size_t)n>=cap-used)return 0;used+=n;section=d->section;}
  if(d->choices)n=snprintf(out+used,cap-used,"# %s\n",d->choices);else if(d->cap)n=snprintf(out+used,cap-used,"# Amiga path / CAMD port\n");else n=snprintf(out+used,cap-used,"# %u..%u%s\n",d->min,d->max,k==DC_MEMORY?" (1, 2, 4, 8, 16, 32 only)":"");
  if(n<0||(size_t)n>=cap-used)return 0;used+=n;
  n=snprintf(out+used,cap-used,"%s%s=%s\n",k==DC_CORE?"":"# ",d->name,d->example);if(n<0||(size_t)n>=cap-used)return 0;used+=n;
 }return dl_text_valid(out);
}
static unsigned dc_get(const struct dc_config*c,unsigned k,unsigned fallback){return c->set[k]?c->value[k]:fallback;}
static int dc_apply(const struct dc_config*c,const struct dl_profile*base,struct dl_profile*out,char*error,size_t cap){unsigned k;*out=*base;
 for(k=0;k<DC_COUNT;k++)if(c->set[k]&&dc_keys[k].offset>=0){char*at=(char*)out+dc_keys[k].offset;if(dc_keys[k].cap)strcpy(at,c->text[k]);else *(unsigned*)at=c->value[k];}
 if(c->set[DC_CYCLES]&&c->value[DC_CYCLES]>10000)out->expanded=1;
 if(!dl_profile_valid(out)){snprintf(error,cap,"dosbox.conf conflicts with program settings (colours, paths or startup files).");return 0;}
 if(out->scanlines&&!dl_scanlines_available(out->display,out->fps)&&(c->set[DC_SCANLINES]||c->set[DC_FULLSCREEN]||c->set[DC_FPS])){snprintf(error,cap,"Scanlines require fullscreen and FPS off.");return 0;}
 if((out->system==1||out->system==2)&&(c->set[DC_FG]||c->set[DC_BG]||(c->set[DC_LIVE]&&!out->live))){snprintf(error,cap,"Booted DOS controls its own colours and runs until closed.");return 0;}
 if(out->system==2){
  if((c->set[DC_RENDERER]&&out->renderer)||(c->set[DC_PRESENTATION]&&out->presentation)||(c->set[DC_SCANLINES]&&out->scanlines)||c->set[DC_AUTO_MODE]||c->set[DC_AUTO_FILE]||c->set[DC_CONFIG_MODE]||c->set[DC_CONFIG_FILE]||c->set[DC_FG]||c->set[DC_BG]||(c->set[DC_LIVE]&&!out->live)){
   snprintf(error,cap,"Image sessions use disk startup files, guest colours and Direct presentation.");return 0;
  }
  out->renderer=out->presentation=out->scanlines=0;
 }
 return 1;
}
/* Existing binaries own the CPU core. Never silently ignore an explicit core. */
static const char *dc_runtime(const struct dc_config*c,const struct dl_profile*p,unsigned trace,unsigned point,unsigned memory,char*error,size_t cap){
 unsigned core=dc_get(c,DC_CORE,0);
 if(p->network.enabled){
  if(p->system||p->renderer||p->presentation||p->scanlines||trace||point||memory||dl_profile_expanded(p)||(c->set[DC_CYCLES]&&c->value[DC_CYCLES]>10000)){
   snprintf(error,cap,"Network requires embedded DOS, up to 8 MiB, Direct/Normal and no Expanded or developer options.");return NULL;
  }
  if(core==1){
   if(p->ems||p->sound||p->speaker){snprintf(error,cap,"Network audio/EMS require core auto/dynamic.");return NULL;}
   return "ZZDosBox-Net";
  }
  return "ZZDosBox-Net-JIT";
 }

 if(dl_profile_expanded(p)||(c->set[DC_CYCLES]&&c->value[DC_CYCLES]>10000)){
  if(p->renderer||core==1||trace||point||memory){snprintf(error,cap,"Expanded hardware requires Direct, core auto/dynamic, developer tools off.");return NULL;}
  if(p->serial_mode&&(p->system==1||p->system==2)){snprintf(error,cap,"COM1 currently requires embedded DOS.");return NULL;}
  if(p->serial_mode==3&&p->midi&&!strcmp(p->serial.device,"serial.device")){snprintf(error,cap,"Disable CAMD MIDI when using the built-in serial port.");return NULL;}
  if(p->vcpi&&!p->ems){snprintf(error,cap,"VCPI requires EMS enabled.");return NULL;}
  if((p->system==1||p->system==2)&&(p->ems||p->vcpi||p->umb)){snprintf(error,cap,"Booted DOS supplies its own EMS/VCPI/UMB drivers.");return NULL;}
  if(p->extra_sound>=2&&p->sound){snprintf(error,cap,"Turn Sound Blaster off for Game Blaster or Tandy (shared I/O ports).");return NULL;}
  if(p->extra_sound==1&&p->machine!=0&&p->machine!=1&&p->machine!=3){snprintf(error,cap,"GUS requires EGA, VGA or S3 graphics.");return NULL;}
  return "ZZDosBox-Expanded";
 }
 if(p->ems||p->sound||p->speaker){
  if((p->system&&p->system!=3)||p->renderer||core==1||trace||point||memory){snprintf(error,cap,"EMS/Sound Blaster require embedded DOS, Direct, core auto/dynamic, developer tools off.");return NULL;}
  return "ZZDosBox-Compat";
 }
 if(memory&&(p->renderer||trace||point)){snprintf(error,cap,"Memory tools require Direct with trace and frame breakpoints off.");return NULL;}
 if(p->system==2){if(trace||point||memory){snprintf(error,cap,"Image sessions do not support trace, breakpoints or memory tools.");return NULL;}return core==2?"ZZDosBox-Windows-Dynamic":"ZZDosBox-Windows";}
 if(core==1){if(p->renderer||p->presentation||p->scanlines||trace||point||memory){snprintf(error,cap,"core=normal requires Direct/Normal, scanlines off and no trace or memory tools.");return NULL;}return "ZZDosBox-Windows";}
 if(p->system==1&&core==2){if(p->renderer||p->presentation||p->scanlines||trace||point||memory){snprintf(error,cap,"Guarded boot JIT requires Direct/Normal with no scanlines, trace or memory tools.");return NULL;}return "ZZDosBox-Windows-Dynamic";}
 if(memory)return "ZZDosBox-Memory";
 return dl_dev_runtime(dl_profile_player(p),trace,point);
}
#endif
