#ifndef GRX_CHANNEL_PLAN_H
#define GRX_CHANNEL_PLAN_H
#include <stdint.h>
#include "experimental/stream.h"
/* The returned firmware channel lives below framebuffer RAM. Only its pages
 * are mapped; the rest of firmware remains inaccessible to this worker. */
static inline int grx_channel_map(uint32_t *l1,uint32_t *l2,uint32_t table,uint32_t a,uint32_t bytes){
 if(a<0x100000||a>=0x200000||(a&4095)||bytes!=sizeof(grx_stream_channel)||
    bytes>0x200000-a||(table&1023)||l1[1])return 0;
 for(unsigned i=0;i<256;i++)l2[i]=0;
 for(uint32_t p=a;p<a+bytes;p+=4096)l2[(p>>12)&255]=p|0x453;
 l1[1]=table|1;return 1;
}
#endif
