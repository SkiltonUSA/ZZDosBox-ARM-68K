/* GPL-3.0-or-later. Experimental software Classic mask, independent of FPGA.
 * Every entry refers to a validated, currently locked, owned framebuffer.
 * An epoch change invalidates all three slots after a display/mode transition. */
#pragma once
#include <stdint.h>
struct pc_scan_slot {uint32_t address,pitch,width,height,format;};
struct pc_scan_state {
 uint32_t epoch,mode,next;
 pc_scan_slot slots[3];
 uint32_t frames,rendered,cleared,skipped,repairs;
};
static inline bool pc_scan_dark(unsigned row){return (row&1)!=0;}
static inline bool pc_scan_begin(pc_scan_state *s,unsigned mode,unsigned epoch,
 unsigned address,unsigned pitch,unsigned width,unsigned height,unsigned format){
 if(s->epoch!=epoch||s->mode!=mode){
  s->epoch=epoch;s->mode=mode;s->next=0;
  for(unsigned i=0;i<3;i++)s->slots[i].address=0;
 }
 s->frames++;
 if(mode!=2)return true;
 for(unsigned i=0;i<3;i++){
  const pc_scan_slot &p=s->slots[i];
  if(p.address==address&&p.pitch==pitch&&p.width==width&&p.height==height&&p.format==format)return false;
 }
 pc_scan_slot &p=s->slots[s->next];s->next=(s->next+1)%3;
 p.address=address;p.pitch=pitch;p.width=width;p.height=height;p.format=format;
 return true;
}
#ifdef DB_NEON
extern "C" void pc_neon_black(volatile uint32_t *,unsigned);
#endif
static inline void pc_scan_black(volatile uint32_t *row,unsigned width){
#ifdef DB_NEON
 pc_neon_black(row,width);
#else
 for(unsigned x=0;x<width;x++)row[x]=0;
#endif
}
