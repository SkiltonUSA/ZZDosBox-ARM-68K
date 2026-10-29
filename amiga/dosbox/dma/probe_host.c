/* GPL-3.0-or-later. Uses the normal single-Core1, reset-before-free launcher. */
#include <proto/dos.h>
#include <stdio.h>
#include "probe_wire.h"
int ff_window(volatile uint8_t *base,const struct ad_io *io,int connected)
{
 unsigned i,n;volatile uint8_t *r=base+DP_REPORT;(void)connected;
 for(i=0;i<100;i++) {
  io->pull(r,DP_BYTES,0);
  if(ad_get(r,0)==DP_MAGIC)break;
  Delay(1);
 }
 if(ad_get(r,0)!=DP_MAGIC){puts("DMA survey incomplete; no channel started");return 20;}
 printf("DMA_SURVEY reset=%08lx security=%08lx irq_security=%08lx base=%08lx\n",
  (unsigned long)ad_get(r,4),(unsigned long)ad_get(r,8),
  (unsigned long)ad_get(r,12),(unsigned long)ad_get(r,16));
 n=ad_get(r,20);if(n!=0&&n!=DP_WORDS)return 20;
 for(i=0;i<n;i++)printf("DMA_REG offset=%03lx value=%08lx\n",
  (unsigned long)dp_offsets[i],
  (unsigned long)ad_get(r,32+4*i));
 puts("DMA submissions=0; channel ownership unestablished; player unchanged");
 return 0;
}
