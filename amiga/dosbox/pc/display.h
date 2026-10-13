/* GPL-3.0-or-later. Lifetime extends through synchronous Core1 reset. */
#pragma once
#include "protocol.h"
int pc_display_open(volatile uint8_t*,const struct ad_io*);
int pc_display_service(void);
int pc_display_locked(void);
int pc_display_pending(void);
void pc_display_pipeline(int enabled);
void ff_cleanup(void);

void pc_display_console(int waiting);
