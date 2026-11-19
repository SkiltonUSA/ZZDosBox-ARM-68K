#!/usr/bin/env python3
"""Check the exact benchmark payload and FP restore; no model timing claim."""
import json,struct,subprocess,sys
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_CODE
from unicorn import arm_const as A
ROOT=Path(__file__).resolve().parents[4]
sys.path.insert(0,str(ROOT/'scripts'))
from build_zz9000_debug import unpack_elf
OUT=ROOT/'.context/amiga/glide/capacity'
image,rel,entry=unpack_elf((OUT/'capacity.elf').read_bytes(),0x40000)
symbols={v[2]:int(v[0],16) for line in subprocess.check_output(
    ['arm-none-eabi-nm',OUT/'capacity.elf'],text=True).splitlines() if len(v:=line.split())==3}
expected=int(json.loads((OUT/'build.json').read_text())['expected_hash'],16)
base=0x11000000
u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(A.UC_CPU_ARM_CORTEX_A9)
u.mem_map(base,0x100000)
for offset in rel:struct.pack_into('<I',image,offset,struct.unpack_from('<I',image,offset)[0]+base)
u.mem_write(base,bytes(image))
# Mock feature discovery only: Unicorn's CP15/MVFR reports differ from the
# physical A9. Arithmetic and full FP save/restore execute from the built payload.
# Hardware availability is checked by the unmodified helper in the real run.
def available(uc,address,size,user):
    uc.reg_write(A.UC_ARM_REG_R0,0 if case=='unavailable' else 1)
    uc.reg_write(A.UC_ARM_REG_PC,uc.reg_read(A.UC_ARM_REG_LR))
u.hook_add(UC_HOOK_CODE,available,begin=base+symbols['db_neon_available'],
           end=base+symbols['db_neon_available'])
u.mem_write(base+0xf0000,bytes.fromhex('0100a0e31eff2fe1')) # return true, ARM state
for case in ['normal','cancel','unavailable']:
    u.mem_write(base+0x90000,bytes(44))
    u.reg_write(A.UC_ARM_REG_C1_C0_2,0 if case=='unavailable' else 0xf00000)
    u.reg_write(A.UC_ARM_REG_FPEXC,0x40000000)
    u.reg_write(A.UC_ARM_REG_FPSCR,0x01000000)
    original=[0x1234567812345678+i for i in range(32)]
    for i,v in enumerate(original):u.reg_write(getattr(A,'UC_ARM_REG_D'+str(i)),v)
    for reg,value in [(A.UC_ARM_REG_R0,1),(A.UC_ARM_REG_R1,base+0x90000),
                      (A.UC_ARM_REG_R2,base+0xf0000 if case=='cancel' else 0),
                      (A.UC_ARM_REG_R3,0),(A.UC_ARM_REG_SP,base+0xfe000),
                      (A.UC_ARM_REG_LR,base+0xff000)]:u.reg_write(reg,value)
    u.emu_start(base+symbols['mesen_test'],base+0xff000,count=20000000)
    assert u.reg_read(A.UC_ARM_REG_PC)==base+0xff000,'instruction budget exhausted'
    assert u.reg_read(A.UC_ARM_REG_R0)=={'normal':0,'cancel':2,'unavailable':6}[case], (case,u.reg_read(A.UC_ARM_REG_R0))
    assert [u.reg_read(getattr(A,'UC_ARM_REG_D'+str(i))) for i in range(32)]==original
    assert u.reg_read(A.UC_ARM_REG_FPSCR)==0x01000000
    if case=='normal':
        result=struct.unpack('<11I',u.mem_read(base+0x90000,44))
        assert result[:7]==(320,0,320,expected,0,0,0),result
    print('PASS compiled A9 capacity:',case,'FP context retained')
