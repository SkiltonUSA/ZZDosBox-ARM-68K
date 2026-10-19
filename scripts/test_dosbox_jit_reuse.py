#!/usr/bin/env python3
"""Differential A9 execution of CPU-register store-forwarding transformations."""
import ctypes,random,struct,subprocess,tempfile
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
from unicorn.arm_const import *
root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as directory:
 p=Path(directory);(p/'bridge.c').write_text('#include "jit_reuse.h"\nint forward(unsigned a,unsigned b){return db_arm_stored_register(a,b);}\n')
 subprocess.run(['clang','-shared','-O2','-I'+str(root/'amiga/dosbox/pc'),str(p/'bridge.c'),'-o',str(p/'bridge.dylib')],check=True)
 lib=ctypes.CDLL(str(p/'bridge.dylib'));f=lib.forward;f.argtypes=[ctypes.c_uint32,ctypes.c_uint32];f.restype=ctypes.c_int
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 u.mem_map(0x100000,4096);u.mem_map(0x300000,16384)
 rng=random.Random(1931);cases=0
 def run(code,values,flags):
  u.mem_write(0x300000,bytes(16384));u.mem_write(0x100000,struct.pack('<'+'I'*len(code),*code));u.ctl_remove_cache(0x100000,0x100100)
  for i,value in enumerate(values):u.reg_write(UC_ARM_REG_R0+i,value)
  u.reg_write(UC_ARM_REG_R10,0x301000);u.reg_write(UC_ARM_REG_CPSR,flags|0x13)
  u.emu_start(0x100000,0x100000+len(code)*4)
  return [u.reg_read(UC_ARM_REG_R0+i) for i in range(10)],u.reg_read(UC_ARM_REG_CPSR),bytes(u.mem_read(0x300000,16384))
 for sign in (0,0x800000):
  for src in range(4):
   for dst in range(4):
    for offset in (0,1,2,3,4,127,255,1024,4092,4095):
     store=0xe50a0000|sign|src<<12|offset;load=0xe51a0000|sign|dst<<12|offset
     assert f(store,load)==src
     values=[rng.getrandbits(32)for _ in range(10)];flags=rng.randrange(16)<<28
     replacement=[] if src==dst else [0xe1a00000|dst<<12|src]
     assert run([store,load],values,flags)==run([store,*replacement],values,flags)
     cases+=1
 for bad in (0x04000000,0x00400000,0x00200000,0x01000000,0x10000000,0x00010000,1):
  assert f(0xe58a0000,0xe59a0000^bad)==-1
  assert f(0xe58a0000^bad,0xe59a0000)==-1
 assert f(0xe58a4000,0xe59a0000)==-1 and f(0xe58a0000,0xe59a4000)==-1
print(f'PASS ARM store forwarding: {cases} differential cases, offsets, registers, flags and rejected encodings')
