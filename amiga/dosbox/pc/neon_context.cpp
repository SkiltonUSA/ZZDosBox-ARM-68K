/* GPL-3.0-or-later. Preserve firmware SIMD state across normal/error/stop exits.
 * Compile this translation unit with the original soft-float flags. */
#include "dosbox.h"
#include "pc_wire.h"
#include <string.h>
extern "C" volatile uint8_t *db_shared();
extern "C" unsigned db_neon_available(uint32_t *),db_neon_witness();
extern "C" void db_neon_ieee_begin();
extern "C" void db_neon_snapshot(uint32_t *),db_neon_restore(const uint32_t *);
alignas(8) static uint32_t before[65],after[65],policy[4],restored_policy[4];
static unsigned active,witness;
extern "C" void db_neon_begin(){
 if(!db_neon_available(policy))E_Exit("NEON unavailable under firmware policy");
 db_neon_snapshot(before);active=1;
 /* Library math can use VFP even when pixel conversion is scalar. Keep
  * denormals, disable exception traps and restore inherited FPSCR at exit. */
 db_neon_ieee_begin();
 witness=db_neon_witness();
 if(witness!=42)E_Exit("NEON execution witness failed");
}
extern "C" void db_neon_finish(){
 unsigned equal=0;
 if(active){
  db_neon_restore(before);db_neon_snapshot(after);
  equal=db_neon_available(restored_policy)&&!memcmp(before,after,sizeof(before))&&!memcmp(policy,restored_policy,sizeof(policy));
  active=0;
 }
 volatile uint8_t *b=db_shared();unsigned o=PC_VIDEO+0xd60;
 ad_put(b,o,0x4e454f31);ad_put(b,o+4,witness);ad_put(b,o+8,equal);
 for(unsigned i=0;i<4;i++)ad_put(b,o+12+i*4,policy[i]);
 __asm__ volatile("dsb sy":::"memory");
}
