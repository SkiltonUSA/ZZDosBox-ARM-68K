#!/usr/bin/env python3
"""Native sanitizer checks of the production diagnostic counter/reset/report."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as directory:
    tmp=Path(directory)
    (tmp/'dosbox.h').write_text('#pragma once\n#include <stdint.h>\n#include <stdlib.h>\n#define E_Exit(message) abort()\n')
    (tmp/'mem.h').write_text('#pragma once\nextern unsigned char *MemBase;\n')
    (tmp/'check.cpp').write_text(r'''
#include <cassert>
#include <cstring>
#include "pc_wire.h"
#include "jit_hot.h"
unsigned char memory[8*1048576]; unsigned char *MemBase=memory;
unsigned db_memory_mb=8;
unsigned char shared[PC_BOOT_TEXT+4096];
extern "C" volatile uint8_t *db_shared(){return shared;}
extern unsigned *db_jit_hot_counter(unsigned);
int main(){
 const unsigned o=PC_VIDEO+0xe60;
 auto a=db_jit_hot_counter(0x1000),b=db_jit_hot_counter(0x2000);
 *a=900;*b=700;db_jit_hot_report();assert(ad_get(shared,o+400)==0);
 db_jit_hot_reset();assert(!*a&&! *b);*b=9;*a=4;
 memset(memory+0x2000,0xa5,16);db_jit_hot_report();
 assert(ad_get(shared,o+400)==1&&ad_get(shared,o+12)==13);
 assert(ad_get(shared,o+16)==0x2000&&ad_get(shared,o+20)==9&&ad_get(shared,o+24)==0xa5a5a5a5);
 assert(ad_get(shared,o+40)==0x1000&&ad_get(shared,o+44)==4);
 assert(a==db_jit_hot_counter(0x1000));
 for(unsigned i=0;i<16382;i++)db_jit_hot_counter(0x100000+i);
 assert(db_jit_hot_counter(0x1000)==a);
 (*db_jit_hot_counter(0x900000))++;db_jit_hot_report();
 assert(ad_get(shared,o+4)==16384&&ad_get(shared,o+8)==1);
 db_jit_hot_reset();db_jit_hot_report();assert(ad_get(shared,o+12)==0);
}
''')
    includes=[tmp,root/'amiga/dosbox/pc',root/'amiga/dosbox/console',root/'amiga/dosbox/dos',root/'amiga/common/arm',root/'amiga/arm_debug',root/'amiga/arm_debug/zz9000']
    command=['clang++','-std=c++17','-O2','-fsanitize=address,undefined',*[f'-I{p}' for p in includes],str(tmp/'check.cpp'),str(root/'amiga/dosbox/pc/jit_hot.cpp'),'-o',str(tmp/'check')]
    subprocess.run(command,check=True);subprocess.run([str(tmp/'check')],check=True)
print('PASS JIT hot profile: gameplay reset, ranking, repeated keys, saturation and report bounds')
