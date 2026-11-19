/* GPL-2.0-or-later. Decode guest bytes without aliasing, alignment or endian assumptions. */
#include <string.h>
#include "guest_vertex.h"
static float read_float(const uint8_t *p){
 uint32_t w=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
 float f;_Static_assert(sizeof(f)==sizeof(w),"32-bit IEEE float required");memcpy(&f,&w,sizeof(f));return f;
}
int grx_guest_vertices(grx_vertex out[3],const uint32_t pointers[3],int tmu_w,const grx_guest_io *io){
 grx_vertex v[3];
 for(unsigned i=0;i<3;i++){
  uint8_t b[60];uint64_t a=(uint64_t)io->data_base+pointers[i];
  if(a>UINT32_MAX-(sizeof(b)-1)||!io->read(io->opaque,(uint32_t)a,b,sizeof(b)))return 0;
  v[i]=(grx_vertex){read_float(b),read_float(b+4),read_float(b+12),read_float(b+16),read_float(b+20),read_float(b+28),read_float(b+36),read_float(b+40),read_float(b+(tmu_w?44:32))};
 }
 memcpy(out,v,sizeof(v));return 1;
}
