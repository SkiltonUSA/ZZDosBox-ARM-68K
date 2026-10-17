#!/usr/bin/env python3
"""Expanded capability checks in the ARM instruction model, not hardware proof."""
import argparse, importlib.util, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('build',type=Path);p.add_argument('--paging-only',action='store_true');a=p.parse_args()
s=importlib.util.spec_from_file_location('pc_check',ROOT/'tests/amiga/dosbox/check_pc_arm.py')
m=importlib.util.module_from_spec(s);s.loader.exec_module(m);m.OUT=a.build.resolve()
if not a.paging_only:
 for cpu,label in enumerate(('386','486','Pentium')):
  m.run(0x12000040,'capability',options=(8,0,3000),hardware={'cpu':cpu},fixture=('CPUTEST','CPU '+label+' PASS'))
 m.run(0x150020c0,'capability',options=(32,0,3000),hardware={'services':3},fixture=('EXTTEST','Extended RAM VCPI UMB PASS'))
 m.run(0x12000040,'capability',options=(8,0,3000),hardware={'joystick':1,'joy_state':25},fixture=('JOYTEST','Gameport PASS'))
 for sound,fixture in enumerate(('GUSTEST','CMSTEST','TANDTEST'),1):
  m.run(0x150020c0,'capability',options=(8,0,3000),hardware={'sound':sound},fixture=(fixture,'Extra sound PASS'))
out=ROOT/'.context/amiga/dosbox-expansion/paging';out.mkdir(parents=True,exist_ok=True)
for name,faults in [('paging_alias',0),('paging_fetch',1),('paging_rmw',1),('paging_nested',2),('paging_privilege',1)]:
 obj=out/(name+'.o');binary=out/(name+'.bin')
 subprocess.run(['clang','--target=i386-none-elf','-c',str(ROOT/'tests/amiga/dosbox'/(name+'.S')),'-o',str(obj)],check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x7c00','--oformat=binary',str(obj),'-o',str(binary)],check=True)
 assert len(binary.read_bytes())==(1024 if name=='paging_privilege' else 512)
 for base in (0x12000040,0x150020c0):
  if name=='paging_privilege' and base==0x150020c0:
   binary=out/(name+'-486.bin')
   subprocess.run(['clang','--target=i386-none-elf','-DTEST_486','-c',str(ROOT/'tests/amiga/dosbox'/(name+'.S')),'-o',str(obj)],check=True)
   subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x7c00','--oformat=binary',str(obj),'-o',str(binary)],check=True)
  m.run(base,'paging',options=(8,0,3000),hardware={'paged_jit':1,'cpu':1 if name=='paging_privilege' and base==0x150020c0 else 0},paging_fixture=binary,paging_faults=faults)
