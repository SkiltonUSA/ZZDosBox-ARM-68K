/* GPL-3.0-or-later. Native asynchronous console window. */
#pragma once
#include <exec/types.h>
struct DbWindow;
struct DbWindow *db_window_open(void);
int db_window_closed(struct DbWindow *);
int db_window_read(struct DbWindow *); /* byte, -1: no input, -2: I/O error */
int db_window_write(struct DbWindow *,const UBYTE *,ULONG);
void db_window_close(struct DbWindow *);

struct DbWindow *db_window_open_colours(unsigned fg,unsigned bg);
