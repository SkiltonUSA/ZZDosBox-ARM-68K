/* GPL-3.0-or-later. Diagnostic-only ARM producer / 68k reader ring.
 * No ownership, completion or timeout decisions may depend on this trace. */
#pragma once
#include "pc_wire.h"
#define PC_TRACE (PC_VIDEO+0xe00u)
#define PC_TRACE_COUNT 16u
#define PC_TRACE_STRIDE 24u
#define PC_TRACE_SIZE (16u+PC_TRACE_COUNT*PC_TRACE_STRIDE)
#define PC_DEV (PC_VIDEO+0xfa0u)
/* Host: flags, breakpoint phase (1 before/2 after), frame, resume token.
 * ARM: paused token, paused frame, phase. Never pause with an ARM RTG map. */
#define PC_DEV_TRACE PC_DEV
#define PC_DEV_PHASE (PC_DEV+4u)
#define PC_DEV_FRAME (PC_DEV+8u)
#define PC_DEV_RESUME (PC_DEV+12u)
#define PC_DEV_PAUSED (PC_DEV+16u)
#define PC_DEV_AT_FRAME (PC_DEV+20u)
#define PC_DEV_AT_PHASE (PC_DEV+24u)
#if PC_TRACE+PC_TRACE_SIZE > PC_DEV || PC_DEV+28u > PC_BOOT_TEXT
#error Developer controls overlap another shared region
#endif
#if PC_TRACE < PC_PRESENT_REPORT+PC_PRESENT_SIZE || PC_TRACE+PC_TRACE_SIZE > PC_BOOT_TEXT
#error Video trace overlaps another report or boot text
#endif
enum pc_trace_phase {
 PT_PREPARE=1, PT_WAIT_GRANT, PT_GRANTED, PT_MAPPED, PT_NONCE,
 PT_RENDER, PT_RENDERED, PT_UNMAPPED, PT_COMPLETE, PT_CLOSED
};
static inline unsigned pc_trace_slot(unsigned serial){
 return PC_TRACE+16u+((serial-1u)&(PC_TRACE_COUNT-1u))*PC_TRACE_STRIDE;
}
static inline const char *pc_trace_name(unsigned phase){
 switch(phase){
 case PT_PREPARE:return "prepare";case PT_WAIT_GRANT:return "wait-grant";
 case PT_GRANTED:return "granted";case PT_MAPPED:return "mapped";
 case PT_NONCE:return "nonce-ok";case PT_RENDER:return "render";
 case PT_RENDERED:return "rendered";case PT_UNMAPPED:return "unmapped";
 case PT_COMPLETE:return "complete";case PT_CLOSED:return "closed";
 default:return "unknown";
 }
}
