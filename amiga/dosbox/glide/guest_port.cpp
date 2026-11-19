/* GPL-2.0-or-later. Opt-in DOSBox diagnostic bridge, no GPU or host I/O ports. */
#include <stdio.h>
#include <string.h>
#include "dosbox.h"
#include "regs.h"
#include "mem.h"
#include "paging.h"
#include "inout.h"
#include "pic.h"
#include "guest.h"
#include "guest_calls.h"
static uint32_t reply;
static unsigned counts[GRX_CALL_COUNT];
static int read_bytes(void *,uint32_t a,void *out,unsigned n){
 uint8_t *p=(uint8_t *)out;
 if(n&&a>UINT32_MAX-(n-1))return 0;
 for(unsigned i=0;i<n;i++)if(mem_readb_checked(a+i,p+i))return 0;
 return 1;
}
static int write_bytes(void *,uint32_t a,const void *in,unsigned n){
 const uint8_t *p=(const uint8_t *)in;
 if(n&&a>UINT32_MAX-(n-1))return 0;
 for(unsigned i=0;i<n;i++)if(mem_writeb_checked(a+i,p[i]))return 0;
 return 1;
}
static Bitu read_port(Bitu port,Bitu){return port==GRX_GATE_PORT?GRX_GATE_MAGIC:reply;}
static void write_port(Bitu,Bitu id,Bitu){
 grx_guest_call call;grx_guest_io io={read_bytes,write_bytes,nullptr,(uint32_t)SegPhys(ds)};reply=0;
 uint64_t linear=(uint64_t)SegPhys(ss)+reg_ecx;
 if(linear>UINT32_MAX||!grx_guest_decode(&call,id,(uint32_t)linear,&io)){
  fprintf(stderr,"GLIDE invalid guest call id=%u stack=%08x:%08x\n",(unsigned)id,(unsigned)SegPhys(ss),(unsigned)reg_ecx);return;
 }
 if(counts[id]++<4){
  fprintf(stderr,"GLIDE tick=%u call=%s args=",(unsigned)PIC_Ticks,grx_calls[id].name);
  for(unsigned i=0;i<call.count;i++)fprintf(stderr,"%s%08x",i?",":"",call.args[i]);
  fputc('\n',stderr);
 }
 reply=grx_guest_diagnostic(&call,&io);
}
void db_glide_diagnostic_open(){
 memset(counts,0,sizeof(counts));reply=0;
 IO_RegisterReadHandler(GRX_GATE_PORT,read_port,IO_MD);
 IO_RegisterReadHandler(GRX_COMMAND_PORT,read_port,IO_MD);
 IO_RegisterWriteHandler(GRX_COMMAND_PORT,write_port,IO_MD);
}
void db_glide_diagnostic_close(){
 for(unsigned i=1;i<GRX_CALL_COUNT;i++)if(counts[i])fprintf(stderr,"GLIDE total %s=%u\n",grx_calls[i].name,counts[i]);
 IO_FreeReadHandler(GRX_GATE_PORT,IO_MD);IO_FreeReadHandler(GRX_COMMAND_PORT,IO_MD);
 IO_FreeWriteHandler(GRX_COMMAND_PORT,IO_MD);
}
