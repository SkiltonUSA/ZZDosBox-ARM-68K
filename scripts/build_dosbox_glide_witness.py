#!/usr/bin/env python3
"""Build an isolated Core1 readback witness; never installs or runs it."""
import argparse, hashlib, json, subprocess, sys
from pathlib import Path
from build_zz9000_debug import unpack_elf
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--connection',default='nuflix-converter-root');a=p.parse_args()
def run(args): subprocess.run(list(map(str,args)),cwd=ROOT,check=True)
run([sys.executable,'scripts/build_dosbox_glide_probe.py','--height','65','--connection',a.connection])
out=ROOT/'.context/amiga/glide/witness';out.mkdir(exist_ok=True)
src=Path('amiga/dosbox/glide');zz=Path('amiga/arm_debug/zz9000')
common=['-DZZ_BLOCK_SIZE=0x20000','-DZZ_CONTROL=0x8000','-DGRX_PROBE_HEIGHT=65']
includes=['-I'+str(out.relative_to(ROOT)),'-I.context/amiga/glide','-Iamiga/arm_debug','-I'+str(zz),'-I'+str(src)]
flags=['--target=arm-none-eabi','-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-ffreestanding',
 '-fno-builtin','-fPIC','-fvisibility=hidden','-O2','-g','-Wall','-Wextra','-Werror',
 '-DZZ_VIDEO','-DZZ_VIDEO_ICACHE',*common,*includes]
ld='/opt/homebrew/opt/lld/bin/ld.lld';objects=[]
for source in [src/'witness_entry.S',src/'witness_worker.c']:
 obj=out/(source.stem+'.o');objects.append(obj);run(['clang',*flags,'-c',source,'-o',obj])
run([ld,'-shared','-Bsymbolic','--no-undefined','-T',zz/'payload.ld',*objects,'-o',out/'worker.elf'])
payload,relocations,entry=unpack_elf((out/'worker.elf').read_bytes(),0x8000)
run(['clang',*flags,'-c',zz/'mapping_probe.S','-o',out/'mapping.o'])
run([ld,'-shared','-Bsymbolic','--no-undefined','-T',zz/'payload.ld',out/'mapping.o','-o',out/'mapping.elf'])
mapping,rel,point=unpack_elf((out/'mapping.elf').read_bytes())
assert not rel and not point and len(mapping)<=1024
def array(name,data):return 'static const unsigned char '+name+'[]={'+','.join(map(str,data))+'};\n'
(out/'zz_payload.h').write_text(f'#define ZZ_ENTRY {entry}u\n'+array('zz_image',payload)+array('zz_mapping_probe',mapping)+
 'static const unsigned long zz_relocations[]={'+','.join(map(str,relocations))+'};\n')
image=json.loads((ROOT/'amiga/upstream.json').read_text())['image'];binary=out/'ZZGlide-Witness'
run(['podman','--connection',a.connection,'run','--rm','--platform','linux/amd64',
 '-v',f'{ROOT}:/work','-w','/work',image,'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020',
 '-O2','-g','-Wall','-Wextra','-Werror','-DZZ_HEADLESS_BENCH','-DZZ_HEADLESS_ICACHE','-DZZ_RELEASE',
 '-DGRX_WITNESS','-DZZ_APP_NAME="ZZGlideWitness"','-DZZ_APP_VERSION="0.1"','-DZZ_MIN_STACK=65536',
 *common,*includes,zz/'launcher.c',src/'probe.c','-lamiga','-o',binary.relative_to(ROOT)])
assert binary.read_bytes()[:4]==b'\x00\x00\x03\xf3'
inputs=list((ROOT/src).glob('*.[ch]'))+list((ROOT/src).glob('witness*'))+[
 ROOT/zz/'launcher.c',ROOT/zz/'mapping_probe.S',ROOT/zz/'payload.ld',ROOT/zz/'layout.h',
 ROOT/'amiga/arm_debug/protocol.h',Path(__file__),ROOT/'scripts/build_dosbox_glide_probe.py',out.parent/'fixtures.h']
(out/'build.json').write_text(json.dumps(dict(image=image,height=65,
 sources={str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in inputs},
 executable_sha256=hashlib.sha256(binary.read_bytes()).hexdigest()),indent=2)+'\n')
print(binary)
