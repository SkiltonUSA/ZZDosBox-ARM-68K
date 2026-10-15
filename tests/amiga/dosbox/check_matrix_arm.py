#!/usr/bin/env python3
"""A9 instruction-model accuracy/bounds tests. No physical speed inference."""
import argparse, importlib.util, json, math, random, struct, subprocess
from pathlib import Path
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_ARM, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_MEM_WRITE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build',required=True,type=Path);p.add_argument('--lifecycle',action='store_true');a=p.parse_args()
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
base=0x12000040
image,rel,_=pack.unpack((a.build/'dosbox.elf').read_bytes())
for off in rel:struct.pack_into('<I',image,off,struct.unpack_from('<I',image,off)[0]+base)
symbols={f[2]:int(f[0],16)+base for line in subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm','-n',str(a.build/'dosbox.elf')],text=True).splitlines() if len(f:=line.split())==3}
u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000);u.reg_write(UC_ARM_REG_FPSCR,0)
u.mem_map(base&~4095,0x2002000);u.mem_write(base,bytes(image))
locations=[base+0x1600004,base+0x1620004,base+0x1640004]
rng=random.Random(7803);allowed=[];bad=[];largest=0.;calls=0
def access(u,kind,address,size,value,data):
    if not any(lo<=address and address+size<=hi and (kind!=UC_MEM_WRITE or writable) for lo,hi,writable in allowed):
        bad.append((hex(address),size,kind));u.emu_stop()
u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base+0x1600000,end=base+0x1680000)
def floats(n):
    return [struct.unpack('<f',struct.pack('<f',rng.uniform(-2,2)))[0] for _ in range(n)]
def check(fn,m,v,n,matrix):
    global allowed,largest,calls
    length=16 if matrix else n*4
    arrays=[m,v,[0.0]*length]
    raw=[struct.pack('<'+'f'*len(x),*x) for x in arrays]
    allowed=[(addr,addr+len(data),i==2) for i,(addr,data) in enumerate(zip(locations,raw))]
    for addr,data in zip(locations,raw):u.mem_write(addr-4,b'\x95'*4+data+b'\x95'*16)
    # n=0 must perform no accesses, including no input loads in vector paths.
    if not n and not matrix:allowed=[]
    for reg,val in [(UC_ARM_REG_R0,locations[0]),(UC_ARM_REG_R1,locations[1]),(UC_ARM_REG_R2,locations[2]),(UC_ARM_REG_R3,n),(UC_ARM_REG_SP,base+0x1ffffb8),(UC_ARM_REG_LR,base+0x1ff0000)]:u.reg_write(reg,val)
    u.emu_start(symbols[fn],base+0x1ff0000,count=5000000)
    assert not bad,(fn,n,bad)
    assert u.reg_read(UC_ARM_REG_PC)==base+0x1ff0000,(fn,n,'did not return')
    got=struct.unpack('<'+'f'*length,u.mem_read(locations[2],length*4))
    for i,value in enumerate(got):
        products=[m[(i//4)*4+k]*v[k*4+i%4] for k in range(4)] if matrix else [m[(i%4)*4+k]*v[(i//4)*4+k] for k in range(4)]
        expected=sum(products);scale=max(1.0,sum(map(abs,products)))
        error=abs(value-expected)/scale
        assert math.isfinite(value) and error<2e-6,(fn,n,i,value,expected,error)
        largest=max(largest,error)
    for i,(addr,data) in enumerate(zip(locations,raw)):
        assert bytes(u.mem_read(addr-4,4))==b'\x95'*4
        assert bytes(u.mem_read(addr+len(data),16))==b'\x95'*16
        if i!=2:assert bytes(u.mem_read(addr,len(data)))==data
    calls+=1
methods=('scalar','hlsl','neon') if json.loads((a.build/'build.json').read_text()).get('hlslpp') else ('scalar','cmsis','neon')
for _ in range(100):
    m,v=floats(16),floats(16)
    for method in methods:check('zz_mat_'+method,m,v,4,True)
for n in (0,1,2,3,4,5,7,8,9,15,16,17,31,32,33,127,256,1023,1024,1025,3072,4096):
    m,v=floats(16),floats(n*4)
    for method in methods:check('zz_vec_'+method,m,v,n,False)
print('PASS',calls,'ARM matrix/vector calls; finite f32 accuracy vs double oracle; strict read/write bounds, immutable inputs, guards; max scaled error',largest,flush=True)
if a.lifecycle:
    import check_pc_arm as pc
    pc.OUT=a.build.resolve()
    for mode in ('fixture','copy-stop','neon-denied'):
        pc.run(0x150020c0,mode,True,options=(8,0,3000),inherited_fpscr=0x03c00000)

    pc.run(0x12000040,'fixture',True,options=(8,0,3000),inherited_fpscr=0x03c00000,matrix_stop=True)
