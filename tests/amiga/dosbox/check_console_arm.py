#!/usr/bin/env python3
"""Run the linked console worker with a simulated, delayed 68k ring consumer."""
import importlib.util,struct,subprocess
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3];OUT=ROOT/'.context/amiga/dosbox-console'
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
elf=OUT/'dosbox.elf'
RXW,RXR,TXW,TXR,EOF=0x184000,0x184040,0x184080,0x1840c0,0x184100
RX,TX=0x184400,0x185000
expected=b'READY\r\nQAB\b \bXC\r'+b'~'*5000+b'\r\nCONSOLE PASS\r\n'
def run(base,mode):
 image,rel,_=pack.unpack(elf.read_bytes())
 for off in rel:struct.pack_into('<I',image,off,struct.unpack_from('<I',image,off)[0]+base)
 symbols={}
 for line in subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm','-n',str(elf)],text=True).splitlines():
  fields=line.split()
  if len(fields)==3:symbols[fields[2]]=int(fields[0],16)+base
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 u.mem_map(base&~4095,0x2002000);u.mem_write(base,bytes(image));u.mem_write(symbols['_ZL6shared'],struct.pack('<I',base))
 def put(off,n):u.mem_write(base+off,struct.pack('>I',n&0xffffffff))
 def get(off):return struct.unpack('>I',u.mem_read(base+off,4))[0]
 program=(OUT/'IoTest.com').read_bytes();u.mem_write(base+0x190000,program)
 put(0x180800+12,len(program));put(0x180800+16,0)
 for off in [RXW,RXR,TXW,TXR]:put(off,0xfffffffc)
 u.mem_write(base+0x1d0000,bytes.fromhex('00009fe51eff2fe100000000'));u.mem_write(base+0x1d0100,bytes.fromhex('0100a0e31eff2fe1'))
 bad=[]
 def access(u,kind,address,size,value,data):
  if not base<=address<=base+0x2000000-size or (size>1 and address%size):bad.append((hex(address),size,hex(u.reg_read(UC_ARM_REG_PC)-base)));u.emu_stop()
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access)
 for reg,val in [(UC_ARM_REG_SP,base+0x1fffff0),(UC_ARM_REG_LR,base+0x1ff0000),(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,base+0x1e0000),(UC_ARM_REG_R2,base+0x1d0000),(UC_ARM_REG_R3,base+0x1d0100)]:u.reg_write(reg,val)
 pc=symbols['mesen_test'];output=bytearray();sent=False;stopping=False;saw_full=False;at_ready=0
 for tick in range(3000):
  u.emu_start(pc,base+0x1ff0000,count=100000);pc=u.reg_read(UC_ARM_REG_PC)
  assert not bad,bad
  used=(get(TXW)-get(TXR))&0xffffffff
  if used==4096:saw_full=True
  # Deliberately allow the TX ring to fill before draining it.
  if mode!='write-stop' or not sent or stopping:
   if used==4096 or (not sent and used>=7) or pc==base+0x1ff0000:
    r=get(TXR)
    for i in range(used):output+=u.mem_read(base+TX+((r+i)&4095),1)
    put(TXR,r+used)
  if output.startswith(b'READY\r\n') and not sent:
   at_ready+=1
   if at_ready>=3:
    if mode=='read-stop':stopping=True
    else:
     w=get(RXW);keys=b'ZQAB\bXC\r'
     for i,c in enumerate(keys):u.mem_write(base+RX+((w+i)&255),bytes([c]))
     put(RXW,w+len(keys))
    sent=True
  if mode=='write-stop' and saw_full:stopping=True
  if stopping:u.mem_write(base+0x1d0008,struct.pack('<I',1))
  if pc==base+0x1ff0000:break
 else:raise AssertionError('Worker did not return')
 result=struct.unpack('<11I',u.mem_read(base+0x1e0000,44));rc=u.reg_read(UC_ARM_REG_R0)
 if mode=='normal':
  assert rc==0 and result[7]==0 and result[4]==0,(rc,result)
  assert output==expected,(len(output),output[-40:])
  assert saw_full and result[8]==len(expected)
 else:assert rc==2 and result[4]==3 and stopping,(mode,rc,result)
 print('PASS ARM console',hex(base),mode,'bytes',len(output),'full',saw_full,'rc',rc,flush=True)
for base in [0x12000040,0x150020c0]:
 for mode in ['normal','read-stop','write-stop']:run(base,mode)
