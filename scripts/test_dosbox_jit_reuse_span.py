#!/usr/bin/env python3
"""Execute original and register-reused A32 streams, including intervening writes."""
import ctypes, random, struct, subprocess, tempfile
from pathlib import Path
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_ARM
from unicorn.arm_const import *
root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as directory:
    p = Path(directory)
    (p/'bridge.c').write_text('#include "jit_reuse.h"\nint scan(const uint32_t *p,unsigned n,uint32_t load){return db_arm_reuse_register(p,n,load);}\n')
    subprocess.run(['clang','-shared','-O2','-I'+str(root/'amiga/dosbox/pc'),str(p/'bridge.c'),'-o',str(p/'bridge.dylib')],check=True)
    lib = ctypes.CDLL(str(p/'bridge.dylib')); scan = lib.scan
    scan.argtypes = [ctypes.POINTER(ctypes.c_uint32),ctypes.c_uint,ctypes.c_uint32]; scan.restype = ctypes.c_int
    def find(code, load=0xe59a3000):
        return scan((ctypes.c_uint32*len(code))(*code),len(code),load)
    # r0 survives arithmetic on r1; this is missed by adjacent-only forwarding.
    assert find([0xe58a0000,0xe2811001]) == 0
    assert find([0xe59a0000,0xe2811001]) == 0
    assert find([0xe58a0000,0xe5801000]) == -1  # unknown/aliased write
    assert find([0xe58a0000,0xe58a1001]) == -1  # overlapping word write
    assert find([0xe58a0000,0xe58a1004]) == 0
    assert find([0xe58a0000,0xe2800001]) == -1  # source overwritten
    assert find([0xe58a0000,0xe28aa004]) == -1  # base overwritten
    assert find([0xe58a0000,0xe59aa004]) == -1
    assert find([0xe58a0000,0xe12fff1e]) == -1
    assert find([0xe58a0000,0xeb000000]) == -1
    assert find([0xe58a0000,0x12811001]) == -1  # conditional path
    assert find([0xe58a0000,0xe5ca1000]) == -1  # partial write
    assert find([0xe58a0000,0xe1ca10b0]) == -1
    assert find([0xe58a0000,0xe1a0f001]) == -1
    assert find([0xe58a0000,0xe10f1000]) == -1  # special, not arithmetic
    assert find([0xe58a0000,0xe0000192]) == -1  # multiply
    assert find([0xe58a0000,0xe3000123]) == -1  # MOVW source
    assert find([0xe58a0000,0xe3400123]) == -1  # MOVT source
    assert find([0xe58a0000,0xe3001123,0xe3401123]) == 0
    assert find([],0xe59a3000) == -1
    assert find([0xe58a0000],0x159a3000) == -1
    # All source/dest registers, signed offsets, clobbers and live NZCV flags.
    u = Uc(UC_ARCH_ARM,UC_MODE_ARM); u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
    u.mem_map(0x100000,4096); u.mem_map(0x300000,16384)
    rng = random.Random(2901); count = hits = 0
    registers = [UC_ARM_REG_R0+i for i in range(13)]+[UC_ARM_REG_SP,UC_ARM_REG_LR]
    def run(code,values,flags):
        u.mem_write(0x300000,bytes(16384))
        u.mem_write(0x100000,struct.pack('<'+'I'*len(code),*code))
        u.ctl_remove_cache(0x100000,0x101000)
        for reg,value in zip(registers,values): u.reg_write(reg,value)
        u.reg_write(UC_ARM_REG_R10,0x301000); u.reg_write(UC_ARM_REG_CPSR,flags|0x13)
        u.emu_start(0x100000,0x100000+4*len(code))
        return [u.reg_read(reg) for reg in registers],u.reg_read(UC_ARM_REG_CPSR),bytes(u.mem_read(0x300000,16384))
    for case in range(6000):
        src,dst = rng.randrange(4),rng.randrange(4)
        sign,offset = rng.choice([0,0x800000]),rng.choice([0,1,2,3,4,127,1024,4092])
        code = [0xe50a0000|sign|src<<12|offset]
        for j in range(rng.randrange(8)):
            r = rng.randrange(4); k = rng.randrange(4)
            code.append(rng.choice([0xe2800001|r<<16|r<<12, 0xe2500001|r<<16|r<<12,
                0xe1a00000|r<<12|k, 0xe0800000|r<<16|r<<12|k,
                0xe3500000|r<<16, 0xe3000123|r<<12,0xe3400123|r<<12,
                0xe59a0000|r<<12|rng.choice([0,4,8]),0xe58a0000|r<<12|rng.choice([0,4,8])]))
        load = 0xe51a0000|sign|dst<<12|offset; source = find(code,load)
        values = [rng.getrandbits(32) for _ in range(15)]; flags = rng.randrange(16)<<28
        replacement = [] if source==dst else [0xe1a00000|dst<<12|source] if source>=0 else [load]
        assert run(code+[load],values,flags) == run(code+replacement,values,flags),(code,load,source)
        count += 1; hits += source>=0
    assert hits>1000
print(f'PASS bounded register reuse: {count} A9 differential streams, {hits} replacements; alias, clobber and control boundaries')
