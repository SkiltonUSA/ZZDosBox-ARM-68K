/* GPL-2.0-or-later. Development-only portable x87 component, captured between
 * guest instructions. Not a complete PC state or an 80-bit precision upgrade. */
#pragma once
#include <stddef.h>
enum { db_fpu_checkpoint_bytes=35*4 };
bool db_fpu_checkpoint_save(void *buffer,size_t size);
bool db_fpu_checkpoint_validate(const void *buffer,size_t size);
bool db_fpu_checkpoint_load(const void *buffer,size_t size);
