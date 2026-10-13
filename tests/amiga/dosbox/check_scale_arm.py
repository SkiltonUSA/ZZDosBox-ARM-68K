#!/usr/bin/env python3
"""Check the production scaler compiled for A9 against an independent reference.

Instruction model only, not a physical performance benchmark.
"""
import subprocess,struct
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3]
OUT=ROOT/'.context/amiga/dosbox-scale';OUT.mkdir(parents=True,exist_ok=True)
(OUT/'probe.cpp').write_text('#include "scale.h"\nextern "C" void probe(volatile uint32_t*d,const uint32_t*s,unsigned w,unsigned f){pc_scale_row(d,s,w,f);}\n')
cmd=['podman','--connection','nuflix-converter-root','exec','zzffmpeg-toolchain']
def run(*args):subprocess.run([*cmd,*args],check=True)
p='/work/.context/amiga/dosbox-scale/'
run('arm-none-eabi-g++','-Os','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-fno-exceptions','-fno-rtti','-I/work/amiga/dosbox/pc','-c',p+'probe.cpp','-o',p+'probe.o')
run('arm-none-eabi-ld','-Ttext=0x10000','-e','probe',p+'probe.o','-o',p+'probe.elf')
run('arm-none-eabi-objcopy','-O','binary',p+'probe.elf',p+'probe.bin')
symbols=subprocess.check_output([*cmd,'arm-none-eabi-nm',p+'probe.elf'],text=True)
entry=int(next(x.split()[0] for x in symbols.splitlines() if x.endswith(' T probe')),16)
u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
u.mem_map(0x10000,0x10000);u.mem_write(0x10000,(OUT/'probe.bin').read_bytes())
u.mem_map(0x30000,0x10000)
# Sources end on an unmapped page boundary; a speculative extra read must fail.
u.mem_map(0x50000,4096);u.mem_map(0x60000,4096)
for width in range(1,1025):
 src=0x51000-width*4;dst=0x60004
 words=[((x*0x1234567)^0xa59e32b7)&0xffffffff for x in range(width)]
 u.mem_write(src,struct.pack('<'+'I'*width,*words))
 for fmt in (3,4):
  u.mem_write(0x60000,b'\xa5'*4096)
  for r,v in [(UC_ARM_REG_SP,0x3fff0),(UC_ARM_REG_LR,0x1fff0),(UC_ARM_REG_R0,dst),(UC_ARM_REG_R1,src),(UC_ARM_REG_R2,width),(UC_ARM_REG_R3,fmt)]:u.reg_write(r,v)
  u.emu_start(entry,0x1fff0,count=100000)
  assert u.reg_read(UC_ARM_REG_PC)==0x1fff0
  expected=b''.join(words[x*width//640].to_bytes(4,'little' if fmt==3 else 'big') for x in range(640))
  assert bytes(u.mem_read(dst,2560))==expected,(width,fmt)
  assert bytes(u.mem_read(0x60000,4))==b'\xa5'*4
  assert bytes(u.mem_read(dst+2560,4096-2564))==b'\xa5'*(4096-2564)
print('PASS A9 scaler model: all 1024 widths, both formats, exact pixels and guards')
