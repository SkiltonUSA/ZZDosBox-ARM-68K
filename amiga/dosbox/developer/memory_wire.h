/* GPL-3.0-or-later. Application-owned DDR, separate command/reply cache lines. */
#pragma once
#include "memory_plan.h"
#define DM_WIRE 0x18c000u
#define DM_COMMAND DM_WIRE
#define DM_SEQUENCE (DM_WIRE+64u)
#define DM_ACK (DM_WIRE+128u)
#define DM_RESPONSE (DM_WIRE+192u)
#define DM_WIRE_END (DM_RESPONSE+sizeof(struct dm_reply))
#define DM_MAGIC 0x5a444d31u
#if DM_WIRE < 0x18c000u
#error Memory debugger overlaps MIDI data
#endif
/* Compile-time layout bounds without depending on native sizeof in #if. */
typedef char dm_wire_bounds[(DM_WIRE_END<=0x190000u && sizeof(struct dm_request)==32 && sizeof(struct dm_reply)==288)?1:-1];
