#!/usr/bin/env python3
"""Execute the ARM measurement service with explicit modeled hardware I/O."""
import argparse,hashlib,json,struct,subprocess
from pathlib import Path
from glide_firmware import validator_source
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/'amiga/dosbox/glide';OUT=ROOT/'.context/amiga/glide/measure-service'
def run(args):return subprocess.check_output(list(map(str,args)),cwd=ROOT,text=True,stderr=subprocess.STDOUT)
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--firmware',type=Path,required=True)
    a=p.parse_args();OUT.mkdir(parents=True,exist_ok=True)
    defines,validator=validator_source(a.firmware)
    c=OUT/'service.c';c.write_text('#include <stdint.h>\nstatic uint32_t be32(uint32_t x){return __builtin_bswap32(x);}\n'+defines+'\n'+validator+
        '\n#include "tests/amiga/dosbox/glide/measure_firmware.c"\n')
    objects=[]
    sources=[c,ROOT/'amiga/common/arm/memory.c',*[SRC/n for n in ['translator.c','descriptor.c','probe_scene.c',
              'experimental/measure.c','experimental/completion_client.c']]]
    for n,source in enumerate(sources):
        obj=OUT/f'{n}.o';objects.append(obj)
        run(['arm-none-eabi-gcc','-I.','-I'+str(SRC),'-I'+str(SRC/'experimental'),
             '-mcpu=cortex-a9','-marm','-mfpu=vfpv3','-mfloat-abi=softfp','-ffreestanding','-O2','-c',source,'-o',obj])
    elf=OUT/'service.elf';binary=OUT/'service.bin'
    run(['arm-none-eabi-ld','-Ttext=0x10000','-e','test_command',*objects,'-o',elf])
    run(['arm-none-eabi-objcopy','-O','binary',elf,binary])
    syms={p[2]:int(p[0],16) for line in run(['arm-none-eabi-nm',elf]).splitlines() if len(p:=line.split())==3}
    results=[]
    for failure in range(5):
        u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.mem_map(0x10000,0x80000)
        u.mem_write(0x10000,binary.read_bytes());u.mem_map(0x06610000,0x20000)
        u.reg_write(UC_ARM_REG_CPSR,0x13);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
        u.cpr_write(15,0,1,0,2,0,False,0xf00000)
        u.mem_write(syms['test_failure'],struct.pack('<I',failure if failure<4 else 0))
        def hook(uc,pc,size,data):
            if pc==0x80000:uc.emu_stop()
            if pc==syms['grx_verify_scene']:
                scene=uc.reg_read(UC_ARM_REG_R1)
                pixels=[]
                for row in range(72):
                    for x in range(64):
                        y=71-row;v=0
                        if scene and x>=10 and y>=10 and x+y<=62:
                            v=0xf800 if scene==1 else 0xffe0 if scene==2 else (0xf800 if x<32 else (0x001f if scene==3 else 0x800f))
                        pixels.append(v)
                if failure==4:pixels[0]=0xffff
                uc.mem_write(0x06610000,struct.pack('<4608H',*pixels))
        u.hook_add(UC_HOOK_CODE,hook)
        def call(name,*args):
            for reg,val in zip([UC_ARM_REG_R0,UC_ARM_REG_R1,UC_ARM_REG_R2],args):u.reg_write(reg,val)
            u.reg_write(UC_ARM_REG_SP,0x70000);u.reg_write(UC_ARM_REG_LR,0x80000)
            u.emu_start(syms[name],0,count=2000000)
            assert u.reg_read(UC_ARM_REG_PC)==0x80000,name
            return u.reg_read(UC_ARM_REG_R0)
        def command(op,frames=0,epoch=0):
            ptr=call('test_command',op,frames,epoch)
            return struct.unpack('>13I',u.mem_read(ptr,52))
        reply=command(1,2,123)
        if failure==1:
            assert reply[3]==0x30002 and call('test_claimed')==0
        else:
            assert reply[2]!=3 and reply[3]==0 and reply[12]==1
            for n in range(20):call('test_poll')
            reply=command(0)
            if failure:
                assert reply[2]==4 and reply[3] and reply[12]==1,reply
                assert command(2)[3]==0x30003 and call('test_claimed')==1
            else:
                assert reply[2]==3 and reply[4]==2 and reply[10]>20000,reply
                assert command(2)[3]==0 and call('test_claimed')==0
                reply=command(1,2,124)
                assert reply[2]==0 and reply[4]==0 and reply[12]==1,reply
        results.append({'modeled_failure':failure,'reply':list(reply)})
    record={'scope':'ARM service execution with modeled MMIO/FIFO/pixels; no physical FPS',
            'scenarios':results,'elf_sha256':hashlib.sha256(elf.read_bytes()).hexdigest()}
    (OUT/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    print('PASS ARM service: start, correctness warmup, counting, close/restart, busy, mapping, FIFO and pixel failures')
if __name__=='__main__':main()
