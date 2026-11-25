#!/usr/bin/env python3
"""Resume the pinned interpreter and portable x87 backend, not a PC save state."""
import argparse, shutil, subprocess
from pathlib import Path
import build_dosbox as b

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--output',type=Path,default=b.ROOT/'.context/checkpoint-fpu')
b.OUT=p.parse_args().output.resolve()
units=b.prepare()
config=b.OUT/'include/config.h'
s=config.read_text().replace('#define C_FPU 0','#define C_FPU 1')
assert '/* #undef C_FPU_X86 */' in s
s=s.replace('/* #undef C_FPU_X86 */','#define C_FPU_X86 0')
config.write_text(s)
shutil.copytree(b.UP/'src/fpu',b.OUT/'src/fpu',dirs_exist_ok=True)
# Four rounding modes, nonempty stacks, arithmetic, memory conversion and the
# guest's FSAVE/FRSTOR path. The infinite final branch bounds native execution.
asm=['.code16','.global _start','_start:','finit']
for mode in range(4):
    asm += [f'fldcw {0x8000+mode*2}', 'fldl 0x8010', 'fldl 0x8018',
            'fxch %st(1)', f'fistpl {0x8100+mode*8}',
            f'fistpl {0x8104+mode*8}', 'fld1', 'fldpi', 'fmulp', 'fsqrt',
            'fsave 0x8200', 'frstor 0x8200', f'fstpl {0x8140+mode*8}']
asm += ['1: jmp 1b']
(b.OUT/'fpu-program.S').write_text('\n'.join(asm)+'\n')
subprocess.run(['clang','--target=i386-none-elf','-c',str(b.OUT/'fpu-program.S'),'-o',str(b.OUT/'fpu-program.o')],check=True)
subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(b.OUT/'fpu-program.o'),'-o',str(b.OUT/'fpu-program.bin')],check=True)
data=(b.OUT/'fpu-program.bin').read_bytes()
(b.OUT/'fpu-program.h').write_text('static const uint8_t program[]={'+','.join(map(str,data))+'};\n')
output=b.OUT/'state-fpu-test'
subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',
 '-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-Wno-format',
 *['-I'+str(p) for p in (b.OUT/'include',b.SRC,b.OUT,b.OUT/'src/cpu')],
 *map(str,units),str(b.OUT/'src/fpu/fpu.cpp'),
 str(b.SRC/'state/cpu_checkpoint.cpp'),str(b.SRC/'state/fpu_checkpoint.cpp'),
 str(b.SRC/'state/processor_checkpoint.cpp'),
 str(b.ROOT/'tests/amiga/dosbox/state_fpu.cpp'),'-Wl,-dead_strip','-o',str(output)],check=True)
subprocess.run([str(output)],check=True)
checkpoint=b.OUT/'cpu-fpu-ram-fixture.bin'
subprocess.run([str(output),'save',str(checkpoint)],check=True)
subprocess.run([str(output),'load',str(checkpoint)],check=True)
