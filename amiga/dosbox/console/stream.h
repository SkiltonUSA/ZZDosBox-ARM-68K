/* GPL-2.0-or-later. SPSC byte rings in owned, uncached shared memory.
 * Every index has a separate cache line and exactly one writer. */
#pragma once
#include "dos_wire.h"
#define DB_STREAM_MODE 20u
#define DB_RX_WRITE 0x184000u
#define DB_RX_READ  0x184040u
#define DB_TX_WRITE 0x184080u
#define DB_TX_READ  0x1840c0u
#define DB_RX_EOF   0x184100u
#define DB_RX_DATA  0x184400u
#define DB_RX_SIZE  256u
#define DB_TX_DATA  0x185000u
#define DB_TX_SIZE  4096u
#if DB_TX_DATA+DB_TX_SIZE > DB_PROGRAM
#error Console overlaps staged executable
#endif
