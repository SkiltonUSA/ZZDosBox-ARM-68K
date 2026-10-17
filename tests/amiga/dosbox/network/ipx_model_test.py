#!/usr/bin/env python3
"""Linked ARM/x86 IPX driver + real native UDP, not a hardware speed test."""
import argparse,ctypes,ipaddress,sys,threading,subprocess,struct
from pathlib import Path
from ipx_relay import Relay,UpstreamPeer
ROOT=Path(__file__).resolve().parents[4]
sys.path.insert(0,str(ROOT/'tests/amiga/dosbox'))
import check_pc_arm as model
model.OUT=ROOT/'.context/amiga/dosbox-pc-cache1024-network'
p=argparse.ArgumentParser();p.add_argument('--bind',required=True);p.add_argument('--upstream',action='store_true');p.add_argument('--cancel',action='store_true');p.add_argument('--build',type=Path);a=p.parse_args()
if a.build:model.OUT=a.build.resolve()
work=ROOT/'.context/amiga/dosbox-ipx';work.mkdir(parents=True,exist_ok=True)
subprocess.run(['clang','--target=i386-none-elf','-c',str(ROOT/'tests/amiga/dosbox/network/ipx_guest.S'),'-o',str(work/'ipx_guest.o')],check=True)
subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(work/'ipx_guest.o'),'-o',str(work/'IPXTEST.COM')],check=True)
relay=UpstreamPeer(a.bind,ROOT) if a.upstream else Relay(a.bind);thread=threading.Thread(target=relay.run,daemon=True);thread.start()
class Config(ctypes.Structure):
 _fields_=[(n,ctypes.c_uint) for n in ('enabled','manual','mtu','retries','timeout')]+[(n,ctypes.c_char*16) for n in ('ip','mask','gateway','dns')]+[('hostname',ctypes.c_char*32),('protocol',ctypes.c_uint),('ipx_port',ctypes.c_uint),('ipx_server',ctypes.c_char*16)]
class Network:
 protocol=1
 cancel=a.cancel
 def __init__(self):
  self.server=int(ipaddress.IPv4Address(a.bind));self.port=relay.address[1];self.tx=self.rx=0
  self.lib=ctypes.CDLL(str(ROOT/'.context/amiga/dosbox-network/native/libzznetwork-model.dylib'))
  self.lib.zz_net_configure.argtypes=[ctypes.POINTER(Config)];self.lib.zz_net_open.argtypes=[ctypes.c_char_p]
  self.lib.zz_net_input.argtypes=[ctypes.c_void_p,ctypes.c_uint];self.lib.zz_net_receive.argtypes=[ctypes.c_void_p,ctypes.c_uint]
  c=Config(1,0,1500,3,10,b'10.0.2.15',b'255.255.255.0',b'10.0.2.2',b'',b'zzdosbox',1,self.port,a.bind.encode())
  assert self.lib.zz_net_configure(ctypes.byref(c)) and self.lib.zz_net_open(None)
  self.buf=ctypes.create_string_buffer(1518)
  self.files={'IPXTEST.COM':(work/'IPXTEST.COM').read_bytes()}
  self.keys=b'ipxnet connect\ripxnet status\ripxtest\ripxnet ping\ripxnet disconnect\ripxnet status\rexit\r'
 def service(self,u,base,put,get):
  w=get(0x18c040);r=get(0x18c080);n=(w-r)&0xffffffff;assert n<=4
  for i in range(n):
   off=0x18c200+((r+i)&3)*1600;length=get(off);assert 30<=length<=1424
   data=bytes(u.mem_read(base+off+4,length));assert self.lib.zz_net_input(data,length)
   put(0x18c080,(r+i+1)&0xffffffff);self.tx+=1
  if self.cancel and self.tx:
   put(0x180080,1);u.mem_write(base+0x1d0008,struct.pack("<I",1))
  assert self.lib.zz_net_poll()
  w=get(0x18c0c0);r=get(0x18c100);n=(w-r)&0xffffffff;assert n<=4
  for i in range(4-n):
   size=self.lib.zz_net_receive(self.buf,1518)
   if not size:break
   off=0x18db00+((w+i)&3)*1600;put(off,size);u.mem_write(base+off+4,self.buf.raw[:size]);put(0x18c0c0,(w+i+1)&0xffffffff);self.rx+=1
 def check(self,output,files):
  (work/'model-console.txt').write_bytes(output)
  for text in [b'IPX connected to configured server',b'PASS IPX socket, broadcast, receive and ESR',b'IPX peer replied',b'IPX client: disconnected']:
   assert text in output,output
  assert relay.registrations==1 and relay.broadcasts==1,(relay.registrations,relay.broadcasts)
  print('PASS linked ARM/x86 IPX discovery, registration, socket, broadcast, receive ESR, peer ping and disconnect:',self.tx,self.rx,flush=True)
net=Network()
try:model.run(0x12000040,'network',network=net)
finally:net.lib.zz_net_close();relay.stop=True;thread.join();relay.close()
