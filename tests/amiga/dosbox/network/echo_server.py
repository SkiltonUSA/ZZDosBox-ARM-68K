#!/usr/bin/env python3
"""Bounded test echo endpoint. Bind explicitly; only named peer addresses accepted."""
import argparse,selectors,socket,time
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--bind',required=True);p.add_argument('--port',type=int,default=55126);p.add_argument('--peer',action='append',required=True);p.add_argument('--seconds',type=int,default=1800);a=p.parse_args()
sel=selectors.DefaultSelector()
udp=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);udp.bind((a.bind,a.port));udp.setblocking(False);sel.register(udp,selectors.EVENT_READ,'udp')
tcp=socket.socket();tcp.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1);tcp.bind((a.bind,a.port));tcp.listen(4);tcp.setblocking(False);sel.register(tcp,selectors.EVENT_READ,'listen')
start=time.monotonic();clients={}
print('READY TCP/UDP',a.bind,a.port,flush=True)
try:
 while time.monotonic()-start<a.seconds:
  for key,events in sel.select(.2):
   s=key.fileobj
   if key.data=='udp':
    data,peer=s.recvfrom(2048)
    if peer[0] in a.peer and len(data)<=1500:s.sendto(data,peer);print('UDP',peer,len(data),flush=True)
   elif key.data=='listen':
    c,peer=s.accept()
    if peer[0] not in a.peer or len(clients)>=4:c.close();continue
    c.setblocking(False);clients[c]=[bytearray(),time.monotonic(),peer];sel.register(c,selectors.EVENT_READ,'client')
   else:
    state=clients[s]
    try:
     if events&selectors.EVENT_READ:
      data=s.recv(1500)
      if not data:raise ConnectionResetError
      state[0].extend(data)
      if len(state[0])>8192:raise ConnectionResetError
      print('TCP',state[2],len(data),flush=True)
     if events&selectors.EVENT_WRITE and state[0]:del state[0][:s.send(state[0])]
     sel.modify(s,selectors.EVENT_READ|(selectors.EVENT_WRITE if state[0] else 0),'client')
    except (ConnectionError,OSError):sel.unregister(s);s.close();clients.pop(s)
  for s,(_,opened,_) in list(clients.items()):
   if time.monotonic()-opened>30:sel.unregister(s);s.close();clients.pop(s)
finally:
 for s in list(clients):s.close()
 tcp.close();udp.close();sel.close()
