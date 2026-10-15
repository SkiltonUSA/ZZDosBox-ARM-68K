#!/usr/bin/env python3
"""Native real-UDP backend bounds, peer filtering and lifecycle tests."""
import argparse,ctypes,socket,time
from pathlib import Path
from ipx_relay import endpoint,packet
ROOT=Path(__file__).resolve().parents[4]
p=argparse.ArgumentParser();p.add_argument('--bind',required=True);a=p.parse_args()
class Config(ctypes.Structure):
 _fields_=[(n,ctypes.c_uint) for n in ('enabled','manual','mtu','retries','timeout')]+[(n,ctypes.c_char*16) for n in ('ip','mask','gateway','dns')]+[('hostname',ctypes.c_char*32),('protocol',ctypes.c_uint),('ipx_port',ctypes.c_uint),('ipx_server',ctypes.c_char*16)]
class Stats(ctypes.Structure):_fields_=[(n,ctypes.c_uint32) for n in ('sent','received','dropped','errors')]
lib=ctypes.CDLL(str(ROOT/'.context/amiga/dosbox-network/native/libzznetwork-model.dylib'))
lib.zz_net_configure.argtypes=[ctypes.POINTER(Config)];lib.zz_net_open.argtypes=[ctypes.c_char_p];lib.zz_net_input.argtypes=[ctypes.c_void_p,ctypes.c_uint];lib.zz_net_receive.argtypes=[ctypes.c_void_p,ctypes.c_uint];lib.zz_net_statistics.restype=Stats
server=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);server.bind((a.bind,0));server.settimeout(1)
attacker=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);attacker.bind((a.bind,0))
c=Config(1,0,1500,3,10,b'10.0.2.15',b'255.255.255.0',b'10.0.2.2',b'',b'zzdosbox',1,server.getsockname()[1],a.bind.encode())
buf=ctypes.create_string_buffer(1518)
try:
 for cycle in range(2):
  assert lib.zz_net_configure(ctypes.byref(c)) and lib.zz_net_open(None)
  frame=packet(endpoint(a.bind,12345,sock=0x4523),endpoint(a.bind,23456,sock=0x4523),b'exact IPX data')
  assert lib.zz_net_input(frame,len(frame));data,client=server.recvfrom(2000);assert data==frame
  # Truncated, oversized, mismatched length and bad checksum packets never reach ARM.
  for bad in [b'',b'x'*29,b'x'*2000,frame[:-1],b'\0\0'+frame[2:]]:server.sendto(bad,client)
  attacker.sendto(frame,client)
  until=time.monotonic()+.05
  while time.monotonic()<until:assert lib.zz_net_poll();time.sleep(.001)
  assert lib.zz_net_receive(buf,1518)==0
  assert lib.zz_net_statistics().dropped==5
  server.sendto(frame,client)
  until=time.monotonic()+1;got=0
  while time.monotonic()<until:
   assert lib.zz_net_poll();got=lib.zz_net_receive(buf,1518)
   if got:break
  assert got==len(frame) and buf.raw[:got]==frame
  assert lib.zz_net_input(b'bad',3) and lib.zz_net_statistics().dropped==6
  assert lib.zz_net_statistics().errors==0
  lib.zz_net_close();lib.zz_net_close()
 print('PASS native IPX UDP: exact bytes, malformed/truncated/oversized drops, source-peer filter, nonblocking poll and repeated close/reopen')
finally:lib.zz_net_close();server.close();attacker.close()
