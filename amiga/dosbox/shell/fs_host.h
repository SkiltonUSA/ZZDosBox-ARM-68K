/* GPL-3.0-or-later. 68k file service rooted in an application-owned drawer. */
#pragma once
#include "protocol.h"
int db_fs_open(volatile uint8_t *,const struct ad_io *,const char *);
int db_fs_service(void);
void db_fs_close(void);
