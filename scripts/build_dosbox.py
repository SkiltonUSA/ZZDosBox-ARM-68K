#!/usr/bin/env python3
"""Prepare a pinned DOSBox interpreter diagnostic; never deploys hardware."""
from pathlib import Path
import argparse,hashlib,json,shutil,subprocess,difflib
ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/'amiga/dosbox';UP=ROOT/'.context/research/dosbox-libretro';OUT=ROOT/'.context/amiga/dosbox'
UNITS=['cpu','core_normal','flags','modrm','paging','callback']
def prepare():
 pin=json.loads((SRC/'upstream.json').read_text())
 if not UP.exists():
  subprocess.run(['git','clone',pin['repository'],str(UP)],check=True)
  subprocess.run(['git','-C',str(UP),'checkout','--detach',pin['revision']],check=True)
 assert subprocess.check_output(['git','-C',str(UP),'rev-parse','HEAD'],text=True).strip()==pin['revision']
 assert not subprocess.check_output(['git','-C',str(UP),'status','--porcelain'],text=True).strip(),'Upstream checkout modified'
 OUT.mkdir(parents=True,exist_ok=True)
 for name in ['include','src/cpu']:
  shutil.copytree(UP/name,OUT/name,dirs_exist_ok=True)
 patches=[]
 def change(rel,fn):
  p=OUT/rel;old=p.read_text();new=fn(old);assert new!=old,rel;p.write_text(new)
  patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 change('include/config.h',lambda s:s.replace('#define C_FPU 1','#define C_FPU 0'))
 def cpu(s):
  s=s[:s.index('class CPU: public Module_base')]
  for h in ['sstream','mapper.h','setup.h','programs.h','support.h']:
   s=s.replace('#include <'+h+'>','').replace('#include "'+h+'"','')
  return s
 change('src/cpu/cpu.cpp',cpu)
 change('src/cpu/paging.cpp',lambda s:s[:s.index('class PAGING:public Module_base')].replace('#include "setup.h"','')+'\nvoid zz_dosbox_paging_init(){paging.enabled=false; PAGING_InitTLB(); for(Bitu i=0;i<LINK_START;i++)paging.firstmb[i]=i;pf_queue.used=0;}\n')
 (OUT/'integration.patch').write_text(''.join(patches))
 subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/'diagnostic.S'),'-o',str(OUT/'diagnostic.o')],check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(OUT/'diagnostic.o'),'-o',str(OUT/'Diagnostic.com')],check=True)
 data=(OUT/'Diagnostic.com').read_bytes()
 (OUT/'fixture.h').write_text('static const uint8_t db_fixture[]={'+','.join(map(str,data))+'};\n')
 shutil.copyfile(UP/'COPYING',OUT/'COPYING')
 memory=(UP/'src/hardware/memory.cpp').read_text()
 (OUT/'memory.cpp').write_text(memory[:memory.index('#include')]+ '\n#include \"paging.h\"\n'+memory[memory.index('Bit16u mem_unalignedreadw('):memory.index('static void write_p92')])
 return [OUT/'src/cpu'/f'{n}.cpp' for n in UNITS]+[OUT/'memory.cpp']
def main():
 p=argparse.ArgumentParser();p.add_argument('--prepare-only',action='store_true');p.add_argument('--sanitize',action='store_true');args=p.parse_args()
 units=prepare()
 if args.prepare_only:return
 flags=['-std=c++17','-O2','-g','-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-I'+str(OUT/'include'),'-I'+str(SRC),'-I'+str(OUT),'-I'+str(OUT/'src/cpu'),'-Wno-format']
 if args.sanitize:flags+=['-fsanitize=address','-fno-omit-frame-pointer']
 subprocess.run(['clang++',*flags,*map(str,units),str(SRC/'adapter.cpp'),str(SRC/'native.cpp'),'-Wl,-dead_strip','-o',str(OUT/'dosbox-test')],check=True)
 subprocess.run([str(OUT/'dosbox-test')],check=True)
if __name__=='__main__':main()
