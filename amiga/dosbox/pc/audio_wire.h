/* GPL-3.0-or-later. PCM SPSC queue in application-owned uncached DDR. */
#pragma once
#include "pc_wire.h"
#define PC_AUDIO_CONTROL 0x198000u
#define PC_AUDIO_MODE PC_AUDIO_CONTROL /* low 2 bits: 0 off, 1 SBPro2, 2 SB16; bit 2: PC speaker */
#define PC_AUDIO_EMS (PC_AUDIO_CONTROL+4u)
#define PC_AUDIO_WRITE (PC_AUDIO_CONTROL+64u)
#define PC_AUDIO_READ (PC_AUDIO_CONTROL+128u)
#define PC_AUDIO_FRAMES 1024u
#define PC_AUDIO_SLOTS 4u
#define PC_AUDIO_STRIDE (PC_AUDIO_FRAMES*4u)
#define PC_AUDIO_DATA 0x1a0000u
#if PC_AUDIO_DATA+PC_AUDIO_SLOTS*PC_AUDIO_STRIDE > 0x1c0000u
#error PCM exceeds shared DDR range
#endif
