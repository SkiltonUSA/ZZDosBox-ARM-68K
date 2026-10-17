#!/usr/bin/env python3
"""Check the production scanline converter compiled for A9 against an independent reference.

Instruction model only, not a physical performance benchmark.
"""
import subprocess,struct,sys
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3]
NEON='--neon' in sys.argv
OUT=ROOT/('.context/amiga/dosbox-capture'+('-neon' if NEON else ''));OUT.mkdir(parents=True,exist_ok=True)
(OUT/'probe.cpp').write_text('#include "capture.h"\nextern "C" bool probe(uint32_t*d,const uint8_t*s,unsigned w,unsigned b,const uint32_t*p){return pc_capture_row(d,s,w,b,p); }\n')
import os
cmd=json.loads(os.environ.get('DOSBOX_ARM_COMMAND','["podman","--connection","nuflix-converter-root","exec","zzdosbox-toolchain"]'))
def run(*args):subprocess.run([*cmd,*args],check=True)
p='/work/'+str(OUT.relative_to(ROOT))+'/'
extra=[]
if NEON:
 run('arm-none-eabi-g++','-O2','-mcpu=cortex-a9','-marm','-mfloat-abi=softfp','-mfpu=neon','-ffreestanding','-fno-builtin','-fno-exceptions','-fno-rtti','-c','/work/amiga/dosbox/pc/pixels_neon.cpp','-o',p+'neon.o')
 extra=[p+'neon.o']
run('arm-none-eabi-g++','-Os','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-fno-exceptions','-fno-rtti','-I/work/amiga/dosbox/pc',*(['-DDB_NEON'] if NEON else []),'-c',p+'probe.cpp','-o',p+'probe.o')
run('arm-none-eabi-gcc','-Os','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-c','/work/amiga/common/arm/memory.c','-o',p+'memory.o')
run('arm-none-eabi-ld','-Ttext=0x10000','-e','probe',p+'probe.o',*extra,p+'memory.o','-o',p+'probe.elf')
run('arm-none-eabi-objcopy','-O','binary',p+'probe.elf',p+'probe.bin')
symbols=subprocess.check_output([*cmd,'arm-none-eabi-nm',p+'probe.elf'],text=True)
entry=int(next(x.split()[0] for x in symbols.splitlines() if x.endswith(' T probe')),16)
u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
if NEON:u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
u.mem_map(0x10000,0x10000);u.mem_write(0x10000,(OUT/'probe.bin').read_bytes())
u.mem_map(0x30000,0x10000)
# Sources end on an unmapped page boundary; a speculative extra read must fail.
u.mem_map(0x50000,4096);u.mem_map(0x60000,8192);u.mem_map(0x70000,4096)
palette=[(x*0x1020304)&0xffffffff for x in range(256)]
u.mem_write(0x70000,struct.pack('<256I',*palette));u.mem_write(0x3fff0,struct.pack('<I',0x70000))
def check(width,bpp,data,offset=0):
 src=0x51000-len(data)-offset;dst=0x60004
 u.mem_write(src,data);u.mem_write(0x60000,b'\xa5'*8192)
 for r,v in [(UC_ARM_REG_SP,0x3fff0),(UC_ARM_REG_LR,0x1fff0),(UC_ARM_REG_R0,dst),(UC_ARM_REG_R1,src),(UC_ARM_REG_R2,width),(UC_ARM_REG_R3,bpp)]:u.reg_write(r,v)
 try:u.emu_start(entry,0x1fff0,count=100000)
 except Exception:
  print('Failed',width,bpp,hex(u.reg_read(UC_ARM_REG_PC)),hex(u.reg_read(UC_ARM_REG_CPSR)),flush=True);raise
 assert u.reg_read(UC_ARM_REG_PC)==0x1fff0 and u.reg_read(UC_ARM_REG_R0)==1
 if bpp==8:expected=b''.join(struct.pack('<I',palette[c]) for c in data)
 elif bpp==32:expected=data
 else:
  expected=bytearray()
  for c, in struct.iter_unpack('<H',data):
   r=(c>>10)&31 if bpp==15 else c>>11;g=(c>>5)&(31 if bpp==15 else 63);b=c&31
   green=((g<<3)|(g>>2)) if bpp==15 else ((g<<2)|(g>>4))
   expected+=struct.pack('<I',(((r<<3)|(r>>2))<<16)|(green<<8)|(b<<3)|(b>>2))
 assert bytes(u.mem_read(dst,width*4))==expected,(width,bpp)
 assert bytes(u.mem_read(0x60000,4))==b'\xa5'*4
 assert bytes(u.mem_read(dst+width*4,8192-width*4-4))==b'\xa5'*(8192-width*4-4)
for width in range(1,1025):
 for bpp in (8,15,16,32):check(width,bpp,bytes((x*137+width*71)&255 for x in range(width*((bpp+7)//8))))
for width in (1,3,4,7,8,9,15,16,17,319,320,321,1023):
 for bpp in (15,16,32):
  for offset in (1,2,3):check(width,bpp,bytes((x*53+width)&255 for x in range(width*((bpp+7)//8))),offset)
for start in range(0,65536,1024):check(1024,16,struct.pack('<1024H',*range(start,start+1024)))
print('PASS A9 capture model: all 65536 RGB565 colours, 1024 widths, all bpp, exact pixels and guards')
