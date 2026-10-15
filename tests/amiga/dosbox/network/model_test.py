#!/usr/bin/env python3
"""Run a real DOS packet driver/mTCP DHCP through the linked ARM NE2000.

Instruction model + native NAT, not proof of Amiga/Roadshow operation.
Inputs are private upstream downloads; no third-party binary is committed.
"""
import argparse,ctypes,http.server,ipaddress,re,sys,threading,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[4]
sys.path.insert(0,str(ROOT/'tests/amiga/dosbox'))
import check_pc_arm as model
model.OUT=ROOT/'.context/amiga/dosbox-pc-cache1024-network'
work=ROOT/'.context/amiga/dosbox-network'
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--http-bind',help='Optional local IPv4 address for a bounded in-process HTTP test')
parser.add_argument('--manual',action='store_true',help='Exercise manual IPv4 without running DHCP')
parser.add_argument('--custom-subnet',action='store_true',help='Exercise configured NAT on 192.168.77.0/24')
args=parser.parse_args()
payload=bytes(i%251 for i in range(4099))
server=None
if args.http_bind:
 args.http_bind=str(ipaddress.IPv4Address(args.http_bind))
 class Handler(http.server.BaseHTTPRequestHandler):
  def do_GET(self):
   if self.client_address[0]!=args.http_bind or self.path!='/fixture':self.send_error(403);return
   self.send_response(200);self.send_header('Content-Length',str(len(payload)));self.end_headers();self.wfile.write(payload)
  def log_message(self,*args):pass
 server=http.server.HTTPServer((args.http_bind,0),Handler)
 threading.Thread(target=server.serve_forever,daemon=True).start()
class Network:
 def __init__(self):
  self.lib=ctypes.CDLL(str(work/'native/libzznetwork-model.dylib'))
  self.lib.zz_net_open.argtypes=[ctypes.c_char_p];self.lib.zz_net_open.restype=ctypes.c_int
  self.lib.zz_net_input.argtypes=[ctypes.c_void_p,ctypes.c_uint];self.lib.zz_net_input.restype=ctypes.c_int
  self.lib.zz_net_receive.argtypes=[ctypes.c_void_p,ctypes.c_uint];self.lib.zz_net_receive.restype=ctypes.c_uint
  self.buf=ctypes.create_string_buffer(1518);self.tx=self.rx=0
  subnet='192.168.77' if args.custom_subnet else '10.0.2'
  self.ip=subnet+('.42' if args.manual else '.15')
  class Config(ctypes.Structure):
   _fields_=[(n,ctypes.c_uint) for n in ('enabled','manual','mtu','retries','timeout')]+[(n,ctypes.c_char*16) for n in ('ip','mask','gateway','dns')]+[('hostname',ctypes.c_char*32),('protocol',ctypes.c_uint),('ipx_port',ctypes.c_uint),('ipx_server',ctypes.c_char*16)]
  c=Config(1,int(args.manual),1500,3,10,self.ip.encode(),b'255.255.255.0',(subnet+'.2').encode(),b'',b'zzdosbox',0,213,b'')
  self.lib.zz_net_configure.argtypes=[ctypes.POINTER(Config)]
  assert self.lib.zz_net_configure(ctypes.byref(c))
  s=(ROOT/'.context/research/dosbox-x/src/builtin/ne2000bin.cpp').read_text()
  s=s.split('bin_ne2000_com[] = {',1)[1].split('};',1)[0]
  driver=bytes(int(x,16) for x in re.findall(r'0x([a-fA-F0-9]{2})',s))
  with zipfile.ZipFile(work/'mtcp.zip') as z:dhcp=z.read('dhcp.exe');htget=z.read('htget.exe')
  self.files={'NE2000.COM':driver,'DHCP.EXE':dhcp,'MTCP.CFG':b'packetint 0x60\r\nhostname zzdosbox\r\n'}
  if args.manual:self.files['MTCP.CFG']+=('IPADDR '+self.ip+'\r\nNETMASK 255.255.255.0\r\nGATEWAY '+subnet+'.2\r\nNAMESERVER 0.0.0.0\r\n').encode()
  self.keys=b'ne2000 0x60 3 0x300\rset MTCPCFG=C:\\MTCP.CFG\r'+(b'' if args.manual else b'dhcp\r')+b'type MTCP.CFG\r'
  if server:
   self.files['HTGET.EXE']=htget
   self.keys+=('htget -o NET.BIN http://'+args.http_bind+':'+str(server.server_port)+'/fixture\r').encode()
  self.keys+=b'exit\r'
  assert self.lib.zz_net_open(None)
 def service(self,u,base,put,get):
  w=get(0x18c040);r=get(0x18c080);n=(w-r)&0xffffffff;assert n<=4
  for i in range(n):
   off=0x18c200+((r+i)&3)*1600;length=get(off);assert 14<=length<=1518
   packet=bytes(u.mem_read(base+off+4,length));assert self.lib.zz_net_input(packet,length)
   put(0x18c080,(r+i+1)&0xffffffff);self.tx+=1
  assert self.lib.zz_net_poll()
  w=get(0x18c0c0);r=get(0x18c100);n=(w-r)&0xffffffff;assert n<=4
  for i in range(4-n):
   size=self.lib.zz_net_receive(self.buf,1518)
   if not size:break
   off=0x18db00+((w+i)&3)*1600
   put(off,size);u.mem_write(base+off+4,self.buf.raw[:size]);put(0x18c0c0,(w+i+1)&0xffffffff);self.rx+=1
 def check(self,output,files):
  (work/'model-console.txt').write_bytes(output)
  assert self.ip.encode() in files['MTCP.CFG'],(output,files)
  assert self.tx>=2 and self.rx>=2,(self.tx,self.rx)
  print('PASS mTCP', 'manual IPv4' if args.manual else 'DHCP', self.ip,'packets TX/RX',self.tx,self.rx,flush=True)
  if server:
   assert files.get('NET.BIN')==payload,(output,len(files.get('NET.BIN',b'')))
   print('PASS mTCP HTTP transfer: 4099 exact bytes through emulated x86/ARM NE2000 and native NAT',flush=True)
net=Network()
try:model.run(0x12000040,'network',network=net)
finally:
 net.lib.zz_net_close()
 if server:server.shutdown();server.server_close()
