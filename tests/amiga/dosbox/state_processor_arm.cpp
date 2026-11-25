/* GPL-2.0-or-later. Actual interpreter continuation on ARM32 from native records. */
#include <assert.h>
#include <new>
#include "state_fpu_fixture.h"
#ifndef PROCESSOR_GOLDEN
#include "processor-golden.h"
extern "C" void __assert_func(const char*,int,const char*,const char*){__builtin_trap();}
extern "C" void _exit(int){__builtin_trap();}
// Static fixture objects live for the entire probe; no exit destructors run.
extern "C" int __aeabi_atexit(void*,void(*)(void*),void*){return 0;}
void operator delete(void*,size_t) noexcept {__builtin_trap();}
#endif
static const unsigned checkpoints[]={0,5,20,60,120};
static const unsigned memory_begin=0x18000,memory_bytes=1024;
static uint32_t memory_hash(){
 uint32_t h=2166136261u;for(unsigned n=0;n<sizeof(ram);n++)h=(h^ram[n])*16777619u;return h;
}
static void word(uint8_t *p,uint32_t v){for(unsigned n=0;n<4;n++)p[n]=v>>(8*n);}
int main(){
 // Bare-metal entry does not run the host C++ startup constructor list.
 new (&ram_page) RamPage;
 uint8_t wire[db_processor_checkpoint_bytes],after[db_processor_checkpoint_bytes];
 uint8_t hash[4];
#ifdef PROCESSOR_GOLDEN
 for(unsigned at:checkpoints){
  initialise();step(at);if(!db_processor_checkpoint_save(wire,sizeof(wire)))return 1;
  if(fwrite(wire,1,sizeof(wire),stdout)!=sizeof(wire))return 2;
  if(fwrite(ram+memory_begin,1,memory_bytes,stdout)!=memory_bytes)return 3;
  step(300-at);if(!db_processor_checkpoint_save(after,sizeof(after)))return 4;
  if(fwrite(after,1,sizeof(after),stdout)!=sizeof(after))return 5;
  word(hash,memory_hash());if(fwrite(hash,1,4,stdout)!=4)return 6;
 }
#else
 unsigned offset=0;
 for(unsigned at:checkpoints){
  const uint8_t *saved=golden+offset;offset+=sizeof(wire);
  const uint8_t *memory=golden+offset;offset+=memory_bytes;
  const uint8_t *expected=golden+offset;offset+=sizeof(after);
  const uint8_t *expected_hash=golden+offset;offset+=4;
  initialise();step(25); // Deliberately leave different CPU/FPU and RAM state.
  memcpy(ram+memory_begin,memory,memory_bytes);
  if(!db_processor_checkpoint_load(saved,sizeof(wire)))return 10;
  step(300-at);
  if(!db_processor_checkpoint_save(after,sizeof(after))||memcmp(after,expected,sizeof(after)))return 11;
  word(hash,memory_hash());if(memcmp(hash,expected_hash,4))return 12;
  for(unsigned n=0;n<sizeof(wire);n++){
   memcpy(wire,saved,sizeof(wire));wire[n]^=0x80;
   if(db_processor_checkpoint_load(wire,sizeof(wire)))return 13;
   if(!db_processor_checkpoint_save(wire,sizeof(wire))||memcmp(wire,expected,sizeof(wire)))return 14;
  }
 }
 if(offset!=sizeof(golden))return 15;
#endif
 return 0;
}
