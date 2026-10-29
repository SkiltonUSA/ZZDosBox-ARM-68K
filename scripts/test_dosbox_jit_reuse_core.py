#!/usr/bin/env python3
"""Run mixed-width register/flag cases through the actual linked ARM dynrec."""
import argparse, importlib.util, subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('build',type=Path);a=p.parse_args();out=a.build.resolve()
spec=importlib.util.spec_from_file_location('model',root/'tests/amiga/dosbox/check_pc_arm.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);m.OUT=out
subprocess.run(['clang','--target=i386-none-elf','-c',str(root/'amiga/dosbox/pc/jitspan.S'),'-o',str(out/'jitspan.o')],check=True)
subprocess.run(['/opt/homebrew/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(out/'jitspan.o'),'-o',str(out/'JITSPAN.COM')],check=True)
for base in (0x12000040,0x150020c0):
 for name,text in [('JITREUSE','JIT register reuse PASS'),('JITSPAN','JIT bounded reuse PASS'),('SMCTEST','JIT self-modifying guest code PASS')]:
  m.run(base,'fixture',True,options=(8,0,10000),hardware={'cpu':1},fixture=(name,text))
