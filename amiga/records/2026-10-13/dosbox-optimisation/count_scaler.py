from pathlib import Path
import subprocess,struct,json
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE
from unicorn.arm_const import *
root=Path.cwd();out=root/'.context/amiga/dosbox-optimisation'
cmd=['podman','--connection','nuflix-converter-root','exec','zzffmpeg-toolchain']
def run(*args):subprocess.run(cmd+list(args),check=True,stdout=subprocess.DEVNULL)
rows=[]
for name,header in [('before','scale-before.h'),('candidate','../../../amiga/dosbox/pc/scale.h')]:
 # Absolute include inside the build container, independent of source staging.
 inc='/work/.context/amiga/dosbox-optimisation/scale-before.h' if name=='before' else '/work/amiga/dosbox/pc/scale.h'
 (out/(name+'.cpp')).write_text('#include "'+inc+'"\nextern "C" void probe(volatile uint32_t*d,const uint32_t*s,unsigned w,unsigned p){pc_scale_row(d,s,w,p&255,p>>8);}\n')
 p='/work/.context/amiga/dosbox-optimisation/'+name
 run('arm-none-eabi-g++','-Os','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-ffreestanding','-fno-builtin','-fno-exceptions','-fno-rtti','-c',p+'.cpp','-o',p+'.o')
 run('arm-none-eabi-ld','-Ttext=0x10000','-e','probe',p+'.o','/usr/lib/gcc/arm-none-eabi/15/thumb/v7-a/nofp/libgcc.a','-o',p+'.elf')
 run('arm-none-eabi-objcopy','-O','binary',p+'.elf',p+'.bin')
 syms=subprocess.check_output(cmd+['arm-none-eabi-nm',p+'.elf'],text=True)
 entry=int(next(s.split()[0] for s in syms.splitlines() if s.endswith(' T probe')),16)
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 for addr,size in [(0x10000,0x10000),(0x30000,0x10000),(0x50000,0x10000)]:u.mem_map(addr,size)
 u.mem_write(0x10000,(out/(name+'.bin')).read_bytes());count=[0]
 def hook(*args):count[0]+=1
 u.hook_add(UC_HOOK_CODE,hook)
 for width,target in [(320,640),(640,640),(640,960),(320,960),(640,508),(800,640)]:
  src=0x50000;dst=0x54000;words=list(range(width));u.mem_write(src,struct.pack('<'+'I'*width,*words))
  count[0]=0
  for reg,val in [(UC_ARM_REG_SP,0x3fff0),(UC_ARM_REG_LR,0x1fff0),(UC_ARM_REG_R0,dst),(UC_ARM_REG_R1,src),(UC_ARM_REG_R2,width),(UC_ARM_REG_R3,(target<<8)|3)]:u.reg_write(reg,val)
  u.emu_start(entry,0x1fff0,count=1000000)
  assert u.reg_read(UC_ARM_REG_PC)==0x1fff0
  assert bytes(u.mem_read(dst,target*4))==struct.pack('<'+'I'*target,*[words[x*width//target] for x in range(target)])
  rows.append(dict(build=name,source_width=width,target_width=target,arm_instructions=count[0]))
(out/'scale-instructions.json').write_text(json.dumps({'scope':'Unicorn executed ARM instructions for one row; not hardware cycles, timing or FPS','rows':rows},indent=2)+'\n')
for row in rows:print(row)
