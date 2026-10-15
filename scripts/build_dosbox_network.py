#!/usr/bin/env python3
"""Build the isolated Roadshow NAT backend probe; never deploy or launch it."""
import argparse,hashlib,json,subprocess,shlex
from pathlib import Path
from prepare_dosbox_network import prepare,ROOT,OUT,SRC
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--native',action='store_true');p.add_argument('--library',action='store_true');p.add_argument('--model-library',action='store_true',help='Native shared library for the ARM instruction model; not sanitized');p.add_argument('--socket-probe',action='store_true',help='Build the standalone Roadshow UDP diagnostic without libslirp');a=p.parse_args()
if a.socket_probe:
    if a.native or a.library or a.model_library:p.error('--socket-probe is a separate Amiga-only diagnostic')
    dest=OUT/'amiga';dest.mkdir(parents=True,exist_ok=True)
    source=ROOT/'tests/amiga/dosbox/network/roadshow_socket_probe.c';binary=dest/'ZZSocketProbe'
    image=json.loads((ROOT/'amiga/upstream.json').read_text())['image']
    command=['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',str(ROOT)+':/work','-w','/work',image,'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-Wall','-Wextra','-Werror',str(source.relative_to(ROOT)),'-lamiga','-o',str(binary.relative_to(ROOT))]
    subprocess.run(command,check=True,cwd=ROOT)
    (dest/'socket-probe-build.json').write_text(json.dumps({'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'bytes':binary.stat().st_size,'image':image},indent=2)+'\n')
    print('Built',binary);raise SystemExit(0)
if a.model_library and (not a.native or a.library):p.error('--model-library requires --native and excludes --library')
dest=OUT/('native' if a.native else 'amiga');dest.mkdir(parents=True,exist_ok=True);inc=prepare(dest/'slirp')
sources=sorted(p for p in inc.glob('*.c') if p.stem not in ('state','stream','vmstate','version'))
sources += [SRC/'backend.c']
if not a.library and not a.model_library:sources += [ROOT/'tests/amiga/dosbox/network/backend_probe.c']
def rel(p):return str(p.relative_to(ROOT))
flags=['-std=gnu99','-I'+rel(inc),'-I'+rel(SRC/'compat'),'-I'+rel(SRC)]
binary=dest/('libzznetwork-model.dylib' if a.model_library else 'libzznetwork.a' if a.library else 'ZZNetProbe')
if a.model_library:
    command=['clang','-O1','-g','-fPIC','-shared',*flags,*map(rel,sources),'-lresolv','-o',rel(binary)]
elif a.native and not a.library:
    cc=['clang','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',*flags]
    command=[*cc,*map(rel,sources),'-lresolv','-o',rel(dest/'ZZNetProbe')]
elif not a.library:
    image=json.loads((ROOT/'amiga/upstream.json').read_text())['image']
    command=['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',str(ROOT)+':/work','-w','/work',image,'m68k-amigaos-gcc','-noixemul','-m68020','-O2','-Werror=frame-larger-than=8192',*flags,*map(rel,sources),'-lamiga','-o',rel(dest/'ZZNetProbe')]
else:
    if a.native:raise SystemExit('native archive is not used by the runtime')
    image=json.loads((ROOT/'amiga/upstream.json').read_text())['image']
    objects=[];commands=[]
    for source in sources:
        obj=dest/(source.stem+'.o');objects.append(obj)
        commands.append(shlex.join(['m68k-amigaos-gcc','-noixemul','-m68020','-O2','-Werror=frame-larger-than=8192',*flags,'-c',rel(source),'-o',rel(obj)]))
    commands.append(shlex.join(['m68k-amigaos-ar','rcs',rel(binary),*map(rel,objects)]))
    command=['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',str(ROOT)+':/work','-w','/work',image,'sh','-ec','\n'.join(commands)]
r=subprocess.run(command,cwd=ROOT)
if r.returncode:raise SystemExit(r.returncode)
record={'native':a.native,'upstream':json.loads((SRC/'upstream.json').read_text()),'bytes':binary.stat().st_size,'sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'sources':{rel(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [*sources,*SRC.glob('*.h'),*SRC.glob('compat/*.h'),Path(__file__).resolve(),ROOT/'scripts/prepare_dosbox_network.py']}}
(dest/('model-library-build.json' if a.model_library else 'library-build.json' if a.library else 'build.json')).write_text(json.dumps(record,indent=2)+'\n')
print('Built',rel(binary),record['bytes'],record['sha256'])
