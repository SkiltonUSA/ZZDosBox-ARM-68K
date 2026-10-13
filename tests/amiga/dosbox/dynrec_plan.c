#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "dynrec_plan.h"
int main(void){
 for(uint32_t off=0;off<4096;off+=64){
  uint32_t base=0x12000000+off,lo=db_jit_begin(base),hi=db_jit_end(base);
  assert(!(lo&4095)&&!(hi&4095)&&lo>=base+DB_JIT_BEGIN&&hi<=base+DB_JIT_END);
  assert(db_jit_span(base,lo,DB_JIT_CACHE+16383));
  assert(db_jit_span(base,hi-4,4));
  assert(!db_jit_span(base,lo-1,1)&&!db_jit_span(base,hi,1));
  assert(!db_jit_span(base,lo,0)&&!db_jit_span(base,hi-4,5));
  assert(!db_jit_span(base,0xfffffff0,32));
  assert(!db_jit_page(base,lo-4096)&&db_jit_page(base,lo));
  assert(db_jit_page(base,hi-4096)&&!db_jit_page(base,hi));
 }
 puts("PASS JIT owned arena spans, partial pages, bounds and overflow");
}
