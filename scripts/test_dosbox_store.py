#!/usr/bin/env python3
"""Compare every lazy record with source facts; reject damaged/stale files."""
import json,struct,subprocess,zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];out=ROOT/'.context/amiga/dosbox-store-tests';out.mkdir(parents=True,exist_ok=True)
src=ROOT/'.context/amiga/dosbox-launcher/Compatibility.dat';data=src.read_bytes();magic=data[:8];count,crc,size=struct.unpack_from('>III',data,8)
assert magic==b'ZZCD0001' and size==len(data) and zlib.crc32(data[20:20+16*count])==crc
facts=sorted(json.loads((ROOT/'amiga/dosbox/compatibility/dosbox-details.json').read_text())['games'],key=lambda g:g['game_id'])
assert count==len(facts)
for i,g in enumerate(facts):
 gid,offset,n,checksum=struct.unpack_from('>IIII',data,20+i*16);record=data[offset:offset+n];assert gid==g['game_id'] and zlib.crc32(record)==checksum
 assert struct.unpack_from('>I',record)[0]==g['comment_count']
 history='|'.join(v['dosbox_version']+'  '+v['status']+('  '+str(v['rating_percent'])+'%' if v['rating_percent'] is not None else '') for v in g['versions'])
 fields=[g['publisher'],g['tested_by'],history,g['latest_comment_at'],g['latest_comment_author'],g['note_preview'],g['discussion']['summary']]
 expected=[''.join(c if ord(c)>=32 and ord(c)!=127 or c=='\n' else ' ' for c in f).encode('latin-1',errors='replace') for f in fields]
 assert record[4:].split(b'\0')==expected+[b'']
exe=out/'check'
subprocess.run(['clang','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-Iamiga/dosbox/launcher','tests/amiga/dosbox/compatibility_store.c','-o',str(exe)],cwd=ROOT,check=True)
def check(content,name,mode):
 p=out/name;p.write_bytes(content);subprocess.run([str(exe),str(p),mode],check=True)
check(data,'valid.dat','valid')
for n in [0,7,19,20,len(data)-1]:check(data[:n],f'truncated-{n}.dat','bad-index')
check(data+b'junk','trailing.dat','bad-index')
for offset in [0,8,12,16,20,28]:
 damaged=bytearray(data);damaged[offset]^=1;check(damaged,f'changed-{offset}.dat','bad-index')
damaged=bytearray(data);offset=struct.unpack_from('>I',data,24)[0];damaged[offset+5]^=1;check(damaged,'changed-record.dat','bad-record')
subprocess.run([str(exe),str(out/'missing.dat'),'bad-index'],check=True)
print('PASS every record equals source; missing/truncated/stale/corrupt stores, retry, close and bounded lookup')
