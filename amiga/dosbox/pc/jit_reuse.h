/* GPL-2.0-or-later. Forward an immediately preceding CPU-register STR.
 * Only unconditional word accesses through the resident r10 base qualify.
 * The caller must exclude block entries and branch targets. */
#pragma once
#include <stdint.h>
static inline int db_arm_stored_register(uint32_t previous,uint32_t load){
 if((load&0xff7f0000u)!=0xe51a0000u)return -1;
 if((previous&0xffff0fffu)!=((load^0x00100000u)&0xffff0fffu))return -1;
 unsigned src=(previous>>12)&15u,dst=(load>>12)&15u;
 return src<4&&dst<4?(int)src:-1;
}
