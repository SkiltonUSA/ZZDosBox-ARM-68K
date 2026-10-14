/* GPL-3.0-or-later. Session-only developer options; never saved in game profiles. */
#pragma once
#include <stdio.h>
#include <string.h>
static inline int dl_dev_frame(const char *text,unsigned *frame){
 unsigned n=0;if(!text||!*text)return 0;
 for(;*text;text++){
  if(*text<'0'||*text>'9'||n>1000000u)return 0;
  n=n*10u+(unsigned)(*text-'0');
  if(n>10000000u)return 0;
 }
 if(!n)return 0;
 *frame=n;return 1;
}
static inline int dl_dev_args(char *out,size_t size,unsigned trace,unsigned point,const char *text){
 unsigned frame=0;int n;
 if(trace>1||point>2||(point&&!dl_dev_frame(text,&frame)))return 0;
 if(point)n=snprintf(out,size,"%s --break-%s %u",trace?" --trace-video":"",point==1?"before":"after",frame);
 else n=snprintf(out,size,"%s",trace?" --trace-video":"");
 return n>=0&&(size_t)n<size;
}
static inline const char *dl_dev_runtime(const char *normal,unsigned trace,unsigned point){
 if(!trace&&!point)return normal;
 return !strcmp(normal,"ZZDosBox-OpenGL")?"ZZDosBox-OpenGL-Trace":"ZZDosBox-Trace";
}
static inline int dl_dev_tools(char *out,size_t size,unsigned trace,unsigned point,
 const char *frame,unsigned profile,unsigned handoff,unsigned poll){
 char base[96];int n;
 static const char *flow[]={""," --serial-video"," --pipeline-video"};
 static const char *wait[]={""," --poll-ms 1"," --poll-ms 4"," --poll-ms 20"};
 if(profile>1||handoff>2||poll>3||!dl_dev_args(base,sizeof(base),trace,point,frame))return 0;
 n=snprintf(out,size,"%s%s%s%s",base,profile?" --profile":"",flow[handoff],wait[poll]);
 return n>=0&&(size_t)n<size;
}
