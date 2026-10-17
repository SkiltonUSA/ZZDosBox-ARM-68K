/* GPL-2.0-or-later. Offsets inside the existing 32 MiB Exec allocation. */
#pragma once
#include <stdint.h>
#include "capabilities.h"
#ifndef DB_JIT_MIB
#define DB_JIT_MIB 1
#endif
#if DB_JIT_MIB != 1 && DB_JIT_MIB != 4
#error Unsupported JIT cache size
#endif
#define DB_JIT_END   (DB_OWNED_BYTES-0xe0000u)
#define DB_JIT_CACHE (DB_JIT_MIB*0x100000u)
#define DB_JIT_BEGIN (DB_JIT_END-DB_JIT_CACHE-0x20000u)
static inline uint32_t db_jit_begin(uint32_t base) {
 return (base+DB_JIT_BEGIN+4095u)&~4095u;
}
static inline uint32_t db_jit_end(uint32_t base) {
 return (base+DB_JIT_END)&~4095u;
}
static inline int db_jit_page(uint32_t base,uint32_t page) {
 return page>=db_jit_begin(base)&&page<db_jit_end(base);
}
static inline int db_jit_span(uint32_t base,uint32_t at,uint32_t size) {
 return size&&at>=db_jit_begin(base)&&at<db_jit_end(base)&&size<=db_jit_end(base)-at;
}
