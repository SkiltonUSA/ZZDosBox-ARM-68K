#!/usr/bin/env python3
"""Build an isolated offscreen 68k/Core0 GPU probe; never installs or runs it."""
import argparse,hashlib,json,re,subprocess,zlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--connection',default='nuflix-converter-root')
p.add_argument('--height',type=int,choices=(65,72),default=72);a=p.parse_args()
geometry='-DGRX_PROBE_HEIGHT='+str(a.height)
out=ROOT/'.context/amiga/glide';out.mkdir(parents=True,exist_ok=True)
base=Path('amiga/dosbox/glide')
sources=[base/n for n in ('probe.c','probe_scene.c','translator.c','descriptor.c')]
image=json.loads((ROOT/'amiga/upstream.json').read_text())['image']
generator=Path('tests/amiga/dosbox/glide/fixtures.c')
subprocess.run(['clang','-std=c11','-O2','-Wall','-Wextra','-Werror',geometry,'-I.',
 str(generator),*map(str,sources[1:]),'-o',str(out/'make-fixtures')],cwd=ROOT,check=True)
with (out/'fixtures.h').open('w') as f:
 subprocess.run([str(out/'make-fixtures')],stdout=f,check=True)
# Independently compute expected firmware CRCs from the generated wire bytes.
rows=re.findall(r'^\{([0-9,]+)\},$',(out/'fixtures.h').read_text(),re.M)
assert len(rows)==10
crcs=[zlib.crc32(bytes(int(x) for x in row.split(',') if x)) for row in rows]
with (out/'fixtures.h').open('a') as f:
 f.write('static const unsigned long grx_probe_crc[2][5]={{'+','.join(map(str,crcs[:5]))+'},{'+','.join(map(str,crcs[5:]))+'}};\n')
subprocess.run(['podman','--connection',a.connection,'run','--rm','--platform','linux/amd64',
 '-v',f'{ROOT}:/work','-w','/work',image,'m68k-amigaos-gcc','-std=c99','-noixemul',
 '-m68020','-O2','-Wall','-Wextra','-Werror',geometry,'-I.context/amiga/glide',str(sources[0]),'-lamiga',
 '-o','.context/amiga/glide/ZZGlide-Probe'],cwd=ROOT,check=True)
inputs=[ROOT/s for s in sources]+list((ROOT/base).glob('*.h'))+[Path(__file__),ROOT/generator]
(out/'probe-build.json').write_text(json.dumps({'image':image,'height':a.height,'source_sha256':{
 str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in inputs},
 'fixtures_sha256':hashlib.sha256((out/'fixtures.h').read_bytes()).hexdigest(),
 'binary_sha256':hashlib.sha256((out/'ZZGlide-Probe').read_bytes()).hexdigest()},indent=2)+'\n')
print(out/'ZZGlide-Probe')
