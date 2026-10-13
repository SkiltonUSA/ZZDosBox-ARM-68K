/* GPL-3.0-or-later. Temporary mapping of a locked OS-owned RTG bitmap. */
#include "dosbox.h"
#include "pc.h"
#include "pc_wire.h"
#include "mmu.h"
#include "shell_api.h"
#include "direct_plan.h"
#include "scale.h"
#include "profile.h"
#include "fps.h"
extern "C" volatile uint8_t *db_shared();
static unsigned sequence;
static pc_fps fps;
static void fence(){__asm__ volatile("dsb sy":::"memory");}
static void tlb(){unsigned z=0;__asm__ volatile("dsb sy\n mcr p15,0,%0,c8,c7,0\n dsb sy\n isb"::"r"(z):"memory");}
void db_video_present(){
 volatile uint8_t *b=db_shared();unsigned seq=++sequence;
 ad_put(b,PC_VIDEO+4,db_width);ad_put(b,PC_VIDEO+8,db_height);ad_put(b,PC_VIDEO+12,db_frames);fence();ad_put(b,PC_VIDEO,seq);fence();
 pc_prof_enter(PP_GRANT);
 while(ad_get(b,PC_VIDEO+16)!=seq)db_shell_poll();fence();
 pc_prof_leave();pc_prof_enter(PP_COPY);
 unsigned addr=ad_get(b,PC_VIDEO+20),pitch=ad_get(b,PC_VIDEO+24),format=ad_get(b,PC_VIDEO+28),nonce=ad_get(b,PC_VIDEO+32);
 unsigned width=ad_get(b,PC_VIDEO+44),height=ad_get(b,PC_VIDEO+48);
 if(!width&&!height){width=640;height=480;} /* Legacy fullscreen/model grant. */
 uint32_t *state=(uint32_t*)(b+MB_MMU_STATE),*l1=(uint32_t*)state[15],table=state[15]+16384+MB_L2_BYTES;
 unsigned error=0,span=pc_span_size(addr,pitch,format,width,height);
 if((format!=3&&format!=4)||!span||table+4096>(unsigned)b+MB_TABLE_END||!pc_map_size(l1,(uint32_t*)table,table,addr,pitch,format,width,height))error=6;
 else{
  tlb();volatile uint8_t *dst=(volatile uint8_t*)addr;
  if(ad_get(dst,0)!=nonce||ad_get(dst,span-4)!=(nonce^0x31415926u))error=7;
  else {
   // Match the PC image's 4:3 display area; VGA scanlines may already be doubled.
   for(unsigned y=0;y<height;y++){
    if(ad_get(b,ZZ_STOP)){error=2;break;}
    volatile uint32_t *row=(volatile uint32_t*)(dst+y*pitch);
    const uint32_t *src=db_pixels+(y*db_height/height)*db_width;
    pc_scale_row(row,src,db_width,format,width);
   }
   if(!error&&(ad_get(b,PC_UI_FLAGS)&1)){
    /* XX19c Zynq global timer: nominal 666.667 MHz CPU / 2. */
    pc_fps_update(&fps,pc_clock_ticks(),db_frames,333333333u);
    pc_fps_draw(dst,pitch,format,fps.tenths,width);ad_put(b,PC_FPS_TENTHS,fps.tenths);
   }
  }
  fence();pc_unmap(l1,addr,span);tlb();
 }
 ad_put(b,PC_VIDEO+40,error);fence();ad_put(b,PC_VIDEO+36,seq);fence();
 pc_prof_leave();
 if(error==2)db_shell_poll(); /* Mapping is restored before cooperative unwind. */
 if(error)E_Exit("Framebuffer handoff error %u",error);
}
