#!/usr/bin/env python3
"""A9 clipper versus native output; no hardware timing claim."""
import ctypes as C
from pathlib import Path
import hashlib,json,random,struct,subprocess
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[4];SRC=ROOT/'amiga/dosbox/glide';OUT=ROOT/'.context/amiga/glide/clip-model';OUT.mkdir(parents=True,exist_ok=True)
class V(C.Structure):
 _fields_=[(n,C.c_float) for n in ('x','y','r','g','b','a','sow','tow','oow')]
def run(*a):subprocess.run(list(map(str,a)),check=True)
run('clang','-std=c11','-shared','-fPIC','-O2','-ffp-contract=off',SRC/'clip.c','-o',OUT/'native.dylib')
lib=C.CDLL(str(OUT/'native.dylib'));lib.grx_clip_triangle.argtypes=[C.POINTER(V),C.c_uint,C.c_uint,C.POINTER(V),C.POINTER(C.c_uint)]
r=random.Random(8871)
triangles=[]
for i in range(256):
 triangles.append((V*3)(*[V(r.randint(-640,1280),r.randint(-480,960),r.randrange(256),r.randrange(256),r.randrange(256),r.randrange(256),r.uniform(-100,300),r.uniform(-100,300),r.uniform(.01,2)) for _ in range(3)]))
# Bare-metal compiler has no C-library headers; link the tested memory helper.
(OUT/'string.h').write_text('#include <stddef.h>\nvoid *memcpy(void *,const void *,size_t);\n')
for abi in ('soft','softfp'):
 elf=OUT/(abi+'.elf');binary=OUT/(abi+'.bin')
 run('arm-none-eabi-gcc','-O2','-std=c11','-I'+str(OUT),'-ffreestanding','-ffp-contract=off','-fno-builtin','-nostdlib','-mcpu=cortex-a9','-marm','-mfpu=vfpv3','-mfloat-abi='+abi,'-Wl,-Ttext=0x100000','-Wl,-e,grx_clip_triangle',SRC/'clip.c',ROOT/'amiga/common/arm/memory.c','-lgcc','-o',elf)
 run('arm-none-eabi-objcopy','-O','binary',elf,binary)
 symbols={p[2]:int(p[0],16) for l in subprocess.check_output(['arm-none-eabi-nm',str(elf)],text=True).splitlines() if len(p:=l.split())==3}
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9);u.mem_map(0x100000,0x200000);u.mem_write(0x100000,binary.read_bytes())
 u.reg_write(UC_ARM_REG_C1_C0_2,0xf00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
 for tri in triangles:
  out=(V*7)();n=C.c_uint(0);expected=lib.grx_clip_triangle(tri,640,480,out,C.byref(n))
  u.mem_write(0x200000,bytes(tri));u.mem_write(0x201000,bytes(C.sizeof(out)));u.mem_write(0x202000,bytes(4))
  u.reg_write(UC_ARM_REG_SP,0x2ff000);u.reg_write(UC_ARM_REG_LR,0x2fe000);u.mem_write(0x2ff000,struct.pack('<I',0x202000))
  for reg,arg in zip((UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2,UC_ARM_REG_R3),(0x200000,640,480,0x201000)):u.reg_write(reg,arg)
  u.emu_start(symbols['grx_clip_triangle'],0x2fe000,count=1000000)
  assert u.reg_read(UC_ARM_REG_PC)==0x2fe000
  assert u.reg_read(UC_ARM_REG_R0)==expected
  assert struct.unpack('<I',u.mem_read(0x202000,4))[0]==n.value
  assert bytes(u.mem_read(0x201000,n.value*C.sizeof(V)))==bytes(out)[:n.value*C.sizeof(V)]
 print('PASS Cortex-A9 '+abi+': 256 clipped triangles bit-identical to native')
inputs=[Path(__file__),SRC/'clip.c',SRC/'clip.h',SRC/'vertex.h',SRC/'translator.h',ROOT/'amiga/common/arm/memory.c']
record={'scope':'Instruction model only; no hardware timing or GPU coverage claim',
 'seed':8871,'bit_identical_triangles':{'soft':256,'softfp':256},
 'compiler':subprocess.check_output(['arm-none-eabi-gcc','--version'],text=True).splitlines()[0],
 'sources':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs}}
(OUT/'validation.json').write_text(json.dumps(record,indent=2)+'\n')
