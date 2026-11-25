/* GPL-2.0-or-later. Paused real-mode CPU + portable x87 only, not a PC state. */
#pragma once
#include "cpu_checkpoint.h"
#include "fpu_checkpoint.h"
enum { db_processor_checkpoint_bytes=db_cpu_checkpoint_bytes+db_fpu_checkpoint_bytes };
bool db_processor_checkpoint_save(void *buffer,size_t size);
bool db_processor_checkpoint_load(const void *buffer,size_t size);
