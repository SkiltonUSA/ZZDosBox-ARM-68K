#!/usr/bin/env python3
"""Execute every generated OVL gateway on an x86 instruction model.

This checks machine code, register preservation and stdcall cleanup, not
DOS/4G loading or ARM/FPGA execution (those need independent tests).
"""
from pathlib import Path
import importlib.util,re,struct
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_INSN
from unicorn.x86_const import *
ROOT=Path(__file__).resolve().parents[4];OUT=ROOT/'.context/amiga/glide/ovl'
spec=importlib.util.spec_from_file_location('ovl',ROOT/'scripts/build_dosbox_glide_ovl.py');ovl=importlib.util.module_from_spec(spec);spec.loader.exec_module(ovl)
calls=ovl.read_api(ROOT/'amiga/dosbox/glide/abi/glide2x.api')
data=(OUT/'GLIDE2X.OVL').read_bytes();h=struct.unpack_from('<I',data,0x3c)[0]
def word(o):return struct.unpack_from('<I',data,h+o)[0]
pages=word(0x80);obj=h+word(0x40);size,base,flags,index,count,_=struct.unpack_from('<6I',data,obj)
assert flags&4 and index==1
code=data[pages:pages+size]
symbols={int(n):int(a,16) for a,n in re.findall(r'^0001:([0-9a-fA-F]+)  zz_([0-9]+)$',(OUT/'glide.map').read_text(),re.M)}
assert len(symbols)==len(calls)
for present in (False,True):
 for id,c in enumerate(calls,1):
  u=Uc(UC_ARCH_X86,UC_MODE_32);u.mem_map(0x100000,0x10000);u.mem_write(0x100000,code)
  u.mem_map(0x200000,0x10000);stack=0x208000;stop=0x10f000
  u.mem_write(stack,struct.pack('<I',stop)+bytes(range(c['bytes'])))
  u.reg_write(UC_X86_REG_ESP,stack)
  saved={UC_X86_REG_EBX:0x12345678,UC_X86_REG_ESI:0x87654321,UC_X86_REG_EDI:0x98765432,UC_X86_REG_EBP:0x207000}
  for r,v in saved.items():u.reg_write(r,v)
  seen=[]
  def inp(uc,port,width,_):
   assert width==4
   assert port in (0x600,0x604)
   return (0x5a474c31 if present else 0xffffffff) if port==0x600 else 0
  def out(uc,port,width,value,_):
   assert port==0x604 and width==4 and value==id
   address=uc.reg_read(UC_X86_REG_ECX)
   assert address==stack+4
   assert bytes(uc.mem_read(address,c['bytes']))==bytes(range(c['bytes'])) if c['bytes'] else True
   seen.append(value)
  u.hook_add(UC_HOOK_INSN,inp,None,1,0,UC_X86_INS_IN)
  u.hook_add(UC_HOOK_INSN,out,None,1,0,UC_X86_INS_OUT)
  u.emu_start(0x100000+symbols[id],stop,count=100)
  assert u.reg_read(UC_X86_REG_EIP)==stop
  assert u.reg_read(UC_X86_REG_ESP)==stack+4+c['bytes']
  assert u.reg_read(UC_X86_REG_EAX)==0
  if c['return']=='float':
   assert (u.reg_read(UC_X86_REG_FPSW)>>11)&7==7
   assert u.reg_read(UC_X86_REG_ST0)==0
  assert seen==([id] if present else [])
  for r,v in saved.items():assert u.reg_read(r)==v
 print('PASS OVL x86: 146 gateways, bridge '+('present' if present else 'absent')+', stdcall cleanup and preserved registers')
