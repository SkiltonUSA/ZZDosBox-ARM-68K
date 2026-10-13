#!/usr/bin/env python3
"""Build DOSBox x86 diagnostic for ZZ9000 Core1. Does not deploy or launch."""
import argparse,hashlib,importlib.util,json,re,subprocess
from pathlib import Path
import build_dosbox as b
from build_zz9000_debug import unpack_elf
from amiga import PIN,container_command
ROOT=b.ROOT;OUT=b.OUT;SRC=b.SRC
import os
cmd=json.loads(os.environ.get('DOSBOX_ARM_COMMAND','["podman","--connection","nuflix-converter-root","exec","zzffmpeg-toolchain"]'))
def inside(p):return '/work/'+str(Path(p).relative_to(ROOT))
def run(args):subprocess.run([*cmd,*map(str,args)],check=True)
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--container-command');args=parser.parse_args()
host_command=json.loads(args.container_command) if args.container_command else container_command()
sources=b.prepare();common=ROOT/'amiga/mesen_bench'
for name in ['worker.cpp','mmu.c','mmu.h','mmu_asm.S','mmu_plan.h']:
 s=(common/name).read_text()
 if name=='worker.cpp':
  s=s.replace('extern "C" void mesen_progress', 'extern "C" void db_output(const char *text) { for(unsigned i=0;i<160;i++){shared[MB_REPLY+128+i]=(uint8_t)text[i];if(!text[i])break;} barrier(); }\nextern "C" void mesen_progress')
 if name=='mmu_plan.h':
  moves={'0x40000u':'0x100000u','0x50000u':'0x118000u','0x60000u':'0x120000u','0x80000u':'0x180000u','0x84000u':'0x184000u','0x82000u':'0x182000u','0x82400u':'0x182400u'}
  s=re.sub('|'.join(moves),lambda m:moves[m[0]],s)
 (OUT/name).write_text(s)
subprocess.run(['clang','-O2','-I'+str(OUT),'-DMB_ALLOCATION=0x2000000u',str(ROOT/'tests/amiga/mesen/mmu_plan.c'),'-o',str(OUT/'test-mmu')],check=True)
subprocess.run([str(OUT/'test-mmu')],check=True)
flags=['-Os' ,'-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-fvisibility=hidden','-ffunction-sections','-fdata-sections','-fno-stack-protector','-DZZ_BLOCK_SIZE=0x2000000','-DZZ_CONTROL=0x180000','-DMB_MMU','-DMB_DCACHE','-DMB_ALLOCATION=0x2000000u',
 *['-I'+inside(p) for p in [OUT,OUT/'include',OUT/'src/cpu',SRC,common,ROOT/'amiga/arm_debug',ROOT/'amiga/arm_debug/zz9000']],'-include',inside(ROOT/'amiga/pocketsnes/libc_config.h')]
sources += [SRC/'adapter.cpp',SRC/'runtime.cpp',SRC/'bench.cpp',ROOT/'amiga/ffmpeg_bench/heap.c',ROOT/'amiga/mesen_snes/system/memory.c',OUT/'worker.cpp',OUT/'mmu.c',OUT/'mmu_asm.S',ROOT/'.context/amiga/ffmpeg-bench/picolibc-1.8.12/libc/machine/arm/setjmp.S']
header_digest=hashlib.sha256(b''.join(p.read_bytes() for p in sorted(OUT.rglob('*.h')) if p.name not in ('zz_payload.h','expected.h'))+b''.join(p.read_bytes() for p in sorted(SRC.glob('*.h')))).digest()
objects=[]
for i,src in enumerate(sources):
 obj=OUT/(str(i)+'-'+src.stem+'.arm.o');objects.append(obj)
 cpp=src.suffix=='.cpp'
 extra=['-std=c++17','-fno-exceptions','-fno-rtti','-fno-threadsafe-statics','-fno-use-cxa-atexit'] if cpp else []
 digest=hashlib.sha256(src.read_bytes()+header_digest+str(flags+extra).encode()).hexdigest();stamp=obj.with_suffix('.sha256')
 if not stamp.exists() or stamp.read_text()!=digest:
  run(['arm-none-eabi-g++' if cpp else 'arm-none-eabi-gcc',*flags,*extra,'-c',inside(src),'-o',inside(obj)])
  stamp.write_text(digest)
