/* GPL-3.0-or-later. Snapshot-only x86 decoder, 16/32-bit instruction mode. */
#pragma once
#include <stddef.h>
#include <stdint.h>
unsigned dm_disassemble(char *out,size_t capacity,const uint8_t *bytes,size_t available,uint32_t ip,unsigned bits);
