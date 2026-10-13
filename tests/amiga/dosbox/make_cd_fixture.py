"""Original, tiny ISO9660 fixture. No commercial disc contents."""
import struct
from pathlib import Path

def make(path):
 data=bytearray(32*2048)
 def both16(n):return struct.pack('<H',n)+struct.pack('>H',n)
 def both32(n):return struct.pack('<I',n)+struct.pack('>I',n)
 def record(name,sector,size,flags=0):
  n=33+len(name)+(len(name)%2==0);r=bytearray(n);r[0]=n;r[2:10]=both32(sector);r[10:18]=both32(size);r[18:25]=bytes([126,10,13,0,0,0,0]);r[25]=flags;r[28:32]=both16(1);r[32]=len(name);r[33:33+len(name)]=name;return r
 p=memoryview(data)[16*2048:17*2048];p[0:7]=b'\1CD001\1';p[8:40]=b'ZZDOSBOX'.ljust(32);p[40:72]=b'ZZCD_TEST'.ljust(32);p[80:88]=both32(32);p[120:124]=both16(1);p[124:128]=both16(1);p[128:132]=both16(2048);p[156:190]=record(b'\0',20,2048,2)
 # Mandatory little/big-endian path tables (MSCDEX uses these for directories).
 for sector,endian in [(18,'<'),(19,'>')]:
  table=b'\1\0'+struct.pack(endian+'IH',20,1)+b'\0\0'+b'\6\0'+struct.pack(endian+'IH',21,1)+b'SUBDIR'
  data[sector*2048:sector*2048+len(table)]=table
 p[132:140]=both32(len(table));p[140:144]=struct.pack('<I',18);p[148:152]=struct.pack('>I',19);p[881]=1
 data[17*2048:17*2048+7]=b'\xffCD001\1'
 text=b'ZZCD data read PASS\r\n';large=bytes(i%251 for i in range(5003))
 root=b''.join([record(b'\0',20,2048,2),record(b'\1',20,2048,2),record(b'HELLO.TXT;1',22,len(text)),record(b'LARGE.BIN;1',24,len(large)),record(b'SUBDIR',21,2048,2)])
 sub=b''.join([record(b'\0',21,2048,2),record(b'\1',20,2048,2),record(b'NESTED.TXT;1',23,len(text))])
 data[20*2048:20*2048+len(root)]=root;data[21*2048:21*2048+len(sub)]=sub
 for at,blob in [(22,text),(23,text),(24,large)]:data[at*2048:at*2048+len(blob)]=blob
 path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
if __name__=='__main__':make(Path('.context/amiga/dosbox-startup-cd/TEST.ISO'))
