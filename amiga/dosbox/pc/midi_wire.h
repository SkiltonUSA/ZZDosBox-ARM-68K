/* GPL-3.0-or-later. Bounded Core1 -> CAMD MIDI byte stream. */
#pragma once
#include "pc_wire.h"
#define PC_MIDI_MODE (PC_VIDEO+960u) /* 0 off, 1 MT-32 intelligent, 2 UART */
#define PC_MIDI_WRITE 0x18a000u
#define PC_MIDI_READ  0x18a040u
#define PC_MIDI_DATA  0x18b000u
#define PC_MIDI_SIZE 4096u
#if PC_MIDI_DATA+PC_MIDI_SIZE > DB_PROGRAM
#error MIDI overlaps program storage
#endif
