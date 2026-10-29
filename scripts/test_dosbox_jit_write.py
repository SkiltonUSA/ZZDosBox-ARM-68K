"""Run real x86 writes through linked ARM dynrec; include SMC and paging cases."""
import argparse,importlib.util,subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('build',type=Path);a=p.parse_args();out=a.build.resolve()
spec=importlib.util.spec_from_file_location('model',root/'tests/amiga/dosbox/check_pc_arm.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);m.OUT=out
subprocess.run(['clang','--target=i386-none-elf','-c',str(root/'amiga/dosbox/pc/jitwrite.S'),'-o',str(out/'jitwrite.o')],check=True)
subprocess.run(['/opt/homebrew/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(out/'jitwrite.o'),'-o',str(out/'JITWRITE.COM')],check=True)
for base in (0x12000040,0x150020c0):
 for name,text in [('JITWRITE','JIT checked writes PASS'),('SMCTEST','JIT self-modifying guest code PASS'),('JITREAD','JIT checked reads PASS')]:
  m.run(base,'fixture',True,options=(8,0,10000),hardware={'cpu':1},fixture=(name,text))
for mode in ('paging','high-bank-bound'):
 m.run(0x12000040,mode,True,options=(8,0,10000),hardware={'cpu':1})
