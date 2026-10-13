/* GPL-3.0-or-later. Single completed-frame handoff in owned uncached DDR. */
#pragma once
#include "stream.h"
#define PC_VIDEO 0x188000u
#define PC_DURATION (PC_VIDEO+64u)
#define PC_DONE (PC_VIDEO+68u)
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
#define PC_KEY_WRITE (PC_VIDEO+320u)
#define PC_KEY_READ (PC_VIDEO+384u)
#define PC_KEY_DATA (PC_VIDEO+448u)
#define PC_KEY_SIZE 64u
#define PC_CONSOLE_WAIT (PC_VIDEO+512u)

#define PC_BOOT_NAME (PC_VIDEO+576u) /* 13-byte 8.3 name; empty = embedded DOS. */
#define PC_HDD_NAME (PC_VIDEO+640u) /* 13-byte 8.3 name; fixed 32 MiB HDD. */
#define PC_BOOT_TEXT 0x189000u /* Final colour text page, then completion magic. */
