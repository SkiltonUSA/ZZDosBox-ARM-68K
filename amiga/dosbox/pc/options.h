#include "adapters.h"
#include "capabilities.h"
/* GPL-2.0-or-later. Explicit PC settings; zeroed legacy mailboxes use defaults. */
#pragma once
#include <stdint.h>
#define PC_OPTIONS_MAGIC 0x50434f31u
static inline int pc_options_valid(unsigned mem,unsigned machine,unsigned cycles){return db_memory_valid(mem,DB_GUEST_MAX_MB)&&machine<PC_ADAPTER_COUNT&&cycles>=500&&cycles<=DB_CYCLES_MAX;}
