/* ASan/UBSan differential test: exact pixels and bounds for every valid width. */
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "scale.h"
int main() {
 unsigned cases=0;
 for(unsigned width=1;width<=1024;width++) {
  uint32_t *src=(uint32_t*)malloc(width*sizeof(*src));
  assert(src);
  for(unsigned x=0;x<width;x++)src[x]=(x*0x1234567u)^0xa59e32b7u;
  for(unsigned format=3;format<=4;format++) {
   uint32_t dst[642];dst[0]=0x98765432;dst[641]=0x456789ab;
   pc_scale_row(dst+1,src,width,format);
   for(unsigned x=0;x<640;x++) {
    uint32_t c=src[(uint64_t)x*width/640];
    uint32_t expected=format==3?c:
      ((c&255u)<<24)|((c&0xff00u)<<8)|((c>>8)&0xff00u)|(c>>24);
    assert(dst[x+1]==expected);
   }
   assert(dst[0]==0x98765432 && dst[641]==0x456789ab);cases++;
  }
  free(src);
 }
 printf("PASS scaler: %u widths/formats, 1310720 exact pixels and row guards\n",cases);
}
