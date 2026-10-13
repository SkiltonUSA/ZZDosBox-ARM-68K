#pragma once
#include "dos_inc.h"
void db_mount(const uint8_t *,unsigned);
void db_unmount();
void db_stdio(DOS_PSP &);
void db_console(const uint8_t *,unsigned);
