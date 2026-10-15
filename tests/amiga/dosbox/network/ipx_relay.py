#!/usr/bin/env python3
"""Bounded test peer implementing DOSBox's IPXNET registration/UDP format.
Not a production relay. Used for byte-exact guest API tests, no game media.
"""
import argparse,socket,struct,time,json

def packet(dest,src,payload=b''):
    return b'\xff\xff'+struct.pack('>H',30+len(payload))+b'\0\0'+dest+src+payload

def endpoint(ip,port,net=0,sock=2):
    return struct.pack('>I',net)+socket.inet_aton(ip)+struct.pack('>HH',port,sock)

class Relay:
    def __init__(self,bind,port=0):
        self.sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);self.sock.bind((bind,port));self.sock.setblocking(False)
        self.address=self.sock.getsockname();self.count=0;self.registrations=0;self.broadcasts=0;self.stop=False
    def poll(self):
        for _ in range(16):
            try:data,peer=self.sock.recvfrom(4096)
            except BlockingIOError:return
            if len(data)<30 or len(data)>1424 or data[:2]!=b'\xff\xff' or int.from_bytes(data[2:4],'big')!=len(data):continue
            self.count+=1
            if data[10:16]==bytes(6) and data[16:18]==b'\0\2':
                self.registrations+=1
                self.sock.sendto(packet(endpoint(*peer),endpoint(*self.address,net=1)),peer)
            elif data[30:]==b'ZZIPXHELLO' and data[10:16]==b'\xff'*6:
                self.broadcasts+=1
                # A distinct remote node answers the guest's directed receive ECB.
                self.sock.sendto(packet(data[18:30],endpoint(self.address[0],self.address[1],sock=0x4523),b'ZZIPXREPLY'),peer)
            elif data[16:18]==b'\0\2' and data[10:16]==b'\xff'*6:
                self.sock.sendto(packet(data[18:30],endpoint(*self.address)),peer)
    def run(self):
        while not self.stop:self.poll();time.sleep(.001)
    def close(self):self.stop=True;self.sock.close()
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--bind',required=True);p.add_argument('--port',type=int,default=19900);p.add_argument('--seconds',type=int,default=300);a=p.parse_args();r=Relay(a.bind,a.port)
    print('IPX test relay',r.address,flush=True)
    try:
        until=time.monotonic()+a.seconds
        while time.monotonic()<until:r.poll();time.sleep(.002)
    finally:
        print(json.dumps({'packets':r.count,'registrations':r.registrations,'broadcasts':r.broadcasts}),flush=True);r.close()

class UpstreamPeer:
    """Second independent client through unmodified DOSBox relay source."""
    def __init__(self,bind,root,port=0):
        import subprocess
        probe=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);probe.bind((bind,port));port=probe.getsockname()[1];probe.close()
        self.address=(bind,port);self.stop=False;self.broadcasts=0;self.count=0;self.registrations=1
        self.log=open(root/'.context/amiga/dosbox-ipx/upstream-relay.log','w')
        self.process=subprocess.Popen([str(root/'.context/amiga/dosbox-ipx/upstream-relay/ipxrelay'),bind,str(port),'600'],stdout=subprocess.PIPE,stderr=self.log,text=True)
        assert 'ready' in self.process.stdout.readline()
        self.sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM);self.sock.bind((bind,0));self.sock.connect(self.address);self.sock.settimeout(2)
        self.sock.send(packet(bytes(10)+b'\0\2',bytes(10)+b'\0\2'))
        ack=self.sock.recv(2048);assert len(ack)==30 and ack[16:18]==b'\0\2';self.node=ack[6:18];self.sock.setblocking(False)
    def poll(self):
        for _ in range(16):
            try:data=self.sock.recv(2048)
            except BlockingIOError:return
            self.count+=1
            if data[30:]==b'ZZIPXHELLO':
                assert data[10:16]==b'\xff'*6
                self.broadcasts+=1;self.sock.send(packet(data[18:30],self.node[:10]+b'\x45\x23',b'ZZIPXREPLY'))
            elif data[16:18]==b'\0\2':self.sock.send(packet(data[18:30],self.node))
    def run(self):
        while not self.stop:self.poll();time.sleep(.001)
    def close(self):
        self.stop=True;self.sock.close();self.process.terminate();self.process.wait(timeout=3);self.process.stdout.close();self.log.close()
