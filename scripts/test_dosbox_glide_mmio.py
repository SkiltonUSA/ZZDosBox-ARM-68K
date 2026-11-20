#!/usr/bin/env python3
"""Run guarded MMIO through the actual patched BSP abort handler on ARM32."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import subprocess
from glide_abort import patch_vectors
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_ARM, UC_HOOK_CODE
from unicorn.arm_const import *

ROOT=Path(__file__).resolve().parents[1]
EXP=ROOT/'amiga/dosbox/glide/experimental'
OUT=ROOT/'.context/amiga/glide/mmio-guard'

def run(args):
    return subprocess.check_output(list(map(str,args)),cwd=ROOT,text=True,stderr=subprocess.STDOUT)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--vectors',type=Path,required=True)
    parser.add_argument('--experimental',type=Path,default=EXP)
    parser.add_argument('--output',type=Path,default=OUT)
    a=parser.parse_args()
    exp=a.experimental.resolve();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
    native=out/'native'
    run(['clang','-std=c11','-O2','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',
         '-I.',ROOT/'tests/amiga/dosbox/glide/mmio_guard.c',exp/'mmio_guard.c','-o',native])
    print(run([native]).strip())
    source=a.vectors.read_text()
    patched=patch_vectors(source)
    body=patched[patched.index('DataAbortHandler:'):patched.index('PrefetchAbortHandler:')]
    # Use the real BSP prologue, diagnostic store and return, not a modeled C dispatcher.
    vector=out/'vector.S'
    vector.write_text('.syntax unified\n.arm\n.text\n.global DataAbortHandler\n'+body+'''
.global DataAbortInterrupt
DataAbortInterrupt:
    b DataAbortInterrupt
.bss
.align 2
DataAbortAddr: .space 4
.section .note.GNU-stack,"",%progbits
''')
    flags=['-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-O2','-ffreestanding','-fno-builtin','-DCONFIG_ARM_ERRATA_775420=1']
    objects=[]
    for i,f in enumerate([exp/'mmio_guard.c',exp/'mmio_guard.S',vector]):
        obj=out/f'{i}.o';objects.append(obj)
        run(['arm-none-eabi-gcc',*flags,'-c',f,'-o',obj])
    elf=out/'test.elf';binary=out/'test.bin'
    run(['arm-none-eabi-ld','-Ttext=0x10000','-e','grx_mmio_load',*objects,'-o',elf])
    run(['arm-none-eabi-objcopy','-O','binary',elf,binary])
    symbols={line.split()[2]:int(line.split()[0],16) for line in run(['arm-none-eabi-nm',elf]).splitlines() if len(line.split())==3}
    results=[]
    for mode in (0x13,0x1f):
     for write in (False,True):
       for mask in (0,0x40,0x80,0xc0):
        for fault in ('none','slverr','decerr','async','wrong-address','wrong-direction'):
         u=Uc(UC_ARCH_ARM,UC_MODE_ARM)
         u.mem_map(0x10000,0x40000);u.mem_write(0x10000,binary.read_bytes())
         u.mem_map(0x83c80000,0x1000)
         u.mem_write(0x83c80014,struct.pack('<I',0x12345678))
         u.mem_write(0x40000,struct.pack('<I',0xdeadbeef))
         u.reg_write(UC_ARM_REG_CPSR,0x17);u.reg_write(UC_ARM_REG_SP,0x3f000)
         initial=mode|mask
         u.reg_write(UC_ARM_REG_CPSR,initial);u.reg_write(UC_ARM_REG_SP,0x3e000)
         u.reg_write(UC_ARM_REG_LR,0x20000)
         preserved={reg:0xab000000+i for i,reg in enumerate(range(UC_ARM_REG_R4,UC_ARM_REG_R12))}
         for reg,value in preserved.items():u.reg_write(reg,value)
         u.reg_write(UC_ARM_REG_R0,0x83c80014)
         u.reg_write(UC_ARM_REG_R1,0x87654321 if write else 0x40000)
         entry=symbols['grx_mmio_store' if write else 'grx_mmio_load']
         access=symbols['grx_mmio_store_insn' if write else 'grx_mmio_load_insn']
         state={'injected':False,'fallback':False,'returned':False}
         def hook(uc,pc,size,data):
             if pc==0x20000:
                 state['returned']=True;uc.emu_stop()
             elif pc==symbols['DataAbortInterrupt']:
                 state['fallback']=True;uc.emu_stop()
             elif pc==access and fault!='none' and not state['injected']:
                 state['injected']=True
                 saved=uc.reg_read(UC_ARM_REG_CPSR)
                 dfsr=(0x8 if fault=='decerr' else 0x1008)|(0x800 if write else 0)
                 if fault=='async':dfsr=0x1406
                 if fault=='wrong-direction':dfsr^=0x800
                 uc.cpr_write(15,0,5,0,0,0,False,dfsr)
                 uc.cpr_write(15,0,6,0,0,0,False,0x83c80010 if fault=='wrong-address' else 0x83c80014)
                 uc.reg_write(UC_ARM_REG_CPSR,(saved&~31)|0x17|0x180)
                 uc.reg_write(UC_ARM_REG_SPSR,saved)
                 uc.reg_write(UC_ARM_REG_LR,pc+8)
                 uc.reg_write(UC_ARM_REG_PC,symbols['DataAbortHandler'])
         u.hook_add(UC_HOOK_CODE,hook)
         u.emu_start(entry,0,count=2000)
         recover=fault in ('none','slverr','decerr')
         assert state['returned']==recover and state['fallback']!=recover,(write,mask,fault,state)
         if recover:
             assert u.reg_read(UC_ARM_REG_R0)==int(fault=='none')
             assert u.reg_read(UC_ARM_REG_CPSR)&0xff==initial
             assert u.reg_read(UC_ARM_REG_SP)==0x3e000
             assert all(u.reg_read(reg)==value for reg,value in preserved.items())
         target=0x83c80014 if write else 0x40000
         expected=(0x87654321 if write else 0x12345678) if fault=='none' else (0x12345678 if write else 0xdeadbeef)
         assert struct.unpack('<I',u.mem_read(target,4))[0]==expected,(write,fault)
         results.append({'mode':mode,'write':write,'irq_fiq_mask':mask,'fault':fault,'returned':state['returned'],'fallback':state['fallback']})
    record={'scope':'ARM instruction model, not physical AXI error injection','cases':results,
            'bsp_vectors_sha256':hashlib.sha256(a.vectors.read_bytes()).hexdigest(),
            'patched_vectors_sha256':hashlib.sha256(patched.encode()).hexdigest(),
            'elf_sha256':hashlib.sha256(elf.read_bytes()).hexdigest()}
    (out/'results.json').write_text(json.dumps(record,indent=2)+'\n')
    print(f'PASS ARM guarded MMIO: {len(results)} success/fault/mask cases, original BSP frame and return')

if __name__=='__main__':main()
