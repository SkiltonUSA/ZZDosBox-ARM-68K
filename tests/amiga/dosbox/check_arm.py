#!/usr/bin/env python3
"""Run the actual linked DOSBox ARM code in Unicorn, not physical hardware."""
import importlib.util,struct,subprocess
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3]
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
elf=ROOT/'.context/amiga/dosbox/dosbox.elf'
def run(base,cancel=False,invalid=False):
 image,rel,_=pack.unpack(elf.read_bytes())
 for off in rel:struct.pack_into('<I',image,off,struct.unpack_from('<I',image,off)[0]+base)
 symbols={}
 for line in subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm','-n',elf],text=True).splitlines():
  fields=line.split()
  if len(fields)==3:symbols[fields[2]]=int(fields[0],16)+base
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 u.mem_map(base&~4095,0x2002000);u.mem_write(base,bytes(image));u.mem_write(symbols['_ZL6shared'],struct.pack('<I',base))
 if invalid:
  fixture=(ROOT/'.context/amiga/dosbox/Diagnostic.com').read_bytes();offset=fixture.index(bytes.fromhex('b409cd21'))+1
  u.mem_write(symbols['_ZL10db_fixture']+offset,b'\xff')
 u.mem_write(base+0x1d0000,bytes.fromhex('0000a0e31eff2fe1'))
 u.mem_write(base+0x1d0100,bytes.fromhex('0100a0e31eff2fe1'))
 calls=[]
 def stopped(u,address,size,data):
  calls.append(1);u.reg_write(UC_ARM_REG_R0,int(cancel and len(calls)>=3))
 u.hook_add(UC_HOOK_CODE,stopped,begin=base+0x1d0004,end=base+0x1d0004)
 bad=[]
 def access(u,kind,address,size,value,data):
  if not base<=address<=base+0x2000000-size or (size>1 and address%size):
   bad.append((hex(address),size,hex(u.reg_read(UC_ARM_REG_PC)-base)));u.emu_stop()
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access)
 for repeat in range(2):
  for reg,val in [(UC_ARM_REG_SP,base+0x1fffff0),(UC_ARM_REG_LR,base+0x1ff0000),(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,base+0x1e0000),(UC_ARM_REG_R2,base+0x1d0000),(UC_ARM_REG_R3,base+0x1d0100)]:u.reg_write(reg,val)
  calls.clear()
  try:u.emu_start(symbols['mesen_test'],base+0x1ff0000,count=200000000)
  except Exception:
   pc=u.reg_read(UC_ARM_REG_PC);print('FAULT PC',hex(pc-base),'bytes',bytes(u.mem_read(pc,12)).hex(),flush=True);raise
  assert not bad,bad
  assert u.reg_read(UC_ARM_REG_PC)==base+0x1ff0000,'Diagnostic did not return'
  result=struct.unpack('<11I',u.mem_read(base+0x1e0000,44));rc=u.reg_read(UC_ARM_REG_R0)
  if invalid:assert rc==1 and result[4]==1,(rc,result)
  elif cancel:assert rc==2 and result[2]==2 and result[4]==3,(rc,result)
  else:assert rc==0 and result[:6]==(1,0,4,0xf23411c5,0,0) and result[7:9]==(0,34),(rc,result)
  print('PASS ARM instruction model',hex(base),'repeat',repeat,'cancel',cancel,'invalid',invalid,'result',result,flush=True)
run(0x12000040)
run(0x150020c0)
run(0x12000040,True)
run(0x12000040,invalid=True)
