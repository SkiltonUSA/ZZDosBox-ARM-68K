/* GPL-3.0-or-later. Software scanline modes, independent of FPGA.
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

/* Internal modes: 0 off, 1 Classic rewrite benchmark, 2 Classic preserved,
 * 3 Soft (5/8), 4 Gradient (8/8, 6/8, 4/8, 6/8). Per output row. */
static inline unsigned pc_scan_level(unsigned mode,unsigned row){
 if(mode==1||mode==2)return (row&1)?0:8;
 if(mode==3)return (row&1)?5:8;
 if(mode==4){const unsigned levels[4]={8,6,4,6};return levels[row&3];}
 return 8;
}
/* The fourth byte is unused by the supported P96 scanout formats.
 * This scales it too; an alpha-consuming format needs a different operation. */
static inline uint32_t pc_scan_dim_pixel(uint32_t p,unsigned level){
 uint32_t lo=(((p&0x00ff00ffu)*level)>>3)&0x00ff00ffu;
 uint32_t hi=(((((p>>8)&0x00ff00ffu)*level)>>3)&0x00ff00ffu)<<8;
 return lo|hi;
}
