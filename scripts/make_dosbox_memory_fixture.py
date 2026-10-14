#!/usr/bin/env python3
"""Original DOS COM fixture: wait for an external edit of translated code."""
from pathlib import Path
import struct
def make():
    b=bytearray.fromhex('31c08ec08cc826a30005') # ES=0; publish CS at 0000:0500
    b+=bytes.fromhex('26c7060205');offset=len(b);b+=b'\0\0'
    b+=bytes.fromhex('2666c70604054d454d31') # marker MEM1
    loop=len(b);b+=b'\xe8';call=len(b);b+=b'\0\0'
    b+=bytes.fromhex('3d222275');b+=bytes([(loop-(len(b)+1))&255])
    b+=b'\xba';message=len(b);b+=b'\0\0';b+=bytes.fromhex('b409cd21b8004ccd21')
    function=len(b);b+=bytes.fromhex('b81111c3')
    text=len(b);b+=b'MEMORY PATCH PASS\r\n$'
    struct.pack_into('<H',b,offset,0x100+function+1)
    struct.pack_into('<h',b,call,function-(call+2))
    struct.pack_into('<H',b,message,0x100+text)
    return bytes(b)
if __name__=='__main__':
    import sys
    Path(sys.argv[1]).write_bytes(make())
