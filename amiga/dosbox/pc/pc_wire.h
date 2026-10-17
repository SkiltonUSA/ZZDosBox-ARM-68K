/* GPL-3.0-or-later. Single completed-frame handoff in owned uncached DDR. */
#pragma once
#include "stream.h"
#define PC_VIDEO 0x188000u
#define PC_DURATION (PC_VIDEO+64u)
#define PC_DONE (PC_VIDEO+68u)
/* Optional presentation experiment: immutable host config, ARM-only report.
 * d00/d40/d60/d80 are Doom/JIT/NEON/scanline reports; mouse ends at c80. */
#define PC_PRESENT_MODE (PC_VIDEO+72u)
#define PC_PRESENT_HZ (PC_VIDEO+76u)
#define PC_PRESENT_REPORT (PC_VIDEO+0xda0u)
#define PC_PRESENT_SIZE 64u
/* Optional profile result, published before the worker completion ACK. */
#define PC_PROFILE (PC_VIDEO+128u)
#define PC_PROFILE_MAGIC 0x50524631u
#define PC_PROFILE_SIZE 120u
/* ARM: sequence +0, width +4, height +8, frame +12.
 * Host: grant +16, ARM bitmap address +20, pitch +24, format +28, nonce +32.
 * ARM: complete +36, error +40. Host consumes only after complete. */
#define PC_DISPLAY_WIDTH 640u
#define PC_DISPLAY_HEIGHT 480u

/* Separate cache lines for the single-producer keyboard ring indices. */
#define PC_OPTIONS (PC_VIDEO+256u)
#define PC_HARDWARE_MAGIC (PC_OPTIONS+20u)
#define PC_HARDWARE_VERSION 0x48574431u
#define PC_CPU_MODEL (PC_OPTIONS+24u)
#define PC_MEMORY_SERVICES (PC_OPTIONS+28u) /* bit 0 VCPI, bit 1 UMB */
#define PC_JOYSTICK_ENABLED (PC_OPTIONS+32u)
#define PC_JOYSTICK_STATE (PC_VIDEO+972u) /* host-owned atomic packed digital state */
#define PC_EXTRA_SOUND (PC_OPTIONS+36u) /* 0 off, 1 GUS, 2 CMS, 3 Tandy */
#define PC_GUEST_TICKS (PC_VIDEO+980u) /* ARM-owned progress for wall-time cycle control */
#define PC_PAGED_JIT (PC_OPTIONS+40u)
#define PC_KEY_WRITE (PC_VIDEO+320u)
#define PC_KEY_READ (PC_VIDEO+384u)
#define PC_KEY_DATA (PC_VIDEO+448u)
#define PC_KEY_SIZE 64u
#define PC_CONSOLE_WAIT (PC_VIDEO+512u)

#define PC_BOOT_NAME (PC_VIDEO+576u) /* 13-byte 8.3 name; empty = embedded DOS. */
#define PC_HDD_NAME (PC_VIDEO+640u) /* 13-byte 8.3 name; 32–256 MiB HDD. */
#define PC_BOOT_TEXT 0x189000u /* Final colour text page, then completion magic. */
#if PC_PRESENT_REPORT+PC_PRESENT_SIZE > PC_BOOT_TEXT
#error Presentation report overlaps boot text
#endif

#define PC_UI_FLAGS (PC_VIDEO+704u) /* bit 0: FPS; bit 1: VGA shell */
#define PC_FPS_TENTHS (PC_VIDEO+708u) /* ARM rolling completed-frame rate */
#define PC_TEXT_COLOURS (PC_VIDEO+712u) /* VGA foreground/background attribute */
#define PC_BOOT_COMMAND (PC_VIDEO+768u) /* 160-byte, NUL-terminated DOS command */

#define PC_CYCLE_REQUEST (PC_VIDEO+964u) /* Host request, 500..10000 cycles/ms. */
#define PC_CYCLE_APPLIED (PC_VIDEO+968u) /* ARM acknowledgement at tick boundary. */

/* Mouse events: independent producer/consumer cache lines, 128 x 16 bytes.
 * x/y normalized 0..65535, buttons bits 0..2, position-valid 0/1.
 * A release without position is used for focus and display transitions. */
#define PC_MOUSE_WRITE (PC_VIDEO+1024u)
#define PC_MOUSE_READ (PC_VIDEO+1088u)
#define PC_MOUSE_DATA (PC_VIDEO+1152u)
#define PC_MOUSE_SIZE 128u
#define PC_MOUSE_STRIDE 16u
#if PC_MOUSE_DATA+PC_MOUSE_SIZE*PC_MOUSE_STRIDE > PC_BOOT_TEXT
#error Mouse queue overlaps boot text
#endif
