/* GPL-2.0-or-later. Read the launcher's already mapped global timer. */
#include "profile.h"
#include "pc_wire.h"
extern "C" volatile uint8_t *db_shared();
bool pc_prof_enabled;
static pc_profile profile;
static uint64_t now(){
 volatile uint32_t *t=(volatile uint32_t*)0xf8f00200u;uint32_t hi,lo;
 __asm__ volatile("dsb sy":::"memory");
 do{hi=t[1];lo=t[0];}while(hi!=t[1]);
 return (uint64_t)hi<<32|lo;
}
void pc_prof_start(){
 pc_prof_enabled=ad_get(db_shared(),PC_PROFILE)!=0;
 if(pc_prof_enabled)pp_init(&profile,now());
}
void pc_prof_push(unsigned n){pp_enter(&profile,n,now());}
void pc_prof_pop(){pp_leave(&profile,now());}
void pc_prof_unwind(){if(pc_prof_enabled)pp_unwind(&profile,now());}
void pc_prof_finish(){
 if(!pc_prof_enabled)return;
 pp_finish(&profile,now());pc_prof_enabled=false;
 volatile uint8_t *p=db_shared()+PC_PROFILE;
 ad_put(p,8,profile.total>>32);ad_put(p,12,profile.total);
 ad_put(p,16,profile.unwound);ad_put(p,20,profile.error);
 for(unsigned i=0;i<PP_COUNT;i++){
  ad_put(p,24+i*16,profile.ticks[i]>>32);ad_put(p,28+i*16,profile.ticks[i]);
  ad_put(p,32+i*16,profile.calls[i]);
 }
 __asm__ volatile("dsb sy":::"memory");ad_put(p,4,PC_PROFILE_MAGIC);
}
