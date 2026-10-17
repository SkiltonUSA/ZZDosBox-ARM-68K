/* GPL-3.0-or-later. Batched AHI worker with bounded PCM queue. */
#pragma once
#include "audio_wire.h"
int db_audio_host_open(volatile uint8_t*,const struct ad_io*,unsigned);
int db_audio_host_service(void);
int db_audio_host_pending(void);
void db_audio_host_close(void);
