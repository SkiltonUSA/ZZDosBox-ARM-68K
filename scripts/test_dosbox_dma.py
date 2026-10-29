#!/usr/bin/env python3
"""Native PL330 program-model checks. Does not execute a DMA controller."""
from pathlib import Path
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='zzdosbox-dma-') as temporary:
    exe=Path(temporary)/'test'
    subprocess.run(['c++','-std=c++17','-O1','-Wall','-Wextra','-Werror',
                    '-fsanitize=address,undefined','-I.',
                    'tests/amiga/dosbox_dma.cpp','-o',str(exe)],cwd=ROOT,check=True)
    subprocess.run([str(exe)],cwd=ROOT,check=True)
    # Compile the same builder as freestanding ARM C, with no C++ runtime.
    source=Path(temporary)/'arm.c'
    source.write_text('''#include "amiga/dosbox/dma/copy_program.h"
int build(struct pc_dma_program *p, struct pc_dma_span a, struct pc_dma_span b,
          unsigned src,unsigned dst,unsigned n,unsigned ns,unsigned swap) {
 return pc_dma_build_copy(p,a,b,src,dst,n,ns,swap);
}
''')
    subprocess.run(['clang','--target=arm-none-eabi','-mcpu=cortex-a9','-marm',
                    '-mfloat-abi=soft','-ffreestanding','-std=c99','-O2',
                    '-Wall','-Wextra','-Werror','-I.','-c',str(source),
                    '-o',str(Path(temporary)/'arm.o')],cwd=ROOT,check=True)
    print('PASS DMA builder: freestanding Cortex-A9 C compile')
