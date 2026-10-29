/* GPL-2.0-or-later. Guest options and build-time owned-memory limits. */
#pragma once
#include "jit_config.h"
#ifdef DB_EXPANDED
#define DB_CYCLES_MAX 100000u
#define DB_GUEST_MAX_MB (DB_JIT_MIB==32?8u:32u)
#define DB_OWNED_BYTES 0x4000000u
#else
#define DB_CYCLES_MAX 10000u
#define DB_GUEST_MAX_MB 8u
#define DB_OWNED_BYTES 0x2000000u
#endif
#define DB_GUEST_PAGES (DB_GUEST_MAX_MB*256u)
enum { DB_CPU_386, DB_CPU_486, DB_CPU_PENTIUM, DB_CPU_COUNT };
static inline int db_memory_valid(unsigned mb, unsigned maximum) {
 return mb && mb<=maximum && !(mb&(mb-1));
}
