/* GPL-3.0-or-later. Optional external MIDI output; no software synthesizer. */
#pragma once
#include "midi_wire.h"
int db_midi_host_open(volatile uint8_t*,const struct ad_io*,unsigned,const char*);
int db_midi_host_service(void);
void db_midi_host_close(void);
