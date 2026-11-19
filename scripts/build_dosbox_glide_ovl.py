#!/usr/bin/env python3
"""Build an experimental DOS/4G Glide 2 call gateway, not a renderer.

Open Watcom 2026-11-01 (linux-x64), extracted locally; never installed globally.
The generated module uses no runtime library, absolute data address or GPU I/O.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/glide/ovl'
IMAGE='docker.io/amigadev/crosstools@sha256:93ca1a47903b61873f6638881b44f9f2d6086a39f1b9b916a26faff3a8ea4d3d'
def read_api(path):
 calls=[]
 for line in path.read_text().splitlines():
  if not line.strip() or line.startswith('#'):continue
  name,ret,args,*_=map(str.strip,line.split('|'))
  params=[] if args=='void' else args.split(',')
  if any('double' in p or 'long long' in p for p in params):
   raise ValueError('Unsupported argument layout: '+line)
  if not re.fullmatch(r'[A-Za-z][A-Za-z0-9_]*',name):raise ValueError(name)
  calls.append(dict(name=name,bytes=sum(12 if p.strip().startswith('PciRegister ') else 4 for p in params),**{'return':ret}))
 if len({c['name'] for c in calls})!=len(calls):raise ValueError('Duplicate export')
 return calls

def generate(calls):
 asm=['.386p','.model flat','.code','public __DLLstart_',
      '__DLLstart_ proc','mov eax,1','ret','__DLLstart_ endp']
 link=['format os2 le dll initglobal','option modname=glide2x',
       'option start=__DLLstart_','option nocaseexact','option nodefaultlibs',
       'option quiet','option map=glide.map','name GLIDE2X.OVL','file gateway.obj']
 header=['/* Generated from MIT Glide 2 ABI; IDs private to this gateway. */',
         '#ifndef GRX_GUEST_CALLS_H','#define GRX_GUEST_CALLS_H',
         '#define GRX_GATE_MAGIC 0x5a474c31u','#define GRX_GATE_PORT 0x600u',
         '#define GRX_COMMAND_PORT 0x604u','enum { GRX_CALL_INVALID=0,']
 for i,c in enumerate(calls,1):
  name=c['name'];n=c['bytes'];symbol=f'zz_{i}'
  header.append(f' GRX_CALL_{name}={i},')
  link.append(f"export '_{name.upper()}@{n}'={symbol}")
  asm.extend([f'public {symbol}',f'{symbol} proc','mov dx,0600h','in eax,dx',
   'cmp eax,05a474c31h',f'jne absent_{i}','lea ecx,[esp+4]',f'mov eax,{i}',
   'mov dx,0604h','out dx,eax','in eax,dx',f'jmp result_{i}',
   f'absent_{i}:','xor eax,eax',f'result_{i}:'])
  if c['return']=='float':asm.append('fldz')
  asm.extend([f'ret {n}' if n else 'ret',f'{symbol} endp'])
 asm.append('end')
 header.extend([' GRX_CALL_COUNT','};','typedef struct { const char *name; unsigned bytes; } grx_call_info;',
  'static const grx_call_info grx_calls[GRX_CALL_COUNT]={ {"invalid",0},'])
 header.extend(f' {{"{c["name"]}",{c["bytes"]}}},' for c in calls)
 header.extend(['};','#endif'])
 return '\n'.join(asm)+'\n','\n'.join(link)+'\n','\n'.join(header)+'\n'

def names(data,offset):
 result=[]
 while True:
  n=data[offset];offset+=1
  if not n:return result
  text=data[offset:offset+n].decode('ascii');offset+=n
  ordinal=struct.unpack_from('<H',data,offset)[0];offset+=2
  result.append((text,ordinal))

def patch_verify(path,calls):
 data=bytearray(path.read_bytes());h=struct.unpack_from('<I',data,0x3c)[0]
 assert data[:2]==b'MZ' and data[h:h+2]==b'LE'
 off=h+struct.unpack_from('<I',data,h+0x58)[0]
 assert names(data,off)[0]==('GLIDE2X',0)
 data[off+1:off+8]=b'glide2x';struct.pack_into('<I',data,h+0x10,0x8200)
 exports={n for n,o in names(data,off) if o}
 nr=struct.unpack_from('<I',data,h+0x88)[0]
 if nr:exports.update(n for n,o in names(data,nr) if o)
 assert exports=={f'_{c["name"].upper()}@{c["bytes"]}' for c in calls},exports
 path.write_bytes(data)

def main():
 p=argparse.ArgumentParser();p.add_argument('--watcom',type=Path);p.add_argument('--generate-only',action='store_true');p.add_argument('--connection',default='nuflix-converter-root');a=p.parse_args()
 OUT.mkdir(parents=True,exist_ok=True)
 api=ROOT/'amiga/dosbox/glide/abi/glide2x.api';calls=read_api(api)
 for name,data in zip(('gateway.asm','gateway.lnk','guest_calls.h'),generate(calls)):(OUT/name).write_text(data)
 if a.generate_only:return
 if not a.watcom:p.error('--watcom must name the extracted Open Watcom directory')
 tool=a.watcom.resolve()
 base=['podman','--connection',a.connection,'run','--rm','--platform','linux/amd64',
  '-v',str(ROOT)+':/work','-v',str(tool)+':/watcom:ro','-w','/work/'+str(OUT.relative_to(ROOT)),IMAGE]
 subprocess.run(base+['/watcom/binl64/wasm','-zq','-we','-fo=gateway.obj','gateway.asm'],check=True)
 subprocess.run(base+['/watcom/binl64/wlink','@gateway.lnk'],check=True)
 patch_verify(OUT/'GLIDE2X.OVL',calls)
 record={'watcom_release':'2026-11-01-Build','image':IMAGE,'exports':len(calls),
  'sources':{str(x.relative_to(ROOT)):hashlib.sha256(x.read_bytes()).hexdigest() for x in (api,Path(__file__))},
  'ovl_sha256':hashlib.sha256((OUT/'GLIDE2X.OVL').read_bytes()).hexdigest()}
 (OUT/'build.json').write_text(json.dumps(record,indent=2)+'\n');print(OUT/'GLIDE2X.OVL')
if __name__=='__main__':main()
