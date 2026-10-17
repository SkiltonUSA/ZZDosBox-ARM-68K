/* GPL-3.0-or-later. Nonblocking TCP transport for COM1. */
#pragma once
#include "wire.h"
#include "serial_config.h"
int db_serial_host_open(volatile uint8_t*,const struct ad_io*,unsigned,const char*,unsigned,const struct db_serial_config*);
int db_serial_host_poll(uint32_t);
void db_serial_host_close(void);
