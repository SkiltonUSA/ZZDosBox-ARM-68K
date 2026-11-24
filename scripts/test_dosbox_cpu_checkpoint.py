#!/usr/bin/env python3
"""Test the development-only real-mode CPU component; not a PC save state."""
import argparse, subprocess
from pathlib import Path
import build_dosbox as b
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,default=b.ROOT/'.context/amiga/dosbox-state-cpu')
b.OUT=parser.parse_args().output.resolve()
units=b.prepare()
output=b.OUT/'state-cpu-test'
subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined',
 '-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-Wno-format',
 *['-I'+str(p) for p in (b.OUT/'include',b.SRC,b.OUT,b.OUT/'src/cpu')],
 *map(str,units),str(b.SRC/'state/cpu_checkpoint.cpp'),
 str(b.ROOT/'tests/amiga/dosbox/state_cpu.cpp'),'-Wl,-dead_strip','-o',str(output)],check=True)
subprocess.run([str(output)],check=True)
# This file is only the diagnostic CPU/RAM fixture, not a game-state format.
checkpoint=b.OUT/'cpu-ram-fixture.bin'
subprocess.run([str(output),'save',str(checkpoint)],check=True)
subprocess.run([str(output),'load',str(checkpoint)],check=True)
