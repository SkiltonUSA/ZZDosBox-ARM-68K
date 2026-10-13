#!/usr/bin/env python3
"""Linked PIC ARM shell with model 68k console and live file service.

Instruction-model evidence only. AmigaDOS backend and physical lifecycle must
be verified separately. Optional --vette reads user-supplied, gitignored data.
"""
import argparse,importlib.util,struct,subprocess,re
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3];OUT=ROOT/'.context/amiga/dosbox-shell'
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
RXW,RXR,TXW,TXR=0x184000,0x184040,0x184080,0x1840c0
RX,TX,CMD,REPLY,NAME,NAME2,DATA=0x184400,0x185000,0x186000,0x186040,0x186080,0x186090,0x187000
pattern=bytes(i%251 for i in range(100003))
commands=b'dir /b\rtype readme.txt\recho saved text > SAVED.TXT\rtype saved.txt\rfiletest\rexetest\rcopy LARGE.BIN LARGE2.BIN\rren LARGE2.BIN RENAMED.BIN\rdir /b\rexit\r'
def run(base,mode):
 image,rel,_=pack.unpack((OUT/'dosbox.elf').read_bytes())
 for off in rel:struct.pack_into('<I',image,off,struct.unpack_from('<I',image,off)[0]+base)
 symbols={}
 for line in subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm','-n',str(OUT/'dosbox.elf')],text=True).splitlines():
  f=line.split()
  if len(f)==3:symbols[f[2]]=int(f[0],16)+base
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 u.mem_map(base&~4095,0x2002000);u.mem_write(base,bytes(image));u.mem_write(symbols['_ZL6shared'],struct.pack('<I',base))
 def put(off,n):u.mem_write(base+off,struct.pack('>I',n&0xffffffff))
 def get(off):return struct.unpack('>I',u.mem_read(base+off,4))[0]
 for off in [RXW,RXR,TXW,TXR]:put(off,0xfffffffc)
 u.mem_write(base+0x1d0000,bytes.fromhex('00009fe51eff2fe100000000'));u.mem_write(base+0x1d0100,bytes.fromhex('0100a0e31eff2fe1'))
 bad=[]
 def access(u,kind,address,size,value,data):
  if not base<=address<=base+0x2000000-size or (size>1 and address%size):bad.append((hex(address),size,hex(u.reg_read(UC_ARM_REG_PC)-base)));u.emu_stop()
 if mode in ('vette','keen'):
  # Games spend far longer in x86 unpackers. Guard allocation edges only here;
  # normal shell cases above retain the full alignment/address instrumentation.
  u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base&~4095,end=base-1)
  u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base+0x2000000,end=(base&~4095)+0x2002000-1)
 else:u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access)
 for reg,val in [(UC_ARM_REG_SP,base+0x1fffff0),(UC_ARM_REG_LR,base+0x1ff0000),(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,base+0x1e0000),(UC_ARM_REG_R2,base+0x1d0000),(UC_ARM_REG_R3,base+0x1d0100)]:u.reg_write(reg,val)
 files={'FILETEST.COM':(OUT/'FileTest.com').read_bytes(),'EXETEST.EXE':(OUT/'ExeTest.exe').read_bytes(),'INPUT.TXT':(OUT/'INPUT.TXT').read_bytes(),'README.TXT':b'Live C drive\r\n','LARGE.BIN':pattern}
 for i in range(10):files[f'DATA{i}.TXT']=b'entry\r\n'
 keys=commands
 if mode in ('vette','keen'):
  folder,exe=('VETTE','vette') if mode=='vette' else ('KEEN6','keen6c')
  files={p.name:p.read_bytes() for p in (OUT/'Games'/folder).iterdir()}
  files['LOADFIX.COM']=(OUT/'LoadFix.com').read_bytes()
  keys=(('loadfix vette.exe' if mode=='vette' else exe)+'\rexit\r').encode()
 handles={};calls=read=written=0;output=bytearray();sent=0;stopping=False;seen_request=0
 def name(off):
  text=bytes(u.mem_read(base+off,13)).split(b'\0')[0].decode('ascii')
  assert re.fullmatch(r'[A-Z0-9_-]{1,8}(\.[A-Z0-9_-]{1,3})?',text),text
  return text
 pc=symbols['mesen_test']
 for tick in range(20000):
  u.emu_start(pc,base+0x1ff0000,count=100000);pc=u.reg_read(UC_ARM_REG_PC);assert not bad,bad
  used=(get(TXW)-get(TXR))&0xffffffff;assert used<=4096
  r=get(TXR)
  for i in range(used):output+=u.mem_read(base+TX+((r+i)&4095),1)
  put(TXR,r+used)
  if mode!='read-stop' and sent<len(keys):
   w=get(RXW);space=256-((w-get(RXR))&0xffffffff);data=keys[sent:sent+space]
   for i,c in enumerate(data):u.mem_write(base+RX+((w+i)&255),bytes([c]))
   put(RXW,w+len(data));sent+=len(data)
  seq=get(CMD)
  if seq!=get(REPLY):
   assert seq==seen_request+1 or (mode=='fs-stop' and seq==seen_request);seen_request=seq
   if mode=='fs-stop':stopping=True
   else:
    op,h,n,pos,whence=[get(CMD+i*4) for i in range(1,6)];error=result=size=attr=0;calls+=1
    path=name(NAME) if op in (1,2,7,9,10) else None
    if op in (1,2):
     if op==2:files[path]=b''
     if path not in files:error=2
     elif len(handles)>=16:error=4
     else:result=next(k for k in range(1,17) if k not in handles);handles[result]=[path,0,n if op==1 else 2]
    elif op in (3,4,5,6):
     assert h in handles,(op,h,handles)
     path,at,access=handles[h]
     if op==3:del handles[h]
     elif op==4:
      assert n<=4096 and access!=1;data=files[path][at:at+n];u.mem_write(base+DATA,data or b'\0');result=len(data);handles[h][1]+=result;read+=result
     elif op==5:
      assert n<=4096 and access!=0
      data=bytes(u.mem_read(base+DATA,n));old=files[path].ljust(at,b'\0');files[path]=old[:at]+data+(old[at+n:] if n else b'');result=n;handles[h][1]+=n;written+=n
     else:
      displacement=pos if pos<0x80000000 else pos-0x100000000
      result=displacement+(0 if whence==0 else at if whence==1 else len(files[path]));assert result>=0;handles[h][1]=result
    elif op==7:
     if path not in files:error=2
     else:size=len(files[path]);attr=32
    elif op==8:
     names=sorted(files)
     if h>=len(names):error=18
     else:path=names[h];size=len(files[path]);attr=32;u.mem_write(base+NAME,path.encode().ljust(13,b'\0'))
    elif op==9:
     if path not in files:error=2
     else:del files[path]
    elif op==10:files[name(NAME2)]=files.pop(path)
    elif op==11:result=32767;size=16384
    elif op==14:
     directory=bytes(u.mem_read(base+CMD+256,256)).split(b"\0")[0]
     if directory:error=3
    else:raise AssertionError(op)
    for i,v in enumerate([error,result,size,attr,0x5c21,0],1):put(REPLY+i*4,v)
    put(REPLY,seq)
  if mode=='read-stop' and b'C:\\>' in output:stopping=True
  if stopping:u.mem_write(base+0x1d0008,struct.pack('<I',1))
  if pc==base+0x1ff0000:break
 else:raise AssertionError(('worker timeout',output[-200:],hex(pc-base)))
 result=struct.unpack('<11I',u.mem_read(base+0x1e0000,44));rc=u.reg_read(UC_ARM_REG_R0)
 detail=bytes(u.mem_read(base+0x180900+128,160)).split(b'\0')[0]
 if mode=='normal':
  assert rc==0 and result[4]==0,(rc,result,detail,output)
  assert b'ZZDOS files/handles/seek PASS' in output and b'ZZDOS MZ relocation/memory PASS' in output,output
  assert files['RENAMED.BIN']==pattern and files['SAVED.TXT']==b'saved text\r\n'
  assert not handles,handles
 elif mode in ('vette','keen'):
  assert (rc==1 and result[4]==1) or (mode=='vette' and rc==0 and b'Packed file is corrupt' in output),(rc,result,detail,output)
  print(mode+' startup:',detail.decode(errors='replace'),'read',read,'bytes',flush=True)
 else:assert rc==2 and result[4]==3 and stopping,(mode,rc,result,detail)
 (OUT/f'model-{mode}.txt').write_bytes(output+b'\n'+detail+b'\n')
 print('PASS ARM shell',hex(base),mode,'requests',calls,'read',read,'written',written,'rc',rc,flush=True)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--vette',action='store_true');p.add_argument('--keen',action='store_true');args=p.parse_args()
 for base in [0x12000040,0x150020c0]:
  for mode in (['vette'] if args.vette else ['keen'] if args.keen else ['normal','read-stop','fs-stop']):run(base,mode)
