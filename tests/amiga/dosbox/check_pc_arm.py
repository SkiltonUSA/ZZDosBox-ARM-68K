#!/usr/bin/env python3
"""Linked ARM PC-device test with model file and framebuffer handoffs.

Instruction-model evidence only. AmigaDOS backend and physical lifecycle must
be verified separately. Uses an original EGA/PIT fixture, no game data.
"""
import argparse,importlib.util,struct,subprocess,re
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3];OUT=ROOT/'.context/amiga/dosbox-pc'
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
RXW,RXR,TXW,TXR=0x184000,0x184040,0x184080,0x1840c0
RX,TX,CMD,REPLY,NAME,NAME2,DATA=0x184400,0x185000,0x186000,0x186040,0x186080,0x186090,0x187000
pattern=bytes(i%251 for i in range(100003))
commands=b'dir /b\rtype readme.txt\recho saved text > SAVED.TXT\rtype saved.txt\rfiletest\rexetest\rcopy LARGE.BIN LARGE2.BIN\rren LARGE2.BIN RENAMED.BIN\rdir /b\rexit\r'
def run(base,mode,profile=False,options=None):
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
 if profile or mode=="vga-shell":
  # Synthetic clock for accounting checks only, never a speed measurement.
  u.mem_map(0xf8f00000,4096);timer=[0xfffffff0]
  u.mem_write(0xf8f00200,struct.pack('<Q',timer[0]))
  def timer_read(u,kind,address,size,value,data):
   if address==0xf8f00200:
    timer[0]+=37;u.mem_write(0xf8f00200,struct.pack('<Q',timer[0]))
  u.hook_add(UC_HOOK_MEM_READ,timer_read,begin=0xf8f00200,end=0xf8f00207)
  put(0x188080,int(profile))
 for off in [RXW,RXR,TXW,TXR]:put(off,0xfffffffc)
 u.mem_write(base+0x1d0000,bytes.fromhex('00009fe51eff2fe100000000'));u.mem_write(base+0x1d0100,bytes.fromhex('0100a0e31eff2fe1'))
 bad=[]
 def access(u,kind,address,size,value,data):
  if not base<=address<=base+0x2000000-size or (size>1 and address%size):bad.append((hex(address),size,hex(u.reg_read(UC_ARM_REG_PC)-base)));u.emu_stop()
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base&~4095,end=base-1)
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base+0x2000000,end=(base&~4095)+0x2002000-1)
 width,height=640,480
 frame=0x00fff040;pitch=3840 if mode=="resize" else 2560;span=(height-1)*pitch+width*4;fmt=3 if base==0x12000040 else 4
 u.mem_map(frame&~4095,0x2b0000 if mode=="resize" else 0x140000)
 l1=(base+0x100000+16383)&~16383
 u.mem_write(base+0x182000+15*4,struct.pack('<I',l1))
 put(0x188040,1000);put(0x188044,0)
 if options:
  put(0x188100,0x50434f31)
  for i,v in enumerate(options,1):put(0x188100+i*4,v)
 if mode=='keyboard':
  u.mem_write(base+0x1881c0,bytes([0x20,0xa0]));put(0x188140,2)

 copy_stopped=[False]
 if mode=='copy-stop':
  def stop_copy(u,kind,address,size,value,data):
   if not copy_stopped[0]:
    copy_stopped[0]=True;put(0x180080,1);u.mem_write(base+0x1d0008,struct.pack('<I',1))
  u.hook_add(UC_HOOK_MEM_WRITE,stop_copy,begin=frame,end=frame+span-1)
 frame_seq=0;captured=None
 for reg,val in [(UC_ARM_REG_SP,base+0x1fffff0),(UC_ARM_REG_LR,base+0x1ff0000),(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,base+0x1e0000),(UC_ARM_REG_R2,base+0x1d0000),(UC_ARM_REG_R3,base+0x1d0100)]:u.reg_write(reg,val)
 files={'EGATEST.COM':(OUT/'EGATEST.COM').read_bytes()};keys=b'egatest\rexit\r'
 if mode=='vga-shell':
  keys=b'';put(0x1882c0,3);put(0x1882c8,7);put(0x188040,0)
 if mode in ('cga','keyboard'):
  file='CGATEST.COM' if mode=='cga' else 'KEYTEST.COM';files={file:(OUT/file).read_bytes()};keys=file.encode()+b'\rexit\r'
 if mode=='cpu':
  files={name:(OUT/source).read_bytes() for name,source in [('DIAG.COM','Diagnostic.com'),('FILETEST.COM','FileTest.com'),('EXETEST.EXE','ExeTest.exe'),('INPUT.TXT','INPUT.TXT')]}
  keys=b'diag\rfiletest\rexetest\rexit\r'
 if mode in ('guest-bound','high-bank-bound'):
  invalid=bytearray((OUT/'BADMEM.COM').read_bytes())
  if mode=='guest-bound':struct.pack_into('<I',invalid,2,0x01000000)
  files={'BADMEM.COM':bytes(invalid)};keys=b'badmem\r'
 if mode=='boot':
  files={'BOOTTEST.IMG':(OUT/'BOOTTEST.IMG').read_bytes()};keys=b''
  u.mem_write(base+0x188240,b'BOOTTEST.IMG\0');put(0x188040,100)
 if mode=='paging':
  files={'PAGING.IMG':(OUT/'PAGINGTEST.COM').read_bytes()+bytes(1440*1024-512)};keys=b''
  u.mem_write(base+0x188240,b'PAGING.IMG\0');put(0x188040,100)
 if mode=='hdd':
  files={'HDD.IMG':(OUT/'HDDTEST.COM').read_bytes()+bytes(32*1024*1024-512)};keys=b''
  u.mem_write(base+0x188280,b'HDD.IMG\0');put(0x188040,100)
 if mode=='cd':
  from make_cd_fixture import make
  iso_path=OUT/'TEST.ISO';make(iso_path);cd_image=iso_path.read_bytes();put(0x1883b0,len(cd_image)//2048)
  files={};keys=b'type d:\\hello.txt\rcopy d:\\large.bin c:\\CDLARGE.BIN\rexit\r'
 if mode=='midi':
  files={'MIDITEST.COM':(OUT/'MIDITEST.COM').read_bytes()};keys=b'miditest\rexit\r';put(0x1883c0,1);put(0x18a000,0xffffff00);put(0x18a040,0xffffff00)
 midi_bytes=bytearray()
 raw_sent=False
 disk_open=False
 handles={};calls=read=written=0;output=bytearray();sent=0;stopping=False;seen_request=0
 def name(off):
  text=bytes(u.mem_read(base+off,13)).split(b'\0')[0].decode('ascii')
  assert re.fullmatch(r'[A-Z0-9_-]{1,8}(\.[A-Z0-9_-]{1,3})?',text),text
  return text
 def bad_code(u,address,size,data):
  raise AssertionError(('execution outside text',hex(address-base),'lr',hex(u.reg_read(UC_ARM_REG_LR)-base),'sp',hex(u.reg_read(UC_ARM_REG_SP)-base)))
 u.hook_add(UC_HOOK_CODE,bad_code,begin=base+len(image),end=base+0x1cffff)
 u.hook_add(UC_HOOK_CODE,bad_code,begin=base+0x200000,end=base+0x1feffff)
 pc=symbols['mesen_test']
 for tick in range(50000):
  # Preserve Thumb state when resuming a quantum inside the soft-float library.
  u.emu_start(pc | (1 if u.reg_read(UC_ARM_REG_CPSR)&32 else 0),base+0x1ff0000,count=100000);pc=u.reg_read(UC_ARM_REG_PC);assert not bad,bad
  mr=get(0x18a040);mw=get(0x18a000);mn=(mw-mr)&0xffffffff;assert mn<=4096
  for mi in range(mn):midi_bytes+=u.mem_read(base+0x18b000+((mr+mi)&4095),1)
  put(0x18a040,mw)
  used=(get(TXW)-get(TXR))&0xffffffff;assert used<=4096
  r=get(TXR)
  for i in range(used):output+=u.mem_read(base+TX+((r+i)&4095),1)
  put(TXR,r+used)
  if mode!='read-stop' and sent<len(keys):
   w=get(RXW);space=256-((w-get(RXR))&0xffffffff);data=keys[sent:sent+space]
   for i,c in enumerate(data):u.mem_write(base+RX+((w+i)&255),bytes([c]))
   put(RXW,w+len(data));sent+=len(data)
  if mode=='vga-shell' and frame_seq>=2 and not raw_sent:
   # Type VER then EXIT through the production Amiga-raw-key -> BIOS -> shell path.
   raw=[0x34,0xb4,0x12,0x92,0x13,0x93,0x44,0xc4,0x12,0x92,0x32,0xb2,0x17,0x97,0x14,0x94,0x44,0xc4]
   u.mem_write(base+0x1881c0,bytes(raw));put(0x188140,len(raw));raw_sent=True
  vseq=get(0x188000)
  if vseq!=frame_seq:
   assert vseq==frame_seq+1
   if mode=="speed":put(0x1883c4,4000 if vseq<4 else 2500)
   if mode=="resize":
    width,height=[(320,240),(960,720),(508,381),(640,480)][(vseq-1)%4]
    span=(height-1)*pitch+width*4
    put(0x18802c,width);put(0x188030,height)
   nonce=vseq^0x12345678
   u.mem_write(frame,b'\x00'*(span+4))
   u.mem_write(frame,struct.pack('>I',nonce ^ (1 if mode=='badnonce' else 0)));u.mem_write(frame+span-4,struct.pack('>I',nonce^0x31415926))
   for off,val in [(20,frame),(24,pitch),(28,fmt),(32,nonce),(16,vseq)]:put(0x188000+off,val)
   frame_seq=vseq
  if frame_seq and get(0x188024)==frame_seq:
   assert get(0x188028)==(7 if mode=='badnonce' else 2 if mode=='copy-stop' else 0)
   captured=bytes(u.mem_read(frame,span))
   assert bytes(u.mem_read(frame+span,4))==b'\0'*4
  seq=get(CMD)
  if seq!=get(REPLY):
   assert seq==seen_request+1 or (mode=='fs-stop' and seq==seen_request);seen_request=seq
   if mode=='fs-stop':stopping=True
   else:
    op,h,n,pos,whence=[get(CMD+i*4) for i in range(1,6)];error=result=size=attr=0;calls+=1
    path=name(NAME) if op in (1,2,7,9,10,15) else None
    if op==19:
     assert mode=='cd' and n==1 and pos<len(cd_image)//2048
     u.mem_write(base+DATA,cd_image[pos*2048:(pos+1)*2048]);result=n;read+=2048
    elif op==15:
     assert path=='HDD.IMG' and not disk_open;disk_open=True;result=65536
    elif op in (17,18):
     assert disk_open and 1<=n<=8 and pos<65536 and pos+n<=65536
     at=pos*512;count=n*512
     if op==17:u.mem_write(base+DATA,files['HDD.IMG'][at:at+count]);read+=count
     else:
      old=files['HDD.IMG'];files['HDD.IMG']=old[:at]+bytes(u.mem_read(base+DATA,count))+old[at+count:];written+=count
     result=n
    elif op in (1,2):
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
 if profile:
  assert get(0x188084)==0x50524631 and get(0x188094)==0
  total=(get(0x188088)<<32)|get(0x18808c)
  buckets=[(get(0x188098+i*16)<<32)|get(0x18809c+i*16) for i in range(6)]
  assert total>0 and sum(buckets)==total,(total,buckets)
  print('PASS profile publication and exclusive totals',mode,'unwound',get(0x188090),flush=True)
 if mode=='cd':
  assert rc==0 and b'ZZCD data read PASS' in output and files['CDLARGE.BIN']==bytes(i%251 for i in range(5003)),(rc,result,detail,output[-300:])
  print('PASS ARM CD ISO9660 file read and cross-sector copy',flush=True);return
 if mode=='midi':
  assert rc==0 and b'MPU401 intelligent/UART PASS' in output,(rc,result,detail,output[-300:])
  assert midi_bytes==bytes([0xf8])*5000+bytes([0x91,60,64,62,65,0xf8,0x81,60,0,0xf0,0x7d,1,2,3,0xf7])
  print('PASS ARM MPU-401 and bounded MIDI stream/wrap',flush=True);return
 if mode=='boot':
  marker=struct.unpack('<H',u.mem_read(symbols['_ZL3ram']+0x500,2))[0]
  assert rc==2 and result[4]==3 and marker==0xb007 and not handles,(rc,result,hex(marker),detail)
  print('PASS ARM BIOS boot/read/bounds, A20 aliasing, ROM protection and timed stop',flush=True);return
 if mode=='paging':
  marker,faults=struct.unpack('<II',u.mem_read(symbols['_ZL3ram']+0x500,8))
  assert rc==2 and result[4]==3 and marker==0xfa17 and faults==1 and not handles,(rc,result,hex(marker),faults,detail)
  print('PASS ARM protected-mode demand-page fault, full decoder and resume',flush=True);return
 if mode=='hdd':
  marker=struct.unpack('<H',u.mem_read(symbols['_ZL3ram']+0x500,2))[0]
  assert rc==2 and result[4]==3 and marker==0xd007 and written==1536,(rc,result,hex(marker),detail,written)
  assert files['HDD.IMG'][32*512:32*512+2]==bytes.fromhex('1827')
  print('PASS ARM HDD BIOS geometry, cross-track read/write and end bounds',flush=True);return
 if mode in ('read-stop','fs-stop'):
  assert rc==2 and result[4]==3 and stopping,(mode,rc,result,detail)
  print('PASS ARM cancellation',mode,flush=True);return
 if mode=='copy-stop':
  assert rc==2 and result[4]==3 and copy_stopped[0],(rc,result,detail)
  assert get(0x188024)==1 and get(0x188028)==2
  for section in range(frame>>20,((frame+span-1)>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
  print('PASS cancellation during framebuffer write revokes mapping',flush=True);return
 if mode=='badnonce':
  assert rc==1 and result[4]==1 and b'Framebuffer handoff error 7' in detail,(rc,result,detail,output)
  assert get(0x188024)==1 and get(0x188028)==7
  for section in range(frame>>20,((frame+span-1)>>20)+1):
   assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
  print('PASS ARM rejected invalid framebuffer nonce and revoked mapping',flush=True)
  return
 if mode in ('guest-bound','high-bank-bound'):
  expected=(b'Guest page bound',) if mode=='guest-bound' else (b'Guest page bound',b'Out of Memory')
  assert rc==1 and detail in expected,(rc,result,detail)
  print('PASS banked TLB rejects unsupported guest address',mode,detail.decode(),flush=True);return
 if mode=='invalid-options':
  assert rc==1 and b'Invalid PC options' in detail and frame_seq==0,(rc,result,detail)
  print('PASS rejected invalid PC settings',flush=True);return
 assert rc==0 and result[4]==0,(rc,result,detail,output)
 if mode=='vga-shell':
  assert b'DOSBox version' in output and b'C:\\>' in output and raw_sent and frame_seq>=2 and captured,(output,frame_seq)
  assert get(0x188180)==18
  print('PASS VGA shell, BIOS keyboard VER/EXIT, FPS overlay and clean return',flush=True);return
 if mode in ('cga','keyboard','ega-options'):
  expected=b'CGA/PIT PASS' if mode=='cga' else b'PC keyboard PASS' if mode=='keyboard' else b'EGA/PIT PASS'
  assert expected in output,(mode,output)
  if mode=='keyboard':assert get(0x188180)==2
  else:assert frame_seq>=10 and captured
  print('PASS ARM PC options/input',mode,options,'frames',frame_seq,flush=True);return

 if mode=='speed':assert get(0x1883c8)==2500, get(0x1883c8)
 if mode=='cpu':
  for message in [b'ZZDOS x86 arithmetic/memory PASS',b'ZZDOS files/handles/seek PASS',b'ZZDOS MZ relocation/memory PASS']:
   assert message in output,(message,output)
  assert not handles,handles
  for section in range(frame>>20,(frame+span-1>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
  print('PASS ARM x86 arithmetic/memory, DOS files and MZ relocation',hex(base),flush=True)
  return
 assert b'EGA/PIT PASS' in output and frame_seq>=10 and captured,output
 colors=[0x000000,0x0000ad,0x00aa00,0x00aaad,0xad0000,0xad00ad,0xad5500,0xadaaad,0x525552,0x5255ff,0x52ff52,0x52ffff,0xff5552,0xff55ff,0xffff52,0xffffff]
 expected=b''.join(b''.join(struct.pack('<I' if fmt==3 else '>I',colors[x*4//width+(y*4//height)*4]) for x in range(width))+ (bytes(pitch-width*4) if y<height-1 else b'') for y in range(height))
 assert captured==expected,'EGA colour/presentation mismatch'
 # Temporary framebuffer mappings must be revoked before completion.
 for section in range(frame>>20,(frame+span-1>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
 print('PASS ARM EGA/PIT/direct framebuffer',hex(base),'format',fmt,'frames',frame_seq,'pixels',width*height,'geometry',mode,flush=True)
 (OUT/f'model-{mode}.txt').write_bytes(output+b'\n'+detail+b'\n')
 print('PASS ARM shell',hex(base),mode,'requests',calls,'read',read,'written',written,'rc',rc,flush=True)
if __name__=='__main__':
 for base in [0x12000040,0x150020c0]:
  run(base,'fixture',base==0x150020c0);run(base,'cpu')
 run(0x12000040,'badnonce',True)
 run(0x12000040,'read-stop',True);run(0x12000040,'fs-stop',True)

if __name__=='__main__':
 run(0x12000040,'ega-options',options=(1,1,1000))
 run(0x12000040,'cga',options=(2,2,6000))
 run(0x12000040,'keyboard',options=(2,0,3000))
 run(0x12000040,'invalid-options',options=(64,0,3000))

if __name__=='__main__':run(0x12000040,'copy-stop')

if __name__=='__main__':run(0x150020c0,'boot',options=(8,0,3000))
if __name__=='__main__':run(0x12000040,'paging',options=(8,0,3000))

if __name__=='__main__':run(0x12000040,'guest-bound',options=(8,0,3000))

if __name__=='__main__':run(0x12000040,'high-bank-bound',options=(8,0,3000))

if __name__=='__main__':run(0x12000040,'hdd',options=(8,0,3000))

if __name__=='__main__':run(0x12000040,'vga-shell')

if __name__=='__main__':
 for base in [0x12000040,0x150020c0]:run(base,'resize')

if __name__=='__main__':
 run(0x12000040,'cd');run(0x150020c0,'midi')

if __name__=='__main__':run(0x12000040,'speed')
