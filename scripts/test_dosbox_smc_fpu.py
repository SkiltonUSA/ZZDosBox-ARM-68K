#!/usr/bin/env python3
"""Page-crossing aliased SMC and x87 rounding/state on the recorded ARM player."""
import argparse,importlib.util,shutil,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('build',type=Path);p.add_argument('--output',type=Path,default=Path('.context/smc-fpu-test'));a=p.parse_args()
out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
assert out!=a.build.resolve(),'Keep recorded build inputs unchanged'
for name in ['dosbox.elf','build.json','EGATEST.COM']:shutil.copy2(a.build/name,out/name)
spec=importlib.util.spec_from_file_location('model',ROOT/'tests/amiga/dosbox/check_pc_arm.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);m.OUT=out
for source,name,text in [('smc_alias','SMCALIAS','Page alias SMC PASS'),('x87_roundtrip','FPROUND','x87 rounding restore PASS')]:
 subprocess.run(['clang','--target=i386-none-elf','-c',str(ROOT/'tests/amiga/dosbox'/f'{source}.S'),'-o',str(out/(name+'.o'))],check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(out/(name+'.o')),'-o',str(out/(name+'.COM'))],check=True)
 for base,fpscr in [(0x12000040,0x03c09f00),(0x150020c0,0x01000000)]:
  print('CASE',name,hex(base),'inherited FPSCR',hex(fpscr),flush=True)
  m.run(base,'fixture',True,options=(8,0,10000),hardware={'cpu':1},fixture=(name,text),inherited_fpscr=fpscr,timer_hz=400000000)
print('PASS instruction-model compatibility; no physical timing claim')
