/* Private candidate only. Firmware-owned channel mapped NC, never guest MMIO. */
#include "dosbox.h"
#include "mem.h"
#include "pc.h"
#include "pc_wire.h"
#include "mmu.h"
#include "shell_api.h"
#include "profile.h"
#include "live_wire.h"
#include "pixels_neon.h"
#include "guest_calls.h"
#include "channel_plan.h"
extern "C" {
#include "gta.h"
#include "experimental/stream.h"
}
#include <string.h>
extern "C" volatile uint8_t *db_shared();
extern void db_video_present();
static grx_gta game;
static volatile grx_stream_channel *channel;
static uint32_t sequence,nonce,*l1,section;
static uint64_t first,last;
static unsigned completed;
/* Exclusive elapsed ticks: transport/presentation plus lock, unlock, draw and
 * other Glide-call self time (subtract nested transport/presentation costs).
 * Snapshot only complete frames after warm-up; publish once during teardown. */
struct stage_stat {uint32_t calls;uint64_t bytes,ticks;};
static stage_stat stages[PC_GLIDE_PROFILE_STAGES],measured[PC_GLIDE_PROFILE_STAGES];
static void account(unsigned stage,unsigned bytes,uint64_t start){
 stages[stage].calls++;stages[stage].bytes+=bytes;stages[stage].ticks+=pc_clock_ticks()-start;
}
extern void db_glide_restore_video();
bool db_glide_live_active(){return game.opened!=0;}
static void barrier(){__asm__ volatile("dsb sy":::"memory");}
static void tlb(){unsigned z=0;__asm__ volatile("dsb sy\n mcr p15,0,%0,c8,c7,0\n dsb sy\n isb"::"r"(z):"memory");}
static int exchange(unsigned op,uint32_t address,void *data,unsigned n){
 if(!channel||!sequence)return 0;
 uint64_t elapsed=pc_clock_ticks();
 if(op!=GRX_STREAM_READ)memcpy((void*)channel->data,data,n);
 volatile grx_stream_request *q=&channel->request;
 q->nonce=nonce;q->op=op;q->address=address;q->bytes=n;barrier();q->sequence=sequence;barrier();
 uint64_t start=pc_clock_ticks();
 while(channel->result.sequence!=sequence){
  if(pc_clock_ticks()-start>1200000000ull)return 0;
  /* Poll host input/file services; never release an outstanding GPU request. */
  db_shell_poll();barrier();
 }
 barrier();if(channel->result.nonce!=nonce||channel->result.error||channel->result.bytes!=n)return 0;
 /* Only completed payload RAM, mapped normal non-cacheable by channel_plan;
  * never vector-read request/result registers or cross the returned payload. */
 if(op==GRX_STREAM_READ)grx_readback_copy(data,(const void*)channel->data,n);
 account(op-1,n,elapsed);
 sequence++;return 1;
}
static int transfer(unsigned op,uint32_t address,void *data,unsigned n){
 auto *p=(unsigned char*)data;
 while(n){unsigned part=n>GRX_STREAM_BYTES?GRX_STREAM_BYTES:n;if(!exchange(op,address,p,part))return 0;address+=part;p+=part;n-=part;}return 1;
}
static int upload(void *,uint32_t a,const void *p,unsigned n){return transfer(GRX_STREAM_UPLOAD,a,const_cast<void*>(p),n);}
static int readback(void *,uint32_t a,void *p,unsigned n){return transfer(GRX_STREAM_READ,a,p,n);}
static int submit(void *,const grx_packet *p){return exchange(GRX_STREAM_SUBMIT,0,(void*)p->bytes,p->words*4);}
static int present(void *,const unsigned char *pixels,unsigned w,unsigned h){
 db_width=w;db_height=h;db_bpp=32;
 uint64_t convert=pc_clock_ticks();
 grx_rgb565_xrgb(pixels,db_pixels,w*h);
 account(3,w*h*4,convert);
 uint64_t paint=pc_clock_ticks();db_frames++;db_video_present();
 /* Copy completion alone is insufficient: count only a host-painted window
  * or acknowledged fullscreen PAN. Errors unwind without a completed frame. */
 uint32_t wanted=ad_get(db_shared(),PC_VIDEO);uint64_t wait=pc_clock_ticks();
 while(ad_get(db_shared(),PC_GLIDE_DISPLAY_ACK)!=wanted){
  if(pc_clock_ticks()-wait>1200000000ull)return 0;
  db_shell_poll();barrier();
 }
 completed++;
 account(4,w*h*4,paint);
 uint64_t now=pc_clock_ticks();if(completed==10)first=now;if(completed>10)last=now;
 if(completed==10)memset(stages,0,sizeof(stages));
 if(completed>10)memcpy(measured,stages,sizeof(stages));
 volatile uint8_t *shared=db_shared();
 ad_put(shared,PC_GLIDE_REPORT+4,completed);ad_put(shared,PC_GLIDE_REPORT+8,first>>32);ad_put(shared,PC_GLIDE_REPORT+12,first);
 ad_put(shared,PC_GLIDE_REPORT+16,last>>32);ad_put(shared,PC_GLIDE_REPORT+20,last);barrier();ad_put(shared,PC_GLIDE_REPORT,PC_GLIDE_MAGIC);return 1;
}
void db_glide_live_open(uint32_t lfb){
 volatile uint8_t *shared=db_shared();
 uint32_t address=ad_get(shared,PC_GLIDE_HOST+4),bytes=ad_get(shared,PC_GLIDE_HOST+8);
 nonce=ad_get(shared,PC_GLIDE_HOST+12);
 if(ad_get(shared,PC_GLIDE_HOST)!=PC_GLIDE_MAGIC||address<0x100000||address>=0x200000||
    bytes!=sizeof(grx_stream_channel)||bytes>0x200000-address||(address&4095)||!nonce)E_Exit("Glide channel admission failed");
 uint32_t *state=(uint32_t*)(shared+MB_MMU_STATE);
 l1=(uint32_t*)state[15];uint32_t table=state[15]+16384+MB_L2_BYTES+4096;
 if(table+1024>(uintptr_t)shared+MB_TABLE_END||!grx_channel_map(l1,(uint32_t*)table,table,address,bytes))E_Exit("Glide channel mapping conflict");
 section=address>>20;tlb();
 channel=(volatile grx_stream_channel*)address;sequence=1;completed=0;first=last=0;
 memset(stages,0,sizeof(stages));memset(measured,0,sizeof(measured));
 grx_gta_backend backend={nullptr,upload,readback,submit,present};grx_gta_init(&game,&backend,lfb);
}
uint32_t db_glide_live_call(const grx_guest_call *c,const grx_guest_io *io){
 if(c->id==0)E_Exit("Invalid Glide call");
 unsigned was_open=game.opened;
 unsigned stage=8;
 if(c->id==GRX_CALL_grLfbLock)stage=5;
 else if(c->id==GRX_CALL_grLfbUnlock)stage=6;
 else if(c->id==GRX_CALL_grDrawTriangle||c->id==GRX_CALL_guDrawTriangleWithClip)stage=7;
 /* Swap resets warm-up counters and snapshots complete frames inside present.
  * Its transport/conversion/display work is already counted separately. */
 bool profile=c->id!=GRX_CALL_grBufferSwap;
 uint64_t nested=0,start=0;
 if(profile){for(unsigned i=0;i<5;i++)nested+=stages[i].ticks;start=pc_clock_ticks();}
 uint32_t reply;grx_result r=grx_gta_call(&game,c,io,&reply);
 if(profile){
  uint64_t elapsed=pc_clock_ticks()-start,after=0;
  for(unsigned i=0;i<5;i++)after+=stages[i].ticks;
  stages[stage].calls++;stages[stage].ticks+=elapsed-(after-nested);
 }
 if(r!=GRX_OK){ad_put(db_shared(),PC_GLIDE_REPORT+24,c->id);ad_put(db_shared(),PC_GLIDE_REPORT+28,r);E_Exit("Glide call %u failed (%u)",c->id,r);}
 if(was_open&&!game.opened)db_glide_restore_video();
 return reply;
}
void db_glide_live_close(){if(channel){
 for(unsigned i=0;i<PC_GLIDE_PROFILE_STAGES;i++){
  unsigned at=PC_GLIDE_PROFILE+i*PC_GLIDE_PROFILE_STRIDE;const stage_stat &s=measured[i];
  ad_put(db_shared(),at,s.calls);ad_put(db_shared(),at+4,s.bytes>>32);ad_put(db_shared(),at+8,s.bytes);
  ad_put(db_shared(),at+12,s.ticks>>32);ad_put(db_shared(),at+16,s.ticks);
 }
 barrier();l1[section]=0;tlb();channel=nullptr;
}}
