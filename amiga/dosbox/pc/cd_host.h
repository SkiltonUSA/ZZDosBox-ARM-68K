/* GPL-3.0-or-later. Read-only ISO or mounted Amiga CD block source. */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
uint32_t db_cd_host_open(unsigned mode,const char *path);
int db_cd_host_read(uint32_t sector,unsigned count,void *data);
void db_cd_host_close(void);
#ifdef __cplusplus
}
#endif
