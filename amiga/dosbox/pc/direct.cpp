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
#include "video_trace.h"
#ifdef DB_VIDEO_TRACE
static unsigned trace_serial;
static void video_trace(unsigned phase,unsigned frame,unsigned detail){
 volatile uint8_t *b=db_shared();if(!ad_get(b,PC_DEV_TRACE))return;unsigned serial=++trace_serial;
 /* Zero is reserved for an uncommitted slot. Diagnostics are short runs. */
 if(!serial)serial=++trace_serial;
 unsigned o=pc_trace_slot(serial);uint64_t ticks=pc_clock_ticks();
 ad_put(b,o,0);__asm__ volatile("dsb sy":::"memory");
 ad_put(b,o+4,phase);ad_put(b,o+8,frame);ad_put(b,o+12,detail);
 ad_put(b,o+16,ticks>>32);ad_put(b,o+20,ticks);
 __asm__ volatile("dsb sy":::"memory");ad_put(b,o,serial);
 __asm__ volatile("dsb sy":::"memory");ad_put(b,PC_TRACE,serial);
 __asm__ volatile("dsb sy":::"memory");
}
static unsigned checkpoint_token;
static void video_checkpoint(unsigned phase,unsigned frame){
 volatile uint8_t *b=db_shared();
 if(ad_get(b,PC_DEV_PHASE)!=phase||ad_get(b,PC_DEV_FRAME)!=frame)return;
 unsigned token=++checkpoint_token;
 ad_put(b,PC_DEV_AT_FRAME,frame);ad_put(b,PC_DEV_AT_PHASE,phase);
 __asm__ volatile("dsb sy":::"memory");ad_put(b,PC_DEV_PAUSED,token);
 __asm__ volatile("dsb sy":::"memory");
 while(ad_get(b,PC_DEV_RESUME)!=token)db_shell_poll();
 __asm__ volatile("dsb sy":::"memory");ad_put(b,PC_DEV_PAUSED,0);
 __asm__ volatile("dsb sy":::"memory");
}
extern "C" void db_video_debug_end(){
 ad_put(db_shared(),PC_DEV_PAUSED,0);__asm__ volatile("dsb sy":::"memory");
}
#else
static inline void video_checkpoint(unsigned,unsigned){}
static inline void video_trace(unsigned,unsigned,unsigned){}
#endif
#ifdef DB_OPENGL
#include "renderer.h"
static unsigned gl_frames,gl_direct,gl_fallback;static uint64_t gl_ticks;
extern "C" void db_gl_shutdown(){
 zz_gl_close();video_trace(PT_CLOSED,gl_frames,0);volatile uint8_t*b=db_shared();const unsigned o=PC_VIDEO+0xde0u;
 ad_put(b,o+4,gl_frames);ad_put(b,o+8,gl_ticks>>32);ad_put(b,o+12,gl_ticks);
 ad_put(b,o+16,zz_gl_pixels()==nullptr);ad_put(b,o+20,gl_direct);ad_put(b,o+24,gl_fallback);
 __asm__ volatile("dsb sy":::"memory");ad_put(b,o,0x474c3031);
}
#endif
#ifdef DB_ROW_CACHE
#include "row_cache.h"
static pc_row_cache converted_row;
#endif
static unsigned sequence;
static pc_fps fps;
#ifdef DB_PRESENT_TEST
#include "presentation_plan.h"
static pc_presentation presentation;
void db_presentation_reset(){pc_presentation_reset(&presentation);}
void db_presentation_report(){
 volatile uint8_t *b=db_shared();unsigned o=PC_PRESENT_REPORT;
 ad_put(b,o+4,presentation.frames);ad_put(b,o+8,presentation.presented);
 ad_put(b,o+12,presentation.skipped);ad_put(b,o+16,presentation.divisor);
 ad_put(b,o+20,presentation.changes);ad_put(b,o+24,presentation.mode);
 ad_put(b,o+28,presentation.hz);ad_put(b,o+32,presentation.work_mean);
 ad_put(b,o+36,presentation.copy_mean);
 ad_put(b,o+40,presentation.copy_total>>32);ad_put(b,o+44,presentation.copy_total);
 __asm__ volatile("dsb sy":::"memory");ad_put(b,o,0x50525331);
}
#endif
#ifdef DB_SCANLINE_TEST
#include "scanline_pixels.h"
static pc_scan_state scan;
#endif
static void fence(){__asm__ volatile("dsb sy":::"memory");}
static void tlb(){unsigned z=0;__asm__ volatile("dsb sy\n mcr p15,0,%0,c8,c7,0\n dsb sy\n isb"::"r"(z):"memory");}
void db_video_present(){
 volatile uint8_t *b=db_shared();
#ifdef DB_PRESENT_TEST
 uint64_t present_start=pc_clock_ticks();
 if(!pc_presentation_begin(&presentation,ad_get(b,PC_PRESENT_MODE),ad_get(b,PC_PRESENT_HZ),present_start)){
  /* No sequence increment, grant, mapping or bitmap writes on a skipped
   * frame. Keep cancellation and transport service alive. */
  pc_presentation_end(&presentation,present_start,pc_clock_ticks(),false);
  db_shell_poll();return;
 }
#endif
 video_checkpoint(1,sequence+1);
 video_trace(PT_PREPARE,sequence+1,0);
 const uint32_t *frame_pixels=db_pixels;unsigned frame_width=db_width,frame_height=db_height;
#ifdef DB_OPENGL
 /* Allocate before mapping: mapped rendering must not unwind through malloc. */
 if(!zz_gl_prepare())E_Exit("TinyGL initialization failed");
#endif
 unsigned seq=++sequence;video_trace(PT_WAIT_GRANT,seq,0);
 ad_put(b,PC_VIDEO+4,db_width);ad_put(b,PC_VIDEO+8,db_height);ad_put(b,PC_VIDEO+12,db_frames);fence();ad_put(b,PC_VIDEO,seq);fence();
 pc_prof_enter(PP_GRANT);
 while(ad_get(b,PC_VIDEO+16)!=seq)db_shell_poll();fence();
 video_trace(PT_GRANTED,seq,0);
 pc_prof_leave();pc_prof_enter(PP_COPY);
 unsigned addr=ad_get(b,PC_VIDEO+20),pitch=ad_get(b,PC_VIDEO+24),format=ad_get(b,PC_VIDEO+28),nonce=ad_get(b,PC_VIDEO+32);
 unsigned width=ad_get(b,PC_VIDEO+44),height=ad_get(b,PC_VIDEO+48);
 if(!width&&!height){width=640;height=480;} /* Legacy fullscreen/model grant. */
 uint32_t *state=(uint32_t*)(b+MB_MMU_STATE),*l1=(uint32_t*)state[15],table=state[15]+16384+MB_L2_BYTES;
 unsigned error=0,span=pc_span_size(addr,pitch,format,width,height);
 if((format!=3&&format!=4)||!span||table+4096>(unsigned)b+MB_TABLE_END||!pc_map_size(l1,(uint32_t*)table,table,addr,pitch,format,width,height))error=6;
 else{
  tlb();video_trace(PT_MAPPED,seq,span);volatile uint8_t *dst=(volatile uint8_t*)addr;
  if(ad_get(dst,0)!=nonce||ad_get(dst,span-4)!=(nonce^0x31415926u))error=7;
  else {
   video_trace(PT_NONCE,seq,0);
   unsigned mask=0;
#ifdef DB_SCANLINE_TEST
   mask=ad_get(b,PC_VIDEO+52);unsigned epoch=ad_get(b,PC_VIDEO+56);
   /* Experimental mask only on the fixed fullscreen geometry, without HUD. */
   if(mask>4||width!=640||height!=480||(ad_get(b,PC_UI_FLAGS)&1))mask=0;
   bool clear=pc_scan_begin(&scan,mask,epoch,addr,pitch,width,height,format);
#endif
   video_trace(PT_RENDER,seq,format);
   bool rendered_direct=false;
#ifdef DB_OPENGL
   uint64_t gl_start=pc_clock_ticks();
   if(width==640&&height==480&&!mask){
    int result=zz_gl_render_to(db_pixels,db_width,db_height,
      (volatile uint32_t*)dst,pitch,format,b+ZZ_STOP);
    if(result!=1)error=result<0?2:8;
    rendered_direct=true;gl_direct++;
#ifdef DB_SCANLINE_TEST
    scan.rendered+=height;
#endif
   }else{
    /* Preserve two-stage scaling and mask semantics for all other geometries. */
    if(!zz_gl_render(db_pixels,db_width,db_height))error=8;
    frame_pixels=zz_gl_pixels();frame_width=640;frame_height=480;gl_fallback++;
   }
   gl_ticks+=pc_clock_ticks()-gl_start;gl_frames++;
#endif
#ifdef DB_ROW_CACHE
   converted_row.valid=false;
#endif
   // Match the PC image's 4:3 display area; VGA scanlines may already be doubled.
   for(unsigned y=0;!error&&!rendered_direct&&y<height;y++){
    if(ad_get(b,ZZ_STOP)){error=2;break;}
    volatile uint32_t *row=(volatile uint32_t*)(dst+y*pitch);
#ifdef DB_SCANLINE_TEST
    if((mask==1||mask==2)&&pc_scan_dark(y)){
     if(clear){pc_scan_black(row,width);scan.cleared++;}
     else scan.skipped++;
     continue;
    }
    scan.rendered++;
#endif
    const uint32_t *src=frame_pixels+(y*frame_height/height)*frame_width;
#ifdef DB_ROW_CACHE
    pc_cached_row(&converted_row,row,src,frame_width,format,width,
#ifdef DB_SCANLINE_TEST
      pc_scan_level(mask,y),
#else
      8,
#endif
      frame_height<height);
#elif defined(DB_SCANLINE_TEST)
    pc_scan_scale_row(row,src,frame_width,format,width,pc_scan_level(mask,y));
#else
    pc_scale_row(row,src,frame_width,format,width);
#endif
   }
#ifdef DB_SCANLINE_TEST
   /* The ownership challenge touches the final pixel on a black row on
    * EVERY grant. Repair it even when the rest of that row was preserved. */
   if((mask==1||mask==2)&&!error&&pc_scan_dark(height-1)){
    *(volatile uint32_t*)(dst+span-4)=0;scan.repairs++;
   }
   const unsigned o=PC_VIDEO+0xd80;
   ad_put(b,o,0x53434e31);ad_put(b,o+4,scan.frames);
   ad_put(b,o+8,scan.rendered);ad_put(b,o+12,scan.cleared);
   ad_put(b,o+16,scan.skipped);ad_put(b,o+20,scan.repairs);
#endif
   video_trace(PT_RENDERED,seq,error);
   if(!error&&(ad_get(b,PC_UI_FLAGS)&1)){
    /* XX19c Zynq global timer: nominal 666.667 MHz CPU / 2. */
    pc_fps_update(&fps,pc_clock_ticks(),sequence,333333333u);
    pc_fps_draw(dst,pitch,format,fps.tenths,width);ad_put(b,PC_FPS_TENTHS,fps.tenths);
   }
  }
  fence();pc_unmap(l1,addr,span);tlb();video_trace(PT_UNMAPPED,seq,error);
 }
 ad_put(b,PC_VIDEO+40,error);fence();ad_put(b,PC_VIDEO+36,seq);fence();
 video_trace(PT_COMPLETE,seq,error);
 pc_prof_leave();
#ifdef DB_PRESENT_TEST
 if(!error)pc_presentation_end(&presentation,present_start,pc_clock_ticks(),true);
#endif
 if(error==2)db_shell_poll(); /* Mapping is restored before cooperative unwind. */
 if(error)E_Exit("Framebuffer handoff error %u",error);
 video_checkpoint(2,seq);
}
