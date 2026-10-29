#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "amiga/dosbox/pc/dynrec_plan.h"
int main(void) {
 assert(DB_OWNED_BYTES==64u*1024*1024);
 assert(DB_JIT_CACHE==32u*1024*1024);
 assert(DB_GUEST_MAX_MB==8);
 assert(!db_memory_valid(16,DB_GUEST_MAX_MB));
 for(uint32_t offset=0;offset<4096;offset+=64) {
  uint32_t base=0x12000000+offset,first=db_jit_begin(base),end=db_jit_end(base);
  assert(first>=base+0x1f00000&&end<=base+DB_OWNED_BYTES-0xe0000);
  assert(db_jit_span(base,first,DB_JIT_CACHE+8192+8191));
  assert(!db_jit_span(base,first-1,4)&&!db_jit_span(base,end-4,8));
  assert(!db_jit_span(base,first,0));
 }
 puts("PASS 32 MiB executable cache / 64 MiB allocation: guest limit, alignment, bounds");
}
