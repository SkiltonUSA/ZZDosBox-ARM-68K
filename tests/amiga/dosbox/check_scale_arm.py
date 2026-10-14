#!/usr/bin/env python3
"""Check the production scaler compiled for A9 against an independent reference.

Instruction model only, not a physical performance benchmark.
"""
import subprocess,struct,sys
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3]
NEON='--neon' in sys.argv
STYLES='--styles' in sys.argv
DISPATCH='--dispatch' in sys.argv
assert not DISPATCH or NEON
OUT=ROOT/('.context/amiga/dosbox-scale'+('-neon' if NEON else '')+('-dispatch' if DISPATCH else ''));OUT.mkdir(parents=True,exist_ok=True)
(OUT/'probe.cpp').write_text('#include "scanline_pixels.h"\nextern "C" void probe(volatile uint32_t*d,const uint32_t*s,unsigned w,unsigned packed){pc_scan_scale_row(d,s,w,packed&255,(packed>>8)&1023,packed>>20);}\n')
cmd=['podman','--connection','nuflix-converter-root','exec','zzffmpeg-toolchain']
def run(*args):subprocess.run([*cmd,*args],check=True)
p='/work/'+str(OUT.relative_to(ROOT))+'/'
extra=[]
if NEON:
 run('arm-none-eabi-g++','-O2',*(['-DDB_NEON_DISPATCH'] if DISPATCH else []),'-mcpu=cortex-a9','-marm','-mfloat-abi=softfp','-mfpu=neon','-ffreestanding','-fno-builtin','-fno-exceptions','-fno-rtti','-c','/work/amiga/dosbox/pc/pixels_neon.cpp','-o',p+'neon.o')
 extra=[p+'neon.o']
run('arm-none-eabi-g++','-Os','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-fno-exceptions','-fno-rtti','-I/work/amiga/dosbox/pc',*(['-DDB_NEON'] if NEON else []),'-c',p+'probe.cpp','-o',p+'probe.o')
run('arm-none-eabi-ld','-Ttext=0x10000','-e','probe',p+'probe.o',*extra,'/usr/lib/gcc/arm-none-eabi/15/thumb/v7-a/nofp/libgcc.a','-o',p+'probe.elf')
run('arm-none-eabi-objcopy','-O','binary',p+'probe.elf',p+'probe.bin')
symbols=subprocess.check_output([*cmd,'arm-none-eabi-nm',p+'probe.elf'],text=True)
entry=int(next(x.split()[0] for x in symbols.splitlines() if x.endswith(' T probe')),16)
u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
if NEON:u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
u.mem_map(0x10000,0x10000);u.mem_write(0x10000,(OUT/'probe.bin').read_bytes())
u.mem_map(0x30000,0x10000)
# Sources end on an unmapped page boundary; a speculative extra read must fail.
u.mem_map(0x50000,4096);u.mem_map(0x60000,4096)
for width in range(1,1025):
 src=0x51000-width*4;dst=0x60004
 words=[((x*0x1234567)^0xa59e32b7)&0xffffffff for x in range(width)]
 u.mem_write(src,struct.pack('<'+'I'*width,*words))
 for target,fmt,level in ((target,fmt,level) for target in (320,508,640,960) for fmt in (3,4) for level in ((8,5,6,4) if STYLES else (8,))):
  u.mem_write(0x60000,b'\xa5'*4096)
  for r,v in [(UC_ARM_REG_SP,0x3fff0),(UC_ARM_REG_LR,0x1fff0),(UC_ARM_REG_R0,dst),(UC_ARM_REG_R1,src),(UC_ARM_REG_R2,width),(UC_ARM_REG_R3,fmt|(target<<8)|(level<<20))]:u.reg_write(r,v)
  u.emu_start(entry,0x1fff0,count=100000)
  assert u.reg_read(UC_ARM_REG_PC)==0x1fff0
  expected=b''.join(words[x*width//target].to_bytes(4,'little' if fmt==3 else 'big') for x in range(target))
  expected=bytes(c*level//8 for c in expected)
  assert bytes(u.mem_read(dst,target*4))==expected,(width,target,fmt,level)
  assert bytes(u.mem_read(0x60000,4))==b'\xa5'*4
  assert bytes(u.mem_read(dst+target*4,4096-target*4-4))==b'\xa5'*(4096-target*4-4)
print('PASS A9 scaler model: all 1024 widths, four output widths, both formats, exact pixels and guards; styles='+str(STYLES))

if NEON:
 # Exercise every vector tail in the specialised 1x/2x paths directly.
 scaler=int(next(x.split()[0] for x in symbols.splitlines() if x.endswith(' T pc_neon_scale')),16)
 u.mem_map(0x70000,12288)
 for width in range(1,1025):
  src=0x51000-width*4
  words=[((x*0x1234567)^0xa59e32b7)&0xffffffff for x in range(width)]
  u.mem_write(src,struct.pack('<'+'I'*width,*words))
  for ratio,fmt,offset in ((r,f,o) for r in (1,2) for f in (3,4) for o in (4,16)):
   target=width*ratio;dst=0x70000+offset
   u.mem_write(0x70000,b'\xa5'*12288)
   for r,v in [(UC_ARM_REG_SP,0x3fff0),(UC_ARM_REG_LR,0x1fff0),(UC_ARM_REG_R0,dst),(UC_ARM_REG_R1,src),(UC_ARM_REG_R2,width),(UC_ARM_REG_R3,fmt)]:u.reg_write(r,v)
   u.mem_write(0x3fff0,struct.pack('<I',target))
   u.emu_start(scaler,0x1fff0,count=100000)
   assert u.reg_read(UC_ARM_REG_PC)==0x1fff0
   expected=b''.join(words[x//ratio].to_bytes(4,'little' if fmt==3 else 'big') for x in range(target))
   assert bytes(u.mem_read(dst,target*4))==expected,(width,ratio,fmt,offset)
   assert bytes(u.mem_read(0x70000,offset))==b'\xa5'*offset
   remaining=12288-offset-target*4
   assert bytes(u.mem_read(dst+target*4,remaining))==b'\xa5'*remaining
 print('PASS A9 direct NEON scaler: all widths 1..1024, 1x/2x, formats and alignments; dispatch='+str(DISPATCH))
 black=int(next(x.split()[0] for x in symbols.splitlines() if x.endswith(' T pc_neon_black')),16)
 for width in range(1,961):
  u.mem_write(0x60000,b'\xa5'*4096)
  for r,v in [(UC_ARM_REG_SP,0x3fff0),(UC_ARM_REG_LR,0x1fff0),(UC_ARM_REG_R0,0x60004),(UC_ARM_REG_R1,width)]:u.reg_write(r,v)
  u.emu_start(black,0x1fff0,count=100000)
  assert u.reg_read(UC_ARM_REG_PC)==0x1fff0
  assert bytes(u.mem_read(0x60004,width*4))==bytes(width*4)
  assert bytes(u.mem_read(0x60000,4))==b'\xa5'*4
  assert bytes(u.mem_read(0x60004+width*4,4092-width*4))==b'\xa5'*(4092-width*4)
 print('PASS A9 NEON black-row fill: widths 1..960, exact zeros and guards')
