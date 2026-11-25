/* GPL-2.0-or-later. Development-only integer CPU checkpoint component.
 * Normal, real-mode interpreter at a returned instruction boundary only.
 * This is NOT a PC/game save state: RAM, devices, DOS, files, x87 and scheduler
 * still need coordinated serializers. Not linked into production players. */
#pragma once
#include <stddef.h>
#include <stdint.h>
enum { db_cpu_checkpoint_bytes=70*4 };
bool db_cpu_checkpoint_save(void *buffer,size_t size);
bool db_cpu_checkpoint_validate(const void *buffer,size_t size);
bool db_cpu_checkpoint_load(const void *buffer,size_t size);
