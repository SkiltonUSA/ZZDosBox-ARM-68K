/* Included by adapter.cpp after RAM/page-handler definitions. */
#include "memory_wire.h"
static dm_scan memory_scan;
static dm_reply memory_reply;
static unsigned memory_serial,run_depth;
static uint32_t memory_read(uint32_t a,unsigned n){
 uint32_t v=0;for(unsigned i=0;i<n;i++)v|=uint32_t(ram[a+i])<<(8*i);return v;
}
static void memory_write(uint32_t a,unsigned n,uint32_t v){
 for(unsigned i=0;i<n;i++){
  PageHandler *h=MEM_GetPageHandler((a+i)>>12);
  /* Writes through a translated-code handler invalidate compiled x86 blocks. */
  if(h->flags&(PFLAG_HASCODE16|PFLAG_HASCODE32))h->writeb(a+i,(v>>(8*i))&255);
  else ram[a+i]=(v>>(8*i))&255;
 }
}
static void memory_debug_poll(){
 volatile uint8_t *b=db_shared();
 do{
  unsigned seq=ad_get(b,DM_SEQUENCE);
  if(seq&&seq!=memory_serial){
   dm_request q;uint32_t *words=(uint32_t*)&q;
   for(unsigned i=0;i<8;i++)words[i]=ad_get(b,DM_COMMAND+i*4);
   dm_execute(&memory_scan,&q,&memory_reply,db_memory_mb*1024u*1024u,memory_read,memory_write);
   memory_reply.eip=reg_eip;memory_reply.cs=SegValue(cs);memory_reply.esp=reg_esp;
   /* Lazy flags are deliberately not materialised just for diagnostics. */
   memory_reply.flags=reg_flags;
   const uint32_t *r=(const uint32_t*)&memory_reply;
   for(unsigned i=0;i<8;i++)ad_put(b,DM_RESPONSE+i*4,r[i]);
   for(unsigned i=0;i<DM_BYTES;i++)b[DM_RESPONSE+32+i]=memory_reply.bytes[i];
   __asm__ volatile("dsb sy":::"memory");memory_serial=seq;ad_put(b,DM_ACK,seq);
   __asm__ volatile("dsb sy":::"memory");
  }
  if(memory_reply.paused)check_stop();
 }while(memory_reply.paused);
}
static void memory_debug_reset(){memory_serial=run_depth=0;memset(&memory_scan,0,sizeof(memory_scan));memset(&memory_reply,0,sizeof(memory_reply));}
