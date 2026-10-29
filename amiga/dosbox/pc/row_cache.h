/* GPL-3.0-or-later. Per-frame converted row, held in ARM cached RAM.
 * No framebuffer reads. Caller resets valid before every frame, after selecting
 * its immutable source. Geometry/format/brightness form part of the key. */
#pragma once
#include "scanline_pixels.h"
struct pc_row_cache {
 uint32_t pixels[960];
 const uint32_t *source;
 unsigned count,format,width,level,hits,conversions;
 bool valid;
};
static inline void pc_cached_row(pc_row_cache *cache,volatile uint32_t *out,
 const uint32_t *src,unsigned count,unsigned format,unsigned width,
 unsigned level,bool enabled){
 if(!enabled||width>960){pc_scan_scale_row(out,src,count,format,width,level);return;}
 if(!cache->valid||cache->source!=src||cache->count!=count||
    cache->format!=format||cache->width!=width||cache->level!=level){
  pc_scan_scale_row(cache->pixels,src,count,format,width,level);
  cache->source=src;cache->count=count;cache->format=format;
  cache->width=width;cache->level=level;cache->valid=true;cache->conversions++;
 }else cache->hits++;
 /* Already in the destination byte order. Format 3 means no second swap. */
 pc_scale_row(out,cache->pixels,width,3,width);
}
