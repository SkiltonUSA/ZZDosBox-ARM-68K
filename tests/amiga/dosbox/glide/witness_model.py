#!/usr/bin/env python3
"""Exercise compiled read-only worker protocol in Unicorn, not GPU timing.
The model supplies Core1 MPIDR because Unicorn exposes a fixed Core0 ID.
Run build_dosbox_glide_witness.py first. Requires Unicorn and ARM binutils.
"""
from pathlib import Path
import struct,subprocess
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE
from unicorn.arm_const import UC_CPU_ARM_CORTEX_A9,UC_ARM_REG_R0,UC_ARM_REG_PC,UC_ARM_REG_SP,UC_ARM_REG_LR
ROOT=Path(__file__).resolve().parents[4];OUT=ROOT/'.context/amiga/glide/witness'
subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-Ttext=0x100000','-e','zz_worker',str(OUT/'witness_worker.o'),'-o',str(OUT/'model.elf')],check=True)
subprocess.run(['arm-none-eabi-objcopy','-O','binary',str(OUT/'model.elf'),str(OUT/'model.bin')],check=True)
BASE=0x200000;CMD=BASE+0x8800;GO=BASE+0x8880;REPLY=BASE+0x8900;GPU=0x6610000
for case in ('valid','corrupt','idle','bad-nonce','bad-range','cancel','reverse','reverse-corrupt','bad-order','sample','sample-corrupt','bad-sample'):
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 u.mem_map(0x100000,0x200000);u.mem_write(0x100000,(OUT/'model.bin').read_bytes())
 u.mem_map(GPU,0x10000);u.mem_map(0xf8f00000,0x1000)
 u.cpr_write(15,0,1,0,0,0,False,0x1000)
 u.mem_write(0xf8f00208,struct.pack('<I',1))
 def put(addr,v):u.mem_write(addr,struct.pack('>I',v))
 def get(addr):return struct.unpack('>I',u.mem_read(addr,4))[0]
 put(BASE+0x8000,0x12345678)
 for i,v in enumerate((1,0x12345679 if case!='bad-nonce' else 9,0,0x4120,4096 if case!='bad-range' else 999999,64,case=='idle',2 if case=='bad-order' else int(case.startswith('reverse')),2 if case=='bad-sample' else int(case.startswith('sample')))):put(CMD+4*i,v)
 reads=[];writes=[];clock=[0]
 def code(uc,addr,size,data):
  word=struct.unpack('<I',uc.mem_read(addr,4))[0]
  if word&0xffff0fff==0xee100fb0:
   uc.reg_write(UC_ARM_REG_R0+((word>>12)&15),0x80000001)
   uc.reg_write(UC_ARM_REG_PC,addr+4)
 def read(uc,access,addr,size,value,data):
  if GPU<=addr<GPU+0x10000:reads.append(addr)
  if addr==0xf8f00200:
   clock[0]+=100;uc.mem_write(addr,struct.pack('<I',clock[0]))
 def write(uc,access,addr,size,value,data):
  if GPU<=addr<GPU+0x10000:writes.append(addr)
  if addr==REPLY+4:uc.emu_stop()
 u.hook_add(UC_HOOK_CODE,code);u.hook_add(UC_HOOK_MEM_READ,read);u.hook_add(UC_HOOK_MEM_WRITE,write)
 u.reg_write(UC_ARM_REG_R0,BASE);u.reg_write(UC_ARM_REG_SP,0x2ff000);u.reg_write(UC_ARM_REG_LR,0x2fe000)
 u.emu_start(0x100000,0x2fe000,count=50000)
 assert get(BASE+0x8040)==0x41524d31 and not reads and not writes,case
 if case.startswith('bad-'):
  assert u.reg_read(UC_ARM_REG_PC)==0x2fe000 and get(REPLY)==0,case
 else:
  assert get(REPLY)==1,case
  if case=='cancel':
   put(BASE+0x8080,1)
   u.emu_start(u.reg_read(UC_ARM_REG_PC),0x2fe000,count=1000)
   assert u.reg_read(UC_ARM_REG_PC)==0x2fe000 and not reads,case
  else:
   u.mem_write(GPU+8192,struct.pack('<64H',*([0x4120]*64)))
   if case in ('corrupt','reverse-corrupt'):u.mem_write(GPU+24,struct.pack('<H',7))
   if case=='reverse-corrupt':u.mem_write(GPU+8190,struct.pack('<H',8))
   if case=='sample-corrupt':u.mem_write(GPU+3232,struct.pack('<H',0x1234))
   put(GO,1);u.emu_start(u.reg_read(UC_ARM_REG_PC),0x2fe000,count=500000)
   assert get(REPLY+4)==1 and get(REPLY+8)==0 and get(REPLY+12)==(2 if case=='reverse-corrupt' else int(case in ('corrupt','sample-corrupt'))),case
   assert get(REPLY+16)==(0 if case=='idle' else 4096) and not writes,case
   if case.startswith('sample'):
    assert reads[64:68]==[GPU+2*n for n in (1616,2704,3344,3368)],case
    assert [get(REPLY+44+4*k) for k in range(4)]==[0x1234 if case=='sample-corrupt' else 0,0,0,0],case
   else:assert all(get(REPLY+44+4*k)==0xffffffff for k in range(4)),case
   if case=='idle':assert not reads
   if case.startswith('reverse'):assert reads[64]==GPU+8190
   if case=='reverse-corrupt':assert get(REPLY+28)==4095 and get(REPLY+32)==8
 print('PASS witness instruction model:',case)
