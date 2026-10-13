/* GPL-2.0-or-later. Bounded private DOS drive, copied in/out by the host. */
#pragma once
#include <stdint.h>
#define DB_FILE_COUNT 8
#define DB_FILE_CAPACITY 65536
struct DbDiskFile {char name[13];uint32_t size;uint32_t readonly;uint8_t data[DB_FILE_CAPACITY];};
extern DbDiskFile db_files[DB_FILE_COUNT];
extern "C" void db_files_reset();
extern "C" int db_file_put(const char *name,const uint8_t *data,unsigned size,int readonly);
extern "C" int db_file_get(const char *name,const uint8_t **data,unsigned *size);
