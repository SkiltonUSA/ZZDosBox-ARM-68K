/* GPL-2.0-or-later. Opt-in DOSBox diagnostic bridge, no GPU or host I/O ports. */
#include <stdio.h>
#include <string.h>
#include "dosbox.h"
#include "regs.h"
#include "mem.h"
#include "paging.h"
#include "guest_memory.h"
#include "inout.h"
#include "pic.h"
#include "guest.h"
#include "guest_calls.h"
#ifdef DB_GLIDE_SURVEY
#ifndef DB_NATIVE
#error The virtual board survey must never run on hardware
#endif
#include "survey.h"
#include "survey_capture.h"
static grx_survey survey;

#endif
#if defined(DB_GLIDE_SURVEY)||defined(DB_GLIDE_LIVE)
static MemHandle survey_memory;
#endif
#ifdef DB_GLIDE_LIVE
extern void db_glide_live_open(uint32_t),db_glide_live_close();
extern uint32_t db_glide_live_call(const grx_guest_call *,const grx_guest_io *);
#endif
static uint32_t reply;
static unsigned counts[GRX_CALL_COUNT];
static int read_bytes(void *,uint32_t a,void *out,unsigned n){
 return grx_guest_read_checked(a,out,n);
}
static int write_bytes(void *,uint32_t a,const void *in,unsigned n){
 return grx_guest_write_checked(a,in,n);
}
static Bitu read_port(Bitu port,Bitu){return port==GRX_GATE_PORT?GRX_GATE_MAGIC:reply;}
static void write_port(Bitu,Bitu id,Bitu){
 grx_guest_call call;grx_guest_io io={read_bytes,write_bytes,nullptr,(uint32_t)SegPhys(ds)};reply=0;
 uint64_t linear=(uint64_t)SegPhys(ss)+reg_ecx;
 if(linear>UINT32_MAX||!grx_guest_decode(&call,id,(uint32_t)linear,&io)){
  #ifdef DB_NATIVE
  fprintf(stderr,"GLIDE invalid guest call id=%u stack=%08x:%08x\n",(unsigned)id,(unsigned)SegPhys(ss),(unsigned)reg_ecx);
  #else
  E_Exit("Invalid Glide guest call %u",(unsigned)id);
  #endif
  return;
 }
 counts[id]++;
#ifdef DB_NATIVE
 if(counts[id]<=4){
  fprintf(stderr,"GLIDE tick=%u call=%s args=",(unsigned)PIC_Ticks,grx_calls[id].name);
  for(unsigned i=0;i<call.count;i++)fprintf(stderr,"%s%08x",i?",":"",call.args[i]);
  fputc('\n',stderr);
 }
#endif
#ifdef DB_GLIDE_SURVEY
 reply=grx_survey_call(&survey,&call,&io);
 grx_survey_capture_call(&call,reply,&io,&survey);
 if(counts[id]<=4)fprintf(stderr,"SURVEY result=%08x ds=%08x lfb=%08x open=%u\n",reply,io.data_base,survey.lfb_address,survey.open);
#elif defined(DB_GLIDE_LIVE)
 reply=db_glide_live_call(&call,&io);
#else
 reply=grx_guest_diagnostic(&call,&io);
#endif
}
void db_glide_diagnostic_open(){
 memset(counts,0,sizeof(counts));reply=0;
#ifdef DB_GLIDE_SURVEY
 memset(&survey,0,sizeof(survey));
 survey_memory=0;grx_survey_capture_open();
 fputs("GLIDE SURVEY: virtual board replies only; no pixels, rendering or FPS.\n",stderr);
#endif
 IO_RegisterReadHandler(GRX_GATE_PORT,read_port,IO_MD);
 IO_RegisterReadHandler(GRX_COMMAND_PORT,read_port,IO_MD);
 IO_RegisterWriteHandler(GRX_COMMAND_PORT,write_port,IO_MD);
}
#if defined(DB_GLIDE_SURVEY)||defined(DB_GLIDE_LIVE)
/* Called after DOS initializes its extended-memory allocator, before EXEC. */
void db_glide_survey_memory_open(){
 survey_memory=MEM_AllocatePages(150,true);
 if(survey_memory<=0)E_Exit("Glide survey framebuffer allocation failed");
#ifdef DB_GLIDE_LIVE
 db_glide_live_open((uint32_t)survey_memory*4096);
#else
 survey.lfb_address=(uint32_t)survey_memory*4096;
#endif
}
#endif
void db_glide_diagnostic_close(){
#ifdef DB_GLIDE_SURVEY
 grx_survey_capture_close();
#endif
#ifdef DB_GLIDE_LIVE
 db_glide_live_close();
#endif
#if defined(DB_GLIDE_SURVEY)||defined(DB_GLIDE_LIVE)
 if(survey_memory>0)MEM_ReleasePages(survey_memory);
 survey_memory=0;
#endif
#ifdef DB_NATIVE
 for(unsigned i=1;i<GRX_CALL_COUNT;i++)if(counts[i])fprintf(stderr,"GLIDE total %s=%u\n",grx_calls[i].name,counts[i]);
#endif
 IO_FreeReadHandler(GRX_GATE_PORT,IO_MD);IO_FreeReadHandler(GRX_COMMAND_PORT,IO_MD);
 IO_FreeWriteHandler(GRX_COMMAND_PORT,IO_MD);
}
