/* GPL-2.0-or-later. Bounded JIT profiles; shared by ARM and 68k builds. */
#pragma once
#ifndef DB_JIT_MIB
#define DB_JIT_MIB 1
#endif
#if DB_JIT_MIB != 1 && DB_JIT_MIB != 4 && DB_JIT_MIB != 32
#error Unsupported JIT cache size
#endif
#if DB_JIT_MIB == 32 && !defined(DB_EXPANDED)
#error 32 MiB JIT requires the 64 MiB expanded runtime
#endif
/* Metadata is separate from emitted code. Do not scale malloc usage eightfold
 * when growing 4->32 MiB; reserve room for the guest and device allocations. */
#define DB_JIT_BLOCKS (DB_JIT_MIB==32?65536u:8192u*DB_JIT_MIB)
#define DB_JIT_PAGES (DB_JIT_MIB==32?512u:128u)
#define DB_JIT_HEAP_MIB (DB_JIT_MIB==32?12u:DB_JIT_MIB==4?8u:4u)
