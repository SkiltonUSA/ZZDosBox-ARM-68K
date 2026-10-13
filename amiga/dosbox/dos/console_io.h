/* GPL-2.0-or-later. Optional streaming console; callbacks live for one run. */
#pragma once
#include "adapter.h"
struct DbConsoleIO {
 void *opaque;
 int (*read)(void *); /* byte, -1: wait, -2: EOF */
 int (*ready)(void *);
 int (*write)(void *,const uint8_t *,unsigned); /* 1: OK, 0: error, -1: stop */
};
extern "C" int db_run_io(const uint8_t *,unsigned,unsigned,DosboxResult *,
                          int (*)(),const DbConsoleIO *);
unsigned db_console_read(uint8_t *,unsigned,bool);
int db_console_ready();
