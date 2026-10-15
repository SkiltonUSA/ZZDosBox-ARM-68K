#!/usr/bin/env python3
"""Exercise paged aliases and instruction-fetch faults in the ARM instruction model.
Requires a completed PC build and Unicorn. The normal paged-JIT guard is enforced.
No hardware deployment, commercial images, or production guard changes.
"""
import argparse, importlib.util, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('build',type=Path);a=p.parse_args()
out=ROOT/'.context/amiga/dosbox-paging-cases';out.mkdir(parents=True,exist_ok=True)
spec=importlib.util.spec_from_file_location('check_pc',ROOT/'tests/amiga/dosbox/check_pc_arm.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);m.OUT=a.build.resolve()
for name,faults in [('paging_alias',0),('paging_fetch',1),('paging_rmw',1),('paging_nested',2)]:
 obj=out/(name+'.o');binary=out/(name+'.bin')
 subprocess.run(['clang','--target=i386-none-elf','-c',str(ROOT/'tests/amiga/dosbox'/ (name+'.S')),'-o',str(obj)],check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x7c00','--oformat=binary',str(obj),'-o',str(binary)],check=True)
 assert len(binary.read_bytes())==512
 m.run(0x12000040,'paging',options=(8,0,3000),paging_fixture=binary,paging_faults=faults)