libc=ROOT/'.context/amiga/pocketsnes/libc.a'
assert libc.exists(),'Build PIC libc with scripts/build_pocketsnes_libc.py first'
elf=OUT/'dosbox.elf'
run(['arm-none-eabi-ld','-shared','-Bsymbolic','--gc-sections','--exclude-libs=ALL','--no-undefined','-T',inside(SRC/'payload.ld'),'-Map='+inside(OUT/'link.map'),*map(inside,objects),'--start-group',inside(libc),'/usr/lib/gcc/arm-none-eabi/15/thumb/v7-a/nofp/libgcc.a','--end-group','-o',inside(elf)])
spec=importlib.util.spec_from_file_location('pack',SRC/'elf.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
image,rel,entry=m.unpack(elf.read_bytes())
run(['arm-none-eabi-gcc',*flags,'-c',inside(ROOT/'amiga/arm_debug/zz9000/mapping_probe.S'),'-o',inside(OUT/'mapping.o')])
run(['arm-none-eabi-ld','-shared','-Bsymbolic','--no-undefined','-T',inside(ROOT/'amiga/arm_debug/zz9000/payload.ld'),inside(OUT/'mapping.o'),'-o',inside(OUT/'mapping.elf')])
mapping,mrel,point=unpack_elf((OUT/'mapping.elf').read_bytes());assert not mrel and not point and len(mapping)<=1024
def array(n,data):return 'static const unsigned char '+n+'[]={'+','.join(map(str,data))+'};\n'
(OUT/'zz_payload.h').write_text(f'#define ZZ_ENTRY {entry}u\n'+array('zz_image',image)+array('zz_mapping_probe',mapping)+'static const unsigned long zz_relocations[]={'+','.join(map(str,rel))+'};\n')
(OUT/'expected.h').write_text('#define MESEN_EXPECT_CYCLES 4u\n#define MESEN_EXPECT_HASH 0xf23411c5u\n')
host=(common/'host.c').read_text().replace('8991','1').replace('instructions=1','fixtures=1').replace('cycles=','slices=').replace('MESEN CPU-ONLY','DOSBOX X86 INTERPRETER DIAGNOSTIC').replace('MESEN_CPU PASS; CPU-only throughput, NOT emulated video FPS','DOSBOX_INTERPRETER PASS; arithmetic/memory/DOS text; NOT a complete PC or game FPS')
host=host.replace('syncio->pull(reply,96,NULL);','syncio->pull(reply,320,NULL);')
host=host.replace('    for(i=0;i<11;i++) {','    printf("DOS OUTPUT: %.*s\\n",159,(const char *)(shared+MB_REPLY+128));\n    for(i=0;i<11;i++) {')
(OUT/'host.c').write_text(host)
relative=OUT.relative_to(ROOT);binary=OUT/'ZZDosBox-Test'
subprocess.run([*host_command,'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror',
 '-DZZ_HEADLESS_BENCH','-DZZ_RELEASE','-DZZ_BLOCK_SIZE=0x2000000','-DZZ_CONTROL=0x180000','-DMB_ALLOCATION=0x2000000u','-DZZ_APP_NAME="ZZDosBox-Test"','-DZZ_APP_VERSION="0.1.0-dev"','-DZZ_MIN_STACK=65536','-DMB_MMU','-DMB_DCACHE','-DZZ_HEADLESS_MMU','-DZZ_HEADLESS_ICACHE',
 '-I'+str(relative),'-Iamiga/arm_debug','-Iamiga/arm_debug/zz9000','-Iamiga/mesen_bench','amiga/arm_debug/zz9000/launcher.c',str(relative/'host.c'),'-lamiga','-o',str(relative/binary.name)],cwd=ROOT,check=True)
assert binary.read_bytes()[:4]==b'\0\0\x03\xf3'
record={'upstream':json.loads((SRC/'upstream.json').read_text()),'scope':'Original diagnostic COM using upstream x86 interpreter; minimal INT21 text/exit adapter, no full DOS kernel/devices', 'image_bytes':len(image),'relocations':len(rel),'allocation':0x2000000,
 'outputs':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in [elf,binary,OUT/'integration.patch',OUT/'Diagnostic.com']},'libc_sha256':hashlib.sha256(libc.read_bytes()).hexdigest(),
 'source_hashes':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [*sources,*SRC.glob('*.*'),ROOT/'scripts/build_dosbox.py',Path(__file__),ROOT/'amiga/arm_debug/zz9000/launcher.c',ROOT/'amiga/arm_debug/zz9000/layout.h',ROOT/'amiga/arm_debug/zz9000/mapping_probe.S',OUT/'host.c',OUT/'mmu_plan.h'] if p.is_file()},
 'arm_flags':flags,'compiler':subprocess.check_output([*cmd,'arm-none-eabi-g++','--version'],text=True)}
(OUT/'build.json').write_text(json.dumps(record,indent=2)+'\n')
print('Built',binary,binary.stat().st_size,'bytes; ARM image',len(image),'relocations',len(rel))
