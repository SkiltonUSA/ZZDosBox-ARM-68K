#!/usr/bin/env python3
"""Native CPU/x87 checkpoint versus actual Cortex-A9 interpreter continuation."""
import argparse,os,subprocess
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM
from unicorn.arm_const import UC_CPU_ARM_CORTEX_A9,UC_ARM_REG_SP,UC_ARM_REG_LR,UC_ARM_REG_PC,UC_ARM_REG_R0,UC_ARM_REG_C1_C0_2,UC_ARM_REG_FPEXC

r=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--prepared',type=Path,default=r/'.context/checkpoint-fpu')
p.add_argument('--output',type=Path,default=r/'.context/checkpoint-processor-a9')
p.add_argument('--compiler',default='arm-none-eabi-g++')
p.add_argument('--libc',type=Path,default=r/'.context/amiga/dosbox-libc/libc.a')
a=p.parse_args();o=a.output.resolve();o.mkdir(parents=True,exist_ok=True);prepared=a.prepared.resolve()
sources=[prepared/'src/cpu'/f'{n}.cpp' for n in ['cpu','core_normal','flags','modrm','paging','callback']]+[prepared/'memory.cpp',prepared/'src/fpu/fpu.cpp',r/'amiga/dosbox/state/cpu_checkpoint.cpp',r/'amiga/dosbox/state/fpu_checkpoint.cpp',r/'amiga/dosbox/state/processor_checkpoint.cpp',r/'tests/amiga/dosbox/state_processor_arm.cpp']
flags=['-std=c++17','-O2','-fno-exceptions','-fno-rtti','-fno-strict-aliasing','-ffp-contract=off','-ffunction-sections','-fdata-sections','-Wno-format',*['-I'+str(x) for x in (prepared/'include',prepared/'src/cpu',prepared,r/'amiga/dosbox',o)]]
subprocess.run(['clang++',*flags,'-DPROCESSOR_GOLDEN',*map(str,sources),'-Wl,-dead_strip','-o',str(o/'golden')],check=True)
golden=subprocess.check_output([str(o/'golden')]);assert len(golden)==5*(420+1024+420+4)
(o/'processor-golden.h').write_text('static const unsigned char golden[]={'+','.join(map(str,golden))+'};\n')
# Only LONGTYPE is needed from cross.h; bare-metal newlib has no directory
# backend. Preserve its GNU constant macro and skip host filesystem declarations.
objects=[]
for n,src in enumerate(sources):
 obj=o/f'{n}.o';objects.append(obj)
 subprocess.run([a.compiler,*flags,'-DDOSBOX_CROSS_H','-DLONGTYPE(a)=a##LL','-mcpu=cortex-a9','-marm','-mfpu=vfpv3','-mfloat-abi=softfp','-c',str(src),'-o',str(obj)],check=True)
libgcc=subprocess.check_output([a.compiler,'-mcpu=cortex-a9','-marm','-mfpu=vfpv3','-mfloat-abi=softfp','-print-libgcc-file-name'],text=True).strip()
llvm=Path(os.environ.get('LLVM_BIN','/opt/homebrew/opt/llvm/bin'))
subprocess.run([os.environ.get('LD_LLD','/opt/homebrew/opt/lld/bin/ld.lld'),'--gc-sections','-Ttext=0x10000','-e','main',*map(str,objects),'--start-group',str(a.libc.resolve()),libgcc,'--end-group','-o',str(o/'probe.elf')],check=True)
subprocess.run([str(llvm/'llvm-objcopy'),'-O','binary',str(o/'probe.elf'),str(o/'probe.bin')],check=True)
symbols={v[2]:int(v[0],16) for line in subprocess.check_output([str(llvm/'llvm-nm'),str(o/'probe.elf')],text=True).splitlines() if len(v:=line.split())==3}
u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
u.mem_map(0x10000,0x2000000);u.mem_write(0x10000,(o/'probe.bin').read_bytes())
u.reg_write(UC_ARM_REG_C1_C0_2,0xf00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
u.reg_write(UC_ARM_REG_SP,0x2000000);u.reg_write(UC_ARM_REG_LR,0x1ff0000)
for name,address in symbols.items():
 if name.startswith('_GLOBAL__sub_I_'):
  u.reg_write(UC_ARM_REG_LR,0x1ff0000);u.emu_start(address,0x1ff0000,count=1000000)
  assert u.reg_read(UC_ARM_REG_PC)==0x1ff0000,'Constructor did not return'
u.reg_write(UC_ARM_REG_LR,0x1ff0000)
u.emu_start(symbols['main'],0x1ff0000,count=500000000)
assert u.reg_read(UC_ARM_REG_PC)==0x1ff0000,'Instruction budget exhausted'
assert u.reg_read(UC_ARM_REG_R0)==0,('ARM processor continuation differs',u.reg_read(UC_ARM_REG_R0))
print('PASS Cortex-A9 CPU/x87 transaction: five native-record interpreter continuations, whole test-RAM hash, 2100 atomic corrupt-record refusals; no physical/FPS claim')
