#!/usr/bin/env python3
"""Compare A9 machine-code execution with the native encoder; no GPU/FPS claim.

Run scripts/test_dosbox_glide.py --arm first. Requires the Unicorn package.
"""
import ctypes as C
import json
import hashlib
from pathlib import Path
import struct
import subprocess
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_ARM
from unicorn.arm_const import (UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_SP,
    UC_ARM_REG_LR, UC_ARM_REG_C1_C0_2, UC_ARM_REG_FPEXC, UC_CPU_ARM_CORTEX_A9, UC_ARM_REG_PC)

ROOT = Path(__file__).resolve().parents[4]
OUT = ROOT/'.context/amiga/glide'
SRC = ROOT/'amiga/dosbox/glide'


class Config(C.Structure):
    _fields_ = [(name, C.c_uint32) for name in ('width','height','color_address',
        'texture_width','texture_height','texture_address','origin','blend','depth_test')]


class Vertex(C.Structure):
    _fields_ = [(name, C.c_float) for name in ('x','y','r','g','b','a','sow','tow','oow')]


class Packet(C.Structure):
    _fields_ = [('bytes', C.c_ubyte*992), ('words', C.c_uint32),
                ('config', Config), ('open', C.c_uint32)]


def run(*args):
    subprocess.run([str(a) for a in args], check=True)


def main():
    native = OUT/'native.dylib'
    run('clang', '-std=c11', '-shared', '-fPIC', '-O2', SRC/'translator.c', SRC/'descriptor.c', '-o', native)
    lib = C.CDLL(str(native))
    lib.grx_begin.argtypes = [C.POINTER(Packet), C.POINTER(Config)]
    lib.grx_triangle.argtypes = [C.POINTER(Packet), C.POINTER(Vertex)]
    lib.grx_end.argtypes = [C.POINTER(Packet)]
    checked = {}
    for abi in ('soft', 'softfp'):
        elf = OUT/(abi+'.elf'); binary = OUT/(abi+'.bin')
        run('arm-none-eabi-gcc', '-nostdlib', '-mcpu=cortex-a9', '-marm', '-mfpu=vfpv3',
            '-mfloat-abi='+abi, '-Wl,-Ttext=0x100000', '-Wl,-e,grx_begin',
            OUT/('translator.'+abi+'.o'), OUT/('descriptor.'+abi+'.o'), '-lgcc', '-o', elf)
        run('arm-none-eabi-objcopy', '-O', 'binary', elf, binary)
        symbols = {p[2]: int(p[0],16) for line in subprocess.check_output(
            ['arm-none-eabi-nm',str(elf)], text=True).splitlines() if len(p:=line.split())==3}
        u = Uc(UC_ARCH_ARM, UC_MODE_ARM)
        u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
        u.mem_map(0x100000, 0x200000);u.mem_write(0x100000, binary.read_bytes())
        u.reg_write(UC_ARM_REG_C1_C0_2, 0xf00000);u.reg_write(UC_ARM_REG_FPEXC, 0x40000000)

        def call(name, *args):
            u.reg_write(UC_ARM_REG_SP, 0x2ff000);u.reg_write(UC_ARM_REG_LR, 0x2fe000)
            for reg, arg in zip((UC_ARM_REG_R0,UC_ARM_REG_R1),args):u.reg_write(reg,arg)
            u.emu_start(symbols[name],0x2fe000,count=2000000)
            assert u.reg_read(UC_ARM_REG_PC)==0x2fe000, 'instruction budget exhausted'
            return u.reg_read(UC_ARM_REG_R0)

        count = 0
        for origin in range(2):
            for blend in range(3):
                for textured in range(2):
                    c = Config(64,64,0x06600000,64*textured,32*textured,0x06610000*textured,origin,blend,0)
                    v = (Vertex*3)(Vertex(8,8,255,0,0,255,0,0,1),
                        Vertex(40,8,0,255,0,128,128,0,.5),Vertex(8,40,0,0,255,64,0,32,.25))
                    p = Packet()
                    u.mem_write(0x200000, bytes(C.sizeof(p)))
                    u.mem_write(0x201000, bytes(c));u.mem_write(0x202000, bytes(v))
                    host_rc=lib.grx_begin(C.byref(p),C.byref(c))
                    arm_rc=call('grx_begin',0x200000,0x201000)
                    assert host_rc==arm_rc==0, (abi,origin,blend,textured,host_rc,arm_rc)
                    assert lib.grx_triangle(C.byref(p),v)==call('grx_triangle',0x200000,0x202000)==0
                    assert lib.grx_end(C.byref(p))==call('grx_end',0x200000)==0
                    arm = bytes(u.mem_read(0x200000, C.sizeof(p)))
                    assert struct.unpack_from('<I',arm,992)[0]==p.words
                    assert arm[:p.words*4]==bytes(p.bytes)[:p.words*4], (abi,origin,blend,textured)
                    count += 1
        checked[abi] = count
    print('PASS A9 instruction model: bit-identical packets', checked)
    inputs=[Path(__file__), SRC/'translator.c', SRC/'translator.h', SRC/'vertex.h', SRC/'descriptor.c', SRC/'descriptor.h', SRC/'pipeline.h']
    (OUT/'arm-model.json').write_text(json.dumps({'scope':'instruction model, not physical GPU',
        'bit_identical_batches':checked,
        'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs}}, indent=2)+'\n')


if __name__ == '__main__':
    main()
