#!/usr/bin/env python3
"""Linked ARM PC-device test with model file and framebuffer handoffs.

Instruction-model evidence only. AmigaDOS backend and physical lifecycle must
be verified separately. Uses an original EGA/PIT fixture, no game data.
"""
import argparse,importlib.util,struct,subprocess,re,json,hashlib
from pathlib import Path
from unicorn import Uc,UC_ARCH_ARM,UC_MODE_ARM,UC_HOOK_MEM_READ,UC_HOOK_MEM_WRITE,UC_HOOK_CODE
from unicorn.arm_const import *
ROOT=Path(__file__).resolve().parents[3];OUT=ROOT/'.context/amiga/dosbox-pc'
spec=importlib.util.spec_from_file_location('pack',ROOT/'amiga/dosbox/elf.py');pack=importlib.util.module_from_spec(spec);spec.loader.exec_module(pack)
RXW,RXR,TXW,TXR=0x184000,0x184040,0x184080,0x1840c0
RX,TX,CMD,REPLY,NAME,NAME2,DATA=0x184400,0x185000,0x186000,0x186040,0x186080,0x186090,0x187000
pattern=bytes(i%251 for i in range(100003))
commands=b'dir /b\rtype readme.txt\recho saved text > SAVED.TXT\rtype saved.txt\rfiletest\rexetest\rcopy LARGE.BIN LARGE2.BIN\rren LARGE2.BIN RENAMED.BIN\rdir /b\rexit\r'
def scan_row(data,mode,y):
 factor=(0 if y%2 else 8) if mode in (1,2) else (5 if y%2 else 8) if mode==3 else (8,6,4,6)[y%4] if mode==4 else 8
 return bytes(c*factor//8 for c in data)
def run(base,mode,profile=False,options=None,presentation=None,adapter=None,developer=None,scheduler_trace=None,inherited_fpscr=0x01000000,memory_requests=None,memory_stop=False,matrix_stop=False,disk_mib=32,paging_fixture=None,paging_faults=1,network=None,hardware=None,fixture=None,mouse_burst=False):
 build=json.loads((OUT/'build.json').read_text());allocation=build.get('allocation',0x2000000);expanded=build.get('expanded',False)
 image,rel,_=pack.unpack((OUT/'dosbox.elf').read_bytes(),expanded=expanded)
 for off in rel:struct.pack_into('<I',image,off,struct.unpack_from('<I',image,off)[0]+base)
 symbols={}
 for line in subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm','-n',str(OUT/'dosbox.elf')],text=True).splitlines():
  f=line.split()
  if len(f)==3:symbols[f[2]]=int(f[0],16)+base
 trace_build=json.loads((OUT/'build.json').read_text()).get('video_trace',False)
 present_build=json.loads((OUT/'build.json').read_text()).get('presentation_test',False)
 if presentation is not None:assert present_build
 jit='db_dynrec_commit' in symbols
 neon='db_neon_begin' in symbols
 jit_mib=json.loads((OUT/'build.json').read_text()).get('jit_mib',1) if jit else 1
 u=Uc(UC_ARCH_ARM,UC_MODE_ARM);u.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_A9)
 if neon and mode!='neon-denied':
  u.reg_write(UC_ARM_REG_C1_C0_2,0x00f00000);u.reg_write(UC_ARM_REG_FPEXC,0x40000000)
  for i in range(32):u.reg_write(globals()['UC_ARM_REG_D'+str(i)],0xa5a5000000000000+i)
  u.reg_write(UC_ARM_REG_FPSCR,inherited_fpscr)
  # Reserved/unsupported exception-enable bits may be read-as-zero.
  inherited_fpscr=u.reg_read(UC_ARM_REG_FPSCR)
 u.mem_map(base&~4095,(allocation+0x2000));u.mem_write(base,bytes(image));u.mem_write(symbols['_ZL6shared'],struct.pack('<I',base))
 if 'zz_matrix_quick' in symbols:u.mem_write(symbols['zz_matrix_quick'],struct.pack('<I',1)) # correctness only; shorten microbenchmark repetition
 scheduler_digest=hashlib.sha256();scheduler_count=[0]
 if scheduler_trace is not None:
  names={address:name for name,address in symbols.items()}
  # Hash actual queue/callback/cycle decisions, normalising relocated pointers.
  def scheduler_event(u,address,size,data):
   name=names[address];record=[name]
   if name=='_Z12PIC_RunQueuev':
    for field in ('CPU_Cycles','CPU_CycleLeft','CPU_CycleMax','PIC_Ticks'):
     record.append(struct.unpack('<I',u.mem_read(symbols[field],4))[0])
   else:
    record.extend([names.get(u.reg_read(UC_ARM_REG_R0),'unknown-handler'),u.reg_read(UC_ARM_REG_R1),u.reg_read(UC_ARM_REG_R2)])
    # RemoveEvents has only one argument; unused registers are not semantic.
    if name.startswith('_Z16PIC_RemoveEvents'):record=record[:2]
    elif name.startswith('_Z24PIC_RemoveSpecificEvents'):record=record[:3]
   scheduler_digest.update((repr(record)+'\n').encode());scheduler_count[0]+=1
  for name,address in symbols.items():
   if name=='_Z12PIC_RunQueuev' or name.startswith(('_Z12PIC_AddEvent','_Z16PIC_RemoveEvents','_Z24PIC_RemoveSpecificEvents')):
    u.hook_add(UC_HOOK_CODE,scheduler_event,begin=address,end=address)
  if json.loads((OUT/'build.json').read_text()).get('pic_vfp'):
   def ieee_policy(u,address,size,data):
    control=(3<<22)|(1<<24)|(1<<25)|(7<<16)|(3<<20)|0x9f00
    assert not u.reg_read(UC_ARM_REG_FPSCR)&control,'PIC inherited a non-IEEE floating-point policy'
   address=symbols['_Z12PIC_RunQueuev'];u.hook_add(UC_HOOK_CODE,ieee_policy,begin=address,end=address)
 math_calls=[0]
 if mode=='pit-math':
  def math_call(u,address,size,data):
   assert not u.reg_read(UC_ARM_REG_FPSCR)&((3<<22)|(1<<24)|(1<<25)|(7<<16)|(3<<20)|0x9f00),'Library math inherited non-IEEE control state'
   math_calls[0]+=1
  for address in {symbols[n] for n in ('fmod','fmodl') if n in symbols}:
   u.hook_add(UC_HOOK_CODE,math_call,begin=address,end=address)
 if jit and mode=='paging' and 'db_dynrec_paged_fallbacks' in symbols:
  paged=[False];dispatches=[0];paged_dispatches=[0]
  def paging_policy(u,address,size,data):paged[0]=bool(u.reg_read(UC_ARM_REG_R0))
  address=symbols['_Z13PAGING_Enableb'];u.hook_add(UC_HOOK_CODE,paging_policy,begin=address,end=address)
  def jit_dispatch(u,kind,address,size,value,data):
   if paged[0]:
    assert expanded and hardware and hardware.get('paged_jit'),'JIT dispatched while 386 paging was enabled without opt-in'
    paged_dispatches[0]+=1
   dispatches[0]+=1
  address=symbols['db_dynrec_dispatches'];u.hook_add(UC_HOOK_MEM_WRITE,jit_dispatch,begin=address,end=address+3)
 def put(off,n):u.mem_write(base+off,struct.pack('>I',n&0xffffffff))
 def get(off):return struct.unpack('>I',u.mem_read(base+off,4))[0]
 matrix_closed=[]
 if matrix_stop:
  def matrix_cancel(u,address,size,data):
   put(0x180080,1);u.mem_write(base+0x1d0008,struct.pack('<I',1))
  u.hook_add(UC_HOOK_CODE,matrix_cancel,begin=symbols['zz_matrix_scene_draw'],end=symbols['zz_matrix_scene_draw'])
  for close_name in ('tglClose','ZB_close'):
   def closed(u,address,size,data):matrix_closed.append(address)
   u.hook_add(UC_HOOK_CODE,closed,begin=symbols[close_name],end=symbols[close_name])
 trace_records=[]
 if trace_build:
  put(0x188fa0,0 if developer and not developer.get('trace',True) else 1)
  if developer:
   put(0x188fa4,developer['phase']);put(0x188fa8,2)
  def trace_commit(u,kind,address,size,value,data):
   # ad_put publishes an aligned BE word after committing the slot.
   if address+size!=base+0x188e04:return
   serial=len(trace_records)+1;o=0x188e10+((serial-1)&15)*24
   record=struct.unpack('>6I',u.mem_read(base+o,24))
   assert record[0]==serial,record
   trace_records.append(record)
  u.hook_add(UC_HOOK_MEM_WRITE,trace_commit,begin=base+0x188e00,end=base+0x188e03)
 if profile or mode=="vga-shell" or present_build or trace_build:
  # Synthetic clock for accounting checks only, never a speed measurement.
  u.mem_map(0xf8f00000,4096);timer=[0xfffffff0]
  u.mem_write(0xf8f00200,struct.pack('<Q',timer[0]))
  def timer_read(u,kind,address,size,value,data):
   if address==0xf8f00200:
    timer[0]+=37;u.mem_write(0xf8f00200,struct.pack('<Q',timer[0]))
  u.hook_add(UC_HOOK_MEM_READ,timer_read,begin=0xf8f00200,end=0xf8f00207)
  put(0x188080,int(profile))
 if presentation is not None:
  put(0x188048,presentation);put(0x18804c,60)
  def present_enter(u,address,size,data):
   timer[0]+=3333333 # Synthetic 10 ms of non-presentation work per frame.
   u.mem_write(0xf8f00200,struct.pack('<Q',timer[0]))
  addr=symbols['_Z16db_video_presentv']
  u.hook_add(UC_HOOK_CODE,present_enter,begin=addr,end=addr)
  if mode=='present-stop':
   def stop_skipped(u,address,size,data):
    # First frame increments the grant sequence. First skipped frame does
    # not; cancel from its explicit poll, before another bitmap is requested.
    generated=struct.unpack('<I',u.mem_read(symbols['db_frames'],4))[0]
    if generated>get(0x188000):
     put(0x180080,1);u.mem_write(base+0x1d0008,struct.pack('<I',1))
   addr=symbols['_Z13db_shell_pollv']
   u.hook_add(UC_HOOK_CODE,stop_skipped,begin=addr,end=addr)
 if hardware:
  put(0x188114,0x48574431);put(0x188118,hardware.get('cpu',0));put(0x18811c,hardware.get('services',0));put(0x188120,hardware.get('joystick',0));put(0x188124,hardware.get('sound',0));put(0x188128,hardware.get('paged_jit',0));put(0x1883cc,hardware.get('joy_state',0));put(0x198004,hardware.get('ems',1))
 audio_frames=audio_nonzero=network_audio_nonzero=0
 for off in [RXW,RXR,TXW,TXR]:put(off,0xfffffffc)
 u.mem_write(base+0x1d0000,bytes.fromhex('00009fe51eff2fe100000000'));u.mem_write(base+0x1d0100,bytes.fromhex('0100a0e31eff2fe1'))
 bad=[]
 def access(u,kind,address,size,value,data):
  if not base<=address<=base+allocation-size or (size>1 and address%size):bad.append((hex(address),size,hex(u.reg_read(UC_ARM_REG_PC)-base)));u.emu_stop()
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base&~4095,end=base-1)
 u.hook_add(UC_HOOK_MEM_READ|UC_HOOK_MEM_WRITE,access,begin=base+allocation,end=(base&~4095)+(allocation+0x2000)-1)
 scan_test=mode in ("scan-full","scan-skip","scan-transition","scan-stop","scan-soft","scan-gradient")
 scan_mask=0;scan_epoch=0;scan_checked=set();scan_buffers=set()
 width,height=640,480
 frame=0x00fff040;pitch=3840 if mode=="resize" else 2560;span=(height-1)*pitch+width*4;fmt=3 if base==0x12000040 else 4
 u.mem_map(frame&~4095,0x400000 if scan_test else 0x2b0000 if mode=="resize" else 0x140000)
 l1=(base+(0x140000 if expanded else 0x100000)+16383)&~16383
 u.mem_write(base+0x182000+15*4,struct.pack('<I',l1))
 put(0x188040,1000);put(0x188044,0)
 if options:
  put(0x188100,0x50434f31)
  for i,v in enumerate(options,1):put(0x188100+i*4,v)
 if mode=='keyboard':
  u.mem_write(base+0x1881c0,bytes([0x20,0xa0]));put(0x188140,2)

 copy_stopped=[False]
 if mode in ('copy-stop','scan-stop'):
  def stop_copy(u,kind,address,size,value,data):
   if not copy_stopped[0]:
    copy_stopped[0]=True;put(0x180080,1);u.mem_write(base+0x1d0008,struct.pack('<I',1))
  u.hook_add(UC_HOOK_MEM_WRITE,stop_copy,begin=frame,end=frame+span-1)
 frame_seq=0;captured=None
 for reg,val in [(UC_ARM_REG_SP,base+allocation-16),(UC_ARM_REG_LR,base+allocation-0x10000),(UC_ARM_REG_R0,1),(UC_ARM_REG_R1,base+0x1e0000),(UC_ARM_REG_R2,base+0x1d0000),(UC_ARM_REG_R3,base+0x1d0100)]:u.reg_write(reg,val)
 files={'EGATEST.COM':(OUT/'EGATEST.COM').read_bytes()};keys=b'egatest\rexit\r'
 if presentation is not None:
  ega_fixture=files['EGATEST.COM'];assert ega_fixture.count(bytes.fromhex('83fa05'))==1
  files['EGATEST.COM']=ega_fixture.replace(bytes.fromhex('83fa05'),bytes.fromhex('83fa0a'))
 if adapter:
  file=adapter[0]+'.COM';files={file:(OUT/file).read_bytes()};keys=file.encode()+b'\rexit\r'
 if mode=='pit-math':
  files={'PITMATH.COM':(OUT/'PITMATH.COM').read_bytes()};keys=b'pitmath\rexit\r'
 if fixture:
  file=fixture[0]+'.COM';files={file:(OUT/file).read_bytes()};keys=file.encode()+b'\rexit\r'
 if mode=='x87':
  files={'X87TEST.COM':(OUT/'X87TEST.COM').read_bytes()};keys=b'x87test\rexit\r'
 if mode=='vga-shell':
  keys=b'';put(0x1882c0,3);put(0x1882c8,7);put(0x188040,0)
 if mode in ('cga','keyboard'):
  file='CGATEST.COM' if mode=='cga' else 'KEYTEST.COM';files={file:(OUT/file).read_bytes()};keys=file.encode()+b'\rexit\r'
 if mode=='memory-smc':
  import sys
  sys.path.insert(0,str(ROOT/'scripts'))
  from make_dosbox_memory_fixture import make
  files={'MEMPATCH.COM':make()};keys=b'mempatch\rexit\r';memory_requests=[]
 if mode=='smc':
  files={'SMCTEST.COM':(OUT/'SMCTEST.COM').read_bytes()};keys=b'smctest\rexit\r'
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
  paging_data=(Path(paging_fixture) if paging_fixture else OUT/'PAGINGTEST.COM').read_bytes();files={'PAGING.IMG':paging_data+bytes(1440*1024-len(paging_data))};keys=b''
  u.mem_write(base+0x188240,b'PAGING.IMG\0');put(0x188040,100)
 if mode=='hdd':
  assert disk_mib in (32,64,128,256)
  boot=bytearray((OUT/'HDDTEST.COM').read_bytes());cylinders=disk_mib*4
  # Patch fixture constants only; geometry is supplied by actual linked BIOS.
  boot=boot.replace(bytes.fromhex('80fd7f'),bytes([0x80,0xfd,(cylinders-1)&255]))
  boot=boot.replace(bytes.fromhex('80f920'),bytes([0x80,0xf9,32|(((cylinders-1)>>8)<<6)]))
  boot=boot.replace(bytes.fromhex('b9207f'),bytes([0xb9,32|(((cylinders-1)>>8)<<6),(cylinders-1)&255]))
  # Test the next sector beyond the last track, including 1024-cylinder limit.
  boot=boot.replace(bytes.fromhex('b90180'),bytes([0xb9,33|(((cylinders-1)>>8)<<6),(cylinders-1)&255]))
  files={'HDD.IMG':bytes(boot)+bytes(disk_mib*1024*1024-512)};keys=b""
  u.mem_write(base+0x188280,b'HDD.IMG\0');put(0x188040,100)
 if mode=='cd':
  from make_cd_fixture import make
  iso_path=OUT/'TEST.ISO';make(iso_path);cd_image=iso_path.read_bytes();put(0x1883b0,len(cd_image)//2048)
  files={};keys=b'type d:\\hello.txt\rcopy d:\\large.bin c:\\CDLARGE.BIN\rexit\r'
 if mode=='midi':
  files={'MIDITEST.COM':(OUT/'MIDITEST.COM').read_bytes()};keys=b'miditest\rexit\r';put(0x1883c0,1);put(0x18a000,0xffffff00);put(0x18a040,0xffffff00)
 if mode in ('mouse','mouse-invalid','mouse-stop'):
  files={'MOUSETST.COM':(OUT/'MOUSETST.COM').read_bytes()};keys=b'mousetst\rexit\r'
  put(0x188400,0xfffffffe);put(0x188440,0xfffffffe)
 mouse_stage=0
 if network is not None:
  assert json.loads((OUT/'build.json').read_text()).get('network')
  assert memory_requests is None
  files=dict(network.files);keys=network.keys
  put(0x198000,getattr(network,"audio_mode",0));put(0x198004,getattr(network,"ems",0))
  put(0x18c000,0x4e455431);put(0x188040,0)
  put(0x18c004,getattr(network,"protocol",0));put(0x18c008,getattr(network,"server",0));put(0x18c00c,getattr(network,"port",213))
 midi_bytes=bytearray()
 raw_sent=False
 disk_open=False
 handles={};calls=read=written=0;output=bytearray();sent=0;stopping=False;seen_request=0
 def name(off):
  text=bytes(u.mem_read(base+off,13)).split(b'\0')[0].decode('ascii')
  assert re.fullmatch(r'[A-Z0-9_-]{1,8}(\.[A-Z0-9_-]{1,3})?',text),text
  return text
 def bad_code(u,address,size,data):
  if jit and ((base+allocation-0x100000-jit_mib*0x100000+4095)&~4095)<=address<((base+allocation-0xe0000)&~4095):return
  raise AssertionError(('execution outside text',hex(address-base),'lr',hex(u.reg_read(UC_ARM_REG_LR)-base),'sp',hex(u.reg_read(UC_ARM_REG_SP)-base)))
 u.hook_add(UC_HOOK_CODE,bad_code,begin=base+len(image),end=base+0x1cffff)
 u.hook_add(UC_HOOK_CODE,bad_code,begin=base+0x200000,end=base+allocation-0x10001)
 if neon:
  # Unicorn's A9 CPACR MRC reads zero even after C1_C0_2 is set; model only
  # that hardware-register read. Real availability/restoration is checked
  # separately on the A9. Keep all subsequent policy checks/instructions.
  def cpacr_read(u,address,size,data):
   u.reg_write(UC_ARM_REG_R1,u.reg_read(UC_ARM_REG_C1_C0_2))
   u.reg_write(UC_ARM_REG_PC,address+4)
  u.hook_add(UC_HOOK_CODE,cpacr_read,begin=symbols['db_neon_available'],end=symbols['db_neon_available'])
 pause_actions=list(developer['actions']) if developer else []
 pause_seen=[];pause_hold=None
 memory_at=0;memory_pending=False;memory_seen=[]
 external_target=[None];external_writes=[0]
 if mode=='memory-smc':
  def external_write(u,address,size,data):
   at=u.reg_read(UC_ARM_REG_R1)
   if external_target[0] is not None and at in (external_target[0],external_target[0]+1):external_writes[0]+=1
  functions=[a for n,a in symbols.items() if 'CodePageHandlerDynRec' in n and 'writebE' in n]
  assert functions
  for address in functions:u.hook_add(UC_HOOK_CODE,external_write,begin=address,end=address)

 pc=symbols['mesen_test']
 for tick in range(50000):
  if network is not None:network.service(u,base,put,get)
  if mode=='memory-smc' and not memory_requests and tick>100:
   data=bytes(u.mem_read(symbols['_ZL3ram']+0x500,8))
   if data[4:]==b'MEM1':
    segment,offset=struct.unpack('<HH',data[:4]);external_target[0]=segment*16+offset
    memory_requests.extend([((2,0,0,0,0,0,0,0),0,None),((5,external_target[0],0,2,0x2222,0,0,0x1111),0,None),((3,0,0,0,0,0,0,0),0,None)])
  if memory_requests is not None:
   if memory_pending and get(0x18c080)==memory_at+1:
    fields=struct.unpack('>8I',u.mem_read(base+0x18c0c0,32));payload=bytes(u.mem_read(base+0x18c0e0,256))
    request,error,check=memory_requests[memory_at]
    assert fields[0]==error,(request,fields)
    if check:check(fields,payload)
    memory_seen.append(fields);memory_at+=1;memory_pending=False
    if memory_stop and memory_at==len(memory_requests):stopping=True
   if not memory_pending and memory_at<len(memory_requests):
    request=memory_requests[memory_at][0]
    u.mem_write(base+0x18c000,struct.pack('>8I',*request));put(0x18c040,memory_at+1);memory_pending=True

  # Preserve Thumb state when resuming a quantum inside the soft-float library.
  u.emu_start(pc | (1 if u.reg_read(UC_ARM_REG_CPSR)&32 else 0),base+allocation-0x10000,count=100000);pc=u.reg_read(UC_ARM_REG_PC);assert not bad,bad
  if mode in ('mouse','mouse-invalid','mouse-stop'):
   stage=u.mem_read(symbols['_ZL3ram']+0x500,1)[0]
   if 0<stage<5 and stage!=mouse_stage:
    w=get(0x188400);count=3 if mouse_burst and stage==1 else 1
    for index in range(count):
     at=0x188480+((w+index)&127)*16
     for off,val in [(0,16000 if index==1 else 16384),(4,49151),(8,1 if stage in (1,4) else 6 if stage==2 else 0),(12,2 if mode=='mouse-invalid' else int(stage!=3))]:put(at+off,val)
    put(0x188400,w+count);mouse_stage=stage
    if mode=='mouse-stop':stopping=True
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
   if presentation is not None:
    timer[0]+=6666666 # Synthetic 20 ms presentation pressure, no FPS claim.
    u.mem_write(0xf8f00200,struct.pack('<Q',timer[0]))
   if mode=="speed":put(0x1883c4,4000 if vseq<4 else 2500)
   if mode=="resize":
    width,height=[(320,240),(960,720),(508,381),(640,480)][(vseq-1)%4]
    span=(height-1)*pitch+width*4
    put(0x18802c,width);put(0x188030,height)
   if scan_test:
    frame=0x00fff040+((vseq-1)%3)*0x140000
    scan_epoch=(vseq-1)//3+1 if mode=='scan-transition' else 1
    scan_mask=1 if mode=='scan-full' else 3 if mode=='scan-soft' else 4 if mode=='scan-gradient' else [2,3,4,0,2,0][(scan_epoch-1)%6] if mode=='scan-transition' else 2
    put(0x188034,scan_mask);put(0x188038,scan_epoch)
    sw=struct.unpack('<I',u.mem_read(symbols['db_width'],4))[0]
    sh=struct.unpack('<I',u.mem_read(symbols['db_height'],4))[0]
    source=bytes(u.mem_read(symbols['db_pixels'],sw*sh*4))
    rows=[]
    for sy in range(sh):
     line=source[sy*sw*4:(sy+1)*sw*4]
     pixels=[line[(x*sw//width)*4:(x*sw//width)*4+4] for x in range(width)]
     rows.append(b''.join(pixels if fmt==3 else [p[::-1] for p in pixels]))
    scan_expected=b''.join(scan_row(rows[y*sh//height],scan_mask,y)+(bytes(pitch-width*4) if y<height-1 else b'') for y in range(height))
    if frame not in scan_buffers:
     u.mem_write(frame,b'\xa5'*span+b'\0'*4);scan_buffers.add(frame)
   nonce=vseq^0x12345678
   if not scan_test:u.mem_write(frame,b'\x00'*(span+4))
   u.mem_write(frame,struct.pack('>I',nonce ^ (1 if mode=='badnonce' else 0)));u.mem_write(frame+span-4,struct.pack('>I',nonce^0x31415926))
   for off,val in [(20,frame),(24,pitch),(28,fmt),(32,nonce),(16,vseq)]:put(0x188000+off,val)
   frame_seq=vseq
  if frame_seq and get(0x188024)==frame_seq:
   assert get(0x188028)==(7 if mode=='badnonce' else 2 if mode in ('copy-stop','scan-stop') else 0)
   captured=bytes(u.mem_read(frame,span))
   assert bytes(u.mem_read(frame+span,4))==b'\0'*4
   if scan_test and not get(0x188028) and frame_seq not in scan_checked:
    scan_checked.add(frame_seq)
    assert captured==scan_expected,('per-frame scanline pixels',mode,frame_seq,scan_mask)
    if scan_mask in (1,2):
     for y in range(1,height,2):assert captured[y*pitch:y*pitch+width*4]==bytes(width*4),(mode,frame_seq,y)

  if developer:
   token=get(0x188fb0)
   if token and token!=get(0x188fac) and not stopping:
    target,phase=get(0x188fb4),get(0x188fb8)
    assert phase==developer['phase'] and target==get(0x188fa8)
    for section in range(frame>>20,((frame+span-1)>>20)+1):
     assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0,'paused with active RTG map'
    snapshot=(token,target,phase,get(0x188000),get(0x188024),len(trace_records))
    if pause_hold is None:pause_hold=snapshot
    else:
     assert pause_hold==snapshot,'worker advanced while paused'
     assert token==len(pause_seen)+1
     pause_seen.append((target,phase));pause_hold=None
     action=pause_actions.pop(0)
     if action=='stop':stopping=True
     else:
      if action=='next':put(0x188fa8,target+1)
      else:assert action=='continue';put(0x188fa4,0)
      put(0x188fac,token)
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
     assert path=='HDD.IMG' and not disk_open;disk_open=True;result=len(files['HDD.IMG'])//512
    elif op in (17,18):
     assert disk_open and 1<=n<=8 and pos<len(files['HDD.IMG'])//512 and pos+n<=len(files['HDD.IMG'])//512
     at=pos*512;count=n*512
     if op==17:u.mem_write(base+DATA,files['HDD.IMG'][at:at+count]);read+=count
     else:
      old=files['HDD.IMG'];files['HDD.IMG']=old[:at]+bytes(u.mem_read(base+DATA,count))+old[at+count:];written+=count
     result=n
    elif op in (1,2):
     # This fixture filesystem has only a root directory. Reject a missing
     # parent instead of silently creating its leaf in the root.
     directory=bytes(u.mem_read(base+CMD+256,256)).split(b"\0")[0]
     if directory:error=3
     else:
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
  r,w=get(0x198080),get(0x198040)
  assert w-r<=4
  while r!=w:
   pcm=u.mem_read(base+0x1a0000+(r&3)*4096,4096);audio_frames+=1024;audio_nonzero+=sum(c!=0 for c in pcm);r=(r+1)&0xffffffff
   if network is not None and network.rx:network_audio_nonzero+=sum(c!=0 for c in pcm)
  put(0x198080,r)
  if pc==base+allocation-0x10000:break
 else:raise AssertionError(('worker timeout',output[-200:],hex(pc-base)))
 if memory_requests is not None:
  assert memory_at==len(memory_requests),(memory_at,len(memory_requests))
  print('PASS guest memory command/ACK sequence',memory_at,'requests',flush=True)
 result=struct.unpack('<11I',u.mem_read(base+0x1e0000,44));rc=u.reg_read(UC_ARM_REG_R0)
 if scheduler_trace is not None:scheduler_trace.update(digest=scheduler_digest.hexdigest(),calls=scheduler_count[0])
 if mode=='neon-denied':
  assert rc==1 and get(0x188d68)==0
  detail=bytes(u.mem_read(base+0x180900+128,160)).split(b'\0')[0]
  assert b'NEON unavailable' in detail,detail
  print('PASS denied firmware SIMD access fails without executing NEON',flush=True);return
 if neon and mode!='invalid-options':
  assert get(0x188d60)==0x4e454f31 and get(0x188d64)==42 and get(0x188d68)==1,(rc,[hex(get(0x188d60+i*4)) for i in range(7)],bytes(u.mem_read(base+0x180980,160)))
  for i in range(32):assert u.reg_read(globals()['UC_ARM_REG_D'+str(i)])==0xa5a5000000000000+i
  assert u.reg_read(UC_ARM_REG_FPSCR)==inherited_fpscr
  print('PASS NEON execution witness, all 32 SIMD registers and FPSCR restored',flush=True)
 if matrix_stop:
  assert rc==2 and result[4]==3 and not handles,(rc,result)
  assert symbols['tglClose'] in matrix_closed and symbols['ZB_close'] in matrix_closed
  assert get(0x188e00)==0,'Interrupted benchmark must not publish completion'
  print('PASS cancellation inside matrix scene: GL and colour/depth buffers closed, SIMD/FPSCR restored',flush=True);return
 if developer:
  assert not pause_actions and len(pause_seen)==len(developer['actions'])
  assert get(0x188fb0)==0,'paused status not cleared on exit'
  if not developer.get('trace',True):assert not trace_records
  print('PASS cooperative breakpoints: unmapped pause, resume/step/stop',pause_seen,developer['actions'],flush=True)
 if trace_build and trace_records:
  groups={}
  for serial,phase,frame,detail,hi,lo in trace_records:
   assert 1<=phase<=10 and (frame>=1 or phase==10)
   if phase!=10:groups.setdefault(frame,[]).append((phase,detail))
  for frame,records in groups.items():
   phases=[p for p,d in records]
   expected=[1,2,3,4,8,9] if mode=='badnonce' else list(range(1,10))
   assert phases==expected,(mode,frame,records)
   errors=[d for p,d in records if p in (7,8,9)]
   expected_error=7 if mode=='badnonce' else 2 if mode in ('copy-stop','scan-stop') else 0
   assert errors and all(d==expected_error for d in errors),(mode,errors)
  # Ring has overwritten old entries, but each retained slot is committed.
  last=len(trace_records)
  for serial in range(max(1,last-15),last+1):
   o=0x188e10+((serial-1)&15)*24
   assert tuple(struct.unpack('>6I',u.mem_read(base+o,24)))==trace_records[serial-1]
  assert get(0x188e00)==last
  if 'zz_gl_render' in symbols:assert trace_records[-1][1]==10
  print('PASS diagnostic phase ordering, error cleanup and ring wrap',len(trace_records),'records',flush=True)
 if json.loads((OUT/'build.json').read_text()).get('matrix_bench') and not json.loads((OUT/'build.json').read_text()).get('tinygl_bench') and mode not in ('neon-denied','invalid-options'):
  assert get(0x188e00)==0x4d415431 and get(0x188e04)==0 and get(0x188e08)==6 and get(0x188e0c)==1
  for i,n in enumerate((4,16,256,1024,384,3072)):
   o=0x188e10+i*40
   assert get(o)==i and get(o+4)==n and get(o+8)==2
   assert get(o+12)!=0
   for m in range(3):assert get(o+16+m*8) or get(o+20+m*8)
  print('PASS matrix report, exact dyadic outputs, scene hashes, guards and cleanup; timings are model-only',flush=True)
 if json.loads((OUT/'build.json').read_text()).get('tinygl_bench') and not json.loads((OUT/'build.json').read_text()).get('tinygl_raster') and mode not in ('neon-denied','invalid-options'):
  assert (get(0x188e00),get(0x188e04),get(0x188e08),get(0x188e0c))==(0x54474c31,0,3,1),[hex(get(0x188e00+i*4)) for i in range(4)]
  for i,n in enumerate((1024,128,32)):
   o=0x188e10+i*128
   assert (get(o),get(o+4),get(o+8))==(i,n,2) and get(o+12)
   for m in range(2):
    values=[(get(o+32+m*48+j*8)<<32)|get(o+36+m*48+j*8) for j in range(6)]
    assert values[0]==sum(values[1:]) and all(values),values
  print('PASS TinyGL internal A/B image/depth hashes, stage accounting, cleanup; model timings only',flush=True)
 if json.loads((OUT/'build.json').read_text()).get('tinygl_raster') and mode not in ('neon-denied','invalid-options'):
  revision=json.loads((OUT/'build.json').read_text())['tinygl']['raster_experiment']['revision']
  revised=revision>=3
  assert (get(0x188e00),get(0x188e04),get(0x188e08),get(0x188e0c))==((0x52535433,0,8,1) if revision>=4 else (0x52535432,0,8,1) if revised else (0x52535431,0,7,1)),[hex(get(0x188e00+i*4)) for i in range(4)]
  for i,n in enumerate((1024,128,32,32,32,32,32,32) if revision>=4 else (1024,128,32,32,0,0,0)+((32,) if revised else ())):
   o=0x188e10+i*48
   assert (get(o),get(o+4),get(o+8))==(i,n,1 if revision>=4 else 2) and get(o+12)
   for m in range(4):assert get(o+16+m*8) or get(o+20+m*8)
  print('PASS TinyGL raster/clear kernel self-checks, four-method colour/depth hashes, cleanup; model timing only',flush=True)
 if json.loads((OUT/'build.json').read_text()).get('pixel_bench') and mode!='neon-denied':
  assert get(0x188e00)==0x50495831 and get(0x188e04)==0 and get(0x188e08)==8
  for i in range(8):
   o=0x188e10+i*32
   assert get(o) in (320,640) and get(o+4) in (get(o),get(o)*2)
   assert get(o+8) in (3,4) and get(o+12)==2048
   assert get(o+16) or get(o+20)
   assert get(o+24) or get(o+28)
  print('PASS private pixel benchmark report, exact pixels and guards; timings are model-only',flush=True)
 if 'zz_gl_render' in symbols and mode not in ('neon-denied','invalid-options'):
  assert get(0x188de0)==0x474c3031 and get(0x188df0)==1
  # Rendering now follows a validated grant; an invalid nonce never renders.
  assert get(0x188de4)==frame_seq-(1 if mode=='badnonce' else 0)
  assert get(0x188df4)+get(0x188df8)==get(0x188de4)
  if mode=='resize':assert get(0x188df4)>0 and get(0x188df8)>0
  if mode in ('fixture','adapter'):assert get(0x188df4)==frame_seq and get(0x188df8)==0
  assert get(0x188d54)<10*1024*1024
  print('PASS TinyGL execution report and context cleanup',get(0x188de4),'frames',flush=True)
 if jit and mode!='invalid-options':
  assert get(0x188d40)==0x4a495431 and get(0x188d48)==1 and get(0x188d44)>3
  # Cancellation at the initial prompt can precede the first guest dispatch.
  if mode not in ('read-stop','fs-stop') and not memory_stop:assert get(0x188d4c)>0
  print('PASS JIT cache witness/report, commits',get(0x188d44),'dispatches',get(0x188d4c),'heap',get(0x188d54),flush=True)
 if build.get('jit_reuse') and fixture and fixture[0]=='JITREUSE':
  assert get(0x188d5c)>0,'Register forwarding did not reach direct CPU-register loads'
  print('PASS register forwarding exercised:',get(0x188d5c),flush=True)
 if build.get('jit_hot') and mode=='fixture':
  assert get(0x188e60)==0x484f5431 and 0<get(0x188e64)<=16384 and get(0x188e68)==0
  counts=[get(0x188e74+i*24) for i in range(16)]
  assert counts==sorted(counts,reverse=True) and counts[0]>0 and sum(counts)<=get(0x188e6c)
  for i,count in enumerate(counts):
   if count:assert get(0x188e70+i*24)<8*1048576
  print('PASS diagnostic JIT block ranking, bounds and count accounting',flush=True)
 detail=bytes(u.mem_read(base+0x180900+128,160)).split(b'\0')[0]
 if profile:
  assert get(0x188084)==0x50524631 and get(0x188094)==0
  total=(get(0x188088)<<32)|get(0x18808c)
  buckets=[(get(0x188098+i*16)<<32)|get(0x18809c+i*16) for i in range(6)]
  assert total>0 and sum(buckets)==total,(total,buckets)
  print('PASS profile publication and exclusive totals',mode,'unwound',get(0x188090),flush=True)
 if mode=='mouse':
  assert rc==0 and b'DOS mouse + PS2 IRQ12 PASS' in output and not handles,(rc,result,detail,output[-300:])
  assert get(0x188400)==(4 if mouse_burst else 2) and get(0x188440)==get(0x188400)
  print('PASS ARM mouse ring wrap, DOS position/buttons, release and BIOS PS2 IRQ12',flush=True);return
 if mode=='mouse-invalid':
  assert rc==1 and b'Mouse event bound' in detail,(rc,result,detail)
  print('PASS ARM malformed mouse event rejected',flush=True);return
 if developer and developer['actions'][-1]=='stop':
  assert rc==2 and result[4]==3 and stopping and not handles,(rc,result,detail)
  print('PASS checkpoint cancellation and resource cleanup',flush=True);return
 if mode=='mouse-stop':
  assert rc==2 and result[4]==3 and stopping,(rc,result,detail)
  print('PASS ARM cancellation with pending mouse input',flush=True);return
 if mode=='cd':
  assert rc==0 and b'ZZCD data read PASS' in output and files['CDLARGE.BIN']==bytes(i%251 for i in range(5003)),(rc,result,detail,output[-300:])
  print('PASS ARM CD ISO9660 file read and cross-sector copy',flush=True);return
 if mode=='midi':
  assert rc==0 and b'MPU401 intelligent/UART PASS' in output,(rc,result,detail,output[-300:])
  assert midi_bytes==bytes([0xf8])*5000+bytes([0x91,60,64,62,65,0xf8,0x81,60,0,0xf0,0x7d,1,2,3,0xf7])
  print('PASS ARM MPU-401 and bounded MIDI stream/wrap',flush=True);return
 if fixture:
  assert rc==0 and fixture[1].encode() in output and not handles,(rc,result,detail,output)
  if hardware and hardware.get('sound'):assert audio_frames>=16000 and audio_nonzero>1000,(audio_frames,audio_nonzero)
  print('PASS ARM capability',fixture[0],hardware,'PCM',audio_frames,audio_nonzero,flush=True);return
 if mode=='x87':
  assert rc==0 and b'x87 arithmetic rounding state PASS' in output and not handles,(rc,result,detail,output)
  print('PASS ARM x87 arithmetic/rounding/state and SIMD restoration',flush=True);return
 if mode=='pit-math':
  assert rc==0 and b'PIT math PASS' in output and math_calls[0]>0,(rc,result,output,math_calls[0])
  print('PASS PIT library math with IEEE policy and full SIMD state restoration; calls',math_calls[0],flush=True);return
 if mode=='boot':
  marker=struct.unpack('<H',u.mem_read(symbols['_ZL3ram']+0x500,2))[0]
  assert rc==2 and result[4]==3 and marker==0xb007 and not handles,(rc,result,hex(marker),detail)
  print('PASS ARM BIOS boot/read/bounds, A20 aliasing, ROM protection and timed stop',flush=True);return
 if mode=='paging':
  marker,faults=struct.unpack('<II',u.mem_read(symbols['_ZL3ram']+0x500,8))
  assert rc==2 and result[4]==3 and marker==0xfa17 and faults==paging_faults and not handles,(rc,result,hex(marker),faults,detail)
  if jit and 'db_dynrec_paged_fallbacks' in symbols:
   assert get(0x188d58)>0 and dispatches[0]>0,'Missing interpreter fallback or initial JIT execution'
   if hardware and hardware.get('paged_jit'):assert paged_dispatches[0]>0,('Missing paged register JIT execution',get(0x188128),struct.unpack('<I',u.mem_read(symbols['db_paged_jit'],4))[0],get(0x188d58),dispatches[0])
   print('PASS paging policy; paged register dispatches',paged_dispatches[0],'fallback calls',get(0x188d58),flush=True)
  assert get(0x1897d0)==0,'Paged shutdown must not publish a BIOS text dump'
  print('PASS ARM paging fixture',str(paging_fixture or 'demand-page'), 'faults',faults,'and shutdown with unmapped text aperture',flush=True);return
 if mode=='hdd':
  marker=struct.unpack('<H',u.mem_read(symbols['_ZL3ram']+0x500,2))[0]
  assert rc==2 and result[4]==3 and marker==0xd007 and written==1536,(rc,result,hex(marker),detail,written)
  assert files['HDD.IMG'][32*512:32*512+2]==bytes.fromhex('1827')
  print('PASS ARM HDD BIOS geometry, cross-track read/write and end bounds',disk_mib,'MiB',flush=True);return
 if mode in ('read-stop','fs-stop'):
  assert rc==2 and result[4]==3 and stopping,(mode,rc,result,detail)
  print('PASS ARM cancellation',mode,flush=True);return
 if memory_stop:
  assert rc==2 and result[4]==3 and not handles,(rc,result,detail)
  print('PASS memory pause cancellation and owned-resource cleanup',flush=True);return
 if mode=='present-stop':
  assert rc==2 and result[4]==3 and frame_seq==1,(rc,result,detail,frame_seq)
  assert get(0x188da0)==0x50525331 and get(0x188dac)==1
  for section in range(frame>>20,((frame+span-1)>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
  print('PASS cancellation on skipped frame without another grant or mapping',flush=True);return
 if mode in ('copy-stop','scan-stop'):
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
  assert rc==1 and any(detail==e or detail.startswith(e+b' ') for e in expected),(rc,result,detail)
  print('PASS banked TLB rejects unsupported guest address',mode,detail.decode(),flush=True);return
 if mode=='invalid-options':
  assert rc==1 and b'Invalid PC options' in detail and frame_seq==0,(rc,result,detail)
  print('PASS rejected invalid PC settings',flush=True);return
 if network is not None and getattr(network,'cancel',False):
  assert rc==2 and result[4]==3 and not handles and get(0x18c140)==0,(rc,result,detail,output)
  print('PASS IPX connection cancellation and cleanup',flush=True);return
 assert rc==0 and result[4]==0,(rc,result,detail,output)
 if network is not None:
  assert not handles
  (OUT/'model-network.txt').write_bytes(output+b'\n'+detail+b'\n')
  network.check(bytes(output),files)
  if getattr(network,"audio_mode",0):
   assert audio_frames>=2048 and network_audio_nonzero>1000,(audio_frames,audio_nonzero,network_audio_nonzero)
   print("PASS concurrent network/audio PCM",audio_frames,"nonzero bytes",audio_nonzero,flush=True)
  print('PASS ARM guest '+('IPX/UDP' if getattr(network,'protocol',0) else 'NE2000')+', packet queues and clean shell exit',flush=True)
  return
 if adapter:
  assert b'ADAPTER PASS' in output and b'ADAPTER FAIL' not in output and frame_seq>=10 and captured,(rc,detail,output)
  actual=[struct.unpack('<I',u.mem_read(symbols[n],4))[0] for n in ('db_width','db_height','db_bpp')]
  assert actual==list(adapter[1:4]),(actual,adapter)
  pixel=struct.pack('<I' if fmt==3 else '>I',adapter[4]);assert captured==pixel*(width*height),(mode,adapter)
  for section in range(frame>>20,(frame+span-1>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
  assert not handles
  print('PASS ARM adapter bank/LFB, pixels, mapping revoke',adapter[0],actual,'frames',frame_seq,flush=True)
  return
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
 if mode=='memory-smc':
  assert b'MEMORY PATCH PASS' in output and external_writes[0]==2,(output,external_writes)
  print('PASS external paused edit invalidates translated x86 code and executes replacement',flush=True);return
 if mode=='smc':
  assert b'JIT self-modifying guest code PASS' in output and b'FAIL' not in output,(rc,result,detail,output)
  print('PASS ARM JIT guest self-modification / invalidation',flush=True);return
 if mode=='cpu':
  for message in [b'ZZDOS x86 arithmetic/memory PASS',b'ZZDOS files/handles/seek PASS',b'ZZDOS MZ relocation/memory PASS']:
   assert message in output,(message,output)
  assert not handles,handles
  for section in range(frame>>20,(frame+span-1>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
  print('PASS ARM x86 arithmetic/memory, DOS files and MZ relocation',hex(base),flush=True)
  return
 assert b'EGA/PIT PASS' in output and frame_seq>=(4 if presentation else 10) and captured,output
 if presentation is not None:
  assert get(0x188da0)==0x50525331
  generated,shown,skipped,divisor,changes=[get(0x188da4+i*4) for i in range(5)]
  assert generated==shown+skipped and shown==frame_seq and generated==result[9]
  assert get(0x188db8)==presentation and get(0x188dbc)==60
  assert (skipped==0 and divisor==1) if presentation==0 else (skipped>0 and divisor>1)
  if presentation==1:assert changes>0
  print('PASS presentation counters',presentation,'source',generated,'shown',shown,'skipped',skipped,'divisor',divisor,'changes',changes,flush=True)
 colors=[0x000000,0x0000ad,0x00aa00,0x00aaad,0xad0000,0xad00ad,0xad5500,0xadaaad,0x525552,0x5255ff,0x52ff52,0x52ffff,0xff5552,0xff55ff,0xffff52,0xffffff]
 expected=b''.join(b''.join(struct.pack('<I' if fmt==3 else '>I',colors[x*4//width+(y*4//height)*4]) for x in range(width))+ (bytes(pitch-width*4) if y<height-1 else b'') for y in range(height))
 if scan_test:
  expected=bytearray(expected)
  for y in range(height):expected[y*pitch:y*pitch+width*4]=scan_row(expected[y*pitch:y*pitch+width*4],scan_mask,y)
  assert get(0x188d80)==0x53434e31
  assert get(0x188d88)>0
  if mode not in ('scan-soft','scan-gradient'):assert get(0x188d8c)>0
  else:assert get(0x188d8c)==get(0x188d90)==get(0x188d94)==0
  if mode=='scan-skip':assert get(0x188d90)>0
  assert len(scan_buffers)==3 and len(scan_checked)>=10
  print('PASS scanline masks, three preserved slots, nonce repair and row guards',mode,'frames checked',len(scan_checked),flush=True)
 assert captured==expected,'EGA colour/presentation mismatch'
 # Temporary framebuffer mappings must be revoked before completion.
 for section in range(frame>>20,(frame+span-1>>20)+1):assert struct.unpack('<I',u.mem_read(l1+section*4,4))[0]==0
 print('PASS ARM EGA/PIT/direct framebuffer',hex(base),'format',fmt,'frames',frame_seq,'pixels',width*height,'geometry',mode,flush=True)
 (OUT/f'model-{mode}.txt').write_bytes(output+b'\n'+detail+b'\n')
 print('PASS ARM shell',hex(base),mode,'requests',calls,'read',read,'written',written,'rc',rc,flush=True)
 return {'source':result[9],'guest_ms':result[10],'output':bytes(output),'shown':frame_seq}
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

if __name__=="__main__":
 for mode in ("mouse","mouse-invalid","mouse-stop"):run(0x12000040,mode)
