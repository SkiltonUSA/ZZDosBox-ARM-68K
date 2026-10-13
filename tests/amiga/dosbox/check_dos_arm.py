#!/usr/bin/env python3
"""Execute the linked DOS kernel ARM worker in Unicorn; no cache/FPS claims."""
import importlib.util,struct,subprocess
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3];OUT=ROOT/'.context/amiga/dosbox-dos'
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
elf=OUT/'dosbox.elf'
def run(base,program,cancel=False):
 image,rel,_=pack.unpack(elf.read_bytes())
 for off in rel:struct.pack_into('<I',image,off,struct.unpack_from('<I',image,off)[0]+base)
 symbols={}
 for line in subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm','-n',str(elf)],text=True).splitlines():
  fields=line.split()
  if len(fields)==3:symbols[fields[2]]=int(fields[0],16)+base
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 u.mem_map(base&~4095,0x2002000);u.mem_write(base,bytes(image));u.mem_write(symbols['_ZL6shared'],struct.pack('<I',base))
 data=bytes.fromhex('ebfe') if cancel else (OUT/program).read_bytes();input=(OUT/'INPUT.TXT').read_bytes()
 u.mem_write(base+0x190000,data);u.mem_write(base+0x1a0000,input)
 u.mem_write(base+0x180800+12,struct.pack('>II',len(data),len(input)))
 u.mem_write(base+0x1d0000,bytes.fromhex('0000a0e31eff2fe1'));u.mem_write(base+0x1d0100,bytes.fromhex('0100a0e31eff2fe1'))
 calls=[];bad=[]
 def stopped(u,address,size,data):calls.append(1);u.reg_write(UC_ARM_REG_R0,int(cancel and len(calls)>=8))
 u.hook_add(UC_HOOK_CODE,stopped,begin=base+0x1d0004,end=base+0x1d0004)
 def access(u,kind,address,size,value,data):
  if not base<=address<=base+0x2000000-size or (size>1 and address%size):bad.append((hex(address),size,hex(u.reg_read(UC_ARM_REG_PC)-base)));u.emu_stop()
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access)
 for repeat in range(2):
  for reg,val in [(UC_ARM_REG_SP,base+0x1fffff0),(UC_ARM_REG_LR,base+0x1ff0000),(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,base+0x1e0000),(UC_ARM_REG_R2,base+0x1d0000),(UC_ARM_REG_R3,base+0x1d0100)]:u.reg_write(reg,val)
  calls.clear()
  try:u.emu_start(symbols['mesen_test'],base+0x1ff0000,count=200000000)
  except Exception:
   pc=u.reg_read(UC_ARM_REG_PC);print('FAULT PC',hex(pc-base),'bytes',bytes(u.mem_read(pc,12)).hex(),flush=True);raise
  assert not bad,bad
  assert u.reg_read(UC_ARM_REG_PC)==base+0x1ff0000,'Worker did not return'
  result=struct.unpack('<11I',u.mem_read(base+0x1e0000,44));rc=u.reg_read(UC_ARM_REG_R0)
  text=bytes(u.mem_read(base+0x180900+128,160)).split(b'\0')[0]
  if cancel:assert rc==2 and result[4]==3,(rc,result,text)
  else:
   assert rc==0 and result[4]==0 and result[7]==0,(rc,result,text)
   if program=='FileTest.com':assert result[3]==0xf23411c5 and result[9]==256 and bytes(u.mem_read(base+0x1b0000,256))==input,(result,text)
   else:assert text==b'ZZDOS MZ relocation/memory PASS\r\n',(result,text)
  print('PASS ARM DOS model',hex(base),program,'repeat',repeat,'cancel',cancel,'rc',rc,'text',text,flush=True)
for base in [0x12000040,0x150020c0]:
 for program in ['FileTest.com','ExeTest.exe']:run(base,program)
run(0x12000040,'loop',True)
