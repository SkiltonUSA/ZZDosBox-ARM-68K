/* GPL-2.0-or-later. Tick-driven audio; guest DMA/IRQ timing runs on Core1. */
#pragma once
#include <stdint.h>
extern bool db_audio_enabled,db_pc_speaker;
extern unsigned db_sound_card,db_extra_sound;
void db_audio_open();
void db_audio_close();
void db_audio_flush();
void db_audio_samples(const int16_t*,unsigned);
