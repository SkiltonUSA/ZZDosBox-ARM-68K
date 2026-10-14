#!/usr/bin/env python3
"""Adapt pinned DOSBox disassembler to bounded snapshots, never live memory."""
import hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def prepare(out):
    vendor=ROOT/'amiga/dosbox/developer/vendor'
    raw=(vendor/'debug_disasm.cpp').read_bytes()
    assert hashlib.sha256(raw).hexdigest()==json.loads((vendor/'upstream.json').read_text())['sha256']
    s=raw.decode()
    s=s.replace('#include "dosbox.h"', '''#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include "disasm.h"
#define C_DEBUG 1
#define INLINE static inline
typedef uint8_t Bit8u; typedef uint16_t Bit16u; typedef uint32_t Bit32u;
typedef int8_t Bit8s; typedef int16_t Bit16s; typedef int32_t Bit32s;
typedef uint32_t Bitu; typedef uint32_t PhysPt;
static const uint8_t *snapshot;
static unsigned snapshot_size;
static char *output_end;
static jmp_buf decode_failed;
''').replace('#include "mem.h"','')
    old='return mem_readb(getbyte_mac++);'
    assert old in s
    s=s.replace(old,'if(getbyte_mac>=snapshot_size || getbyte_mac>=15)longjmp(decode_failed,1);\n return snapshot[getbyte_mac++];')
    s=s.replace('vsprintf(ubufp, s, arg_ptr);','int written=vsnprintf(ubufp,(size_t)(output_end-ubufp),s,arg_ptr);')
    s=s.replace('va_end(arg_ptr);','va_end(arg_ptr);\n if(written<0 || (size_t)written>=(size_t)(output_end-ubufp))longjmp(decode_failed,1);')
    s=s.replace('  *ubufp++ = c;','  if(ubufp+1>=output_end)longjmp(decode_failed,1);\n  *ubufp++ = c;')
    # Some upstream templates end with a single-character token (e.g. %w).
    # Do not advance beyond the terminating NUL when no subtype is present.
    s=s.replace('percent(c, *str++);','percent(c, *str);\n      if(*str)str++;')
    s=s.replace('sprintf(buffer,','snprintf(buffer, sizeof(buffer),')
    s=s.replace('"ecsdfg"[REG(modrm())]', '(REG(modrm())<6?"ecsdfg"[REG(modrm())]:\'?\')')
    s+='''
unsigned dm_disassemble(char *out,size_t capacity,const uint8_t *bytes,size_t available,uint32_t ip,unsigned bits){
 if(!out||capacity<16)return 0;
 out[0]=0;if(!bytes||!available||(bits!=16&&bits!=32))return 0;
 snapshot=bytes;snapshot_size=available>15?15:(unsigned)available;output_end=out+capacity;
 if(setjmp(decode_failed)){snprintf(out,capacity,"db %02X",bytes[0]);return 1;}
 return DasmI386(out,0,ip,bits==32);
}
'''
    out=Path(out);out.mkdir(parents=True,exist_ok=True)
    p=out/'disasm.c';p.write_text(s);return p
if __name__=='__main__':
    import sys
    print(prepare(sys.argv[1]))
