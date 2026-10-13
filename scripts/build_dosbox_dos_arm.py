#!/usr/bin/env python3
"""Build DOSBox DOS kernel and optional console / shell for ZZ9000 Core1. Does not deploy or launch."""
import argparse,hashlib,importlib.util,json,re,subprocess
from pathlib import Path
import build_dosbox_dos as b
from build_zz9000_debug import unpack_elf
from amiga import PIN,container_command
ROOT=b.ROOT;OUT=b.OUT;SRC=b.SRC;BASE=ROOT/"amiga/dosbox"
import os
cmd=json.loads(os.environ.get('DOSBOX_ARM_COMMAND','["podman","--connection","nuflix-converter-root","exec","zzffmpeg-toolchain"]'))
def inside(p):return '/work/'+str(Path(p).relative_to(ROOT))
def run(args):subprocess.run([*cmd,*map(str,args)],check=True)
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--container-command');parser.add_argument('--console',action='store_true');parser.add_argument('--shell',action='store_true');parser.add_argument('--pc',action='store_true');parser.add_argument('--pc-cpu-opt',choices=['Os','O2','O3'],help='PC interpreter optimisation level (default: O2 for PC)');parser.add_argument('--pc-hardware-opt',choices=['Os','O2'],help='PIC, VGA drawing and VGA memory optimisation level (default: O2 for PC)');args=parser.parse_args()
if not args.pc and args.pc_cpu_opt is not None:parser.error('--pc-cpu-opt requires --pc')
if not args.pc and args.pc_hardware_opt is not None:parser.error('--pc-hardware-opt requires --pc')
args.pc_cpu_opt=args.pc_cpu_opt or ('O2' if args.pc else 'Os')
args.pc_hardware_opt=args.pc_hardware_opt or ('O2' if args.pc else 'Os')
host_command=json.loads(args.container_command) if args.container_command else container_command()
console=ROOT/'amiga/dosbox/console'
shell=ROOT/'amiga/dosbox/shell'
pc=ROOT/'amiga/dosbox/pc'
if args.pc:args.shell=True
if args.shell:args.console=True
if args.console:OUT=ROOT/'.context/amiga/dosbox-console';b.OUT=OUT
if args.shell:
 import build_dosbox_shell as sb
 if args.pc:
  import build_dosbox_pc as pb
  OUT=pb.OUT;b.OUT=OUT;sources=pb.prepare()
 else:OUT=sb.OUT;b.OUT=OUT;sources=sb.prepare()
else:sources=b.prepare()
if args.console:b.console_fixtures()
common=ROOT/'amiga/mesen_bench'
for name in ['worker.cpp','mmu.c','mmu.h','mmu_asm.S','mmu_plan.h']:
 s=(common/name).read_text()
 if name=='worker.cpp':
  s=s.replace('extern "C" void mesen_progress', 'extern "C" volatile uint8_t *db_shared(){return shared;}\nextern "C" void db_output(const char *text) { for(unsigned i=0;i<160;i++){shared[MB_REPLY+128+i]=(uint8_t)text[i];if(!text[i])break;} barrier(); }\nextern "C" void mesen_progress')
 if name=='mmu_plan.h':
  moves={'0x40000u':'0x100000u','0x50000u':'0x118000u','0x60000u':'0x120000u','0x80000u':'0x180000u','0x84000u':'0x1c0000u','0x82000u':'0x182000u','0x82400u':'0x182400u'}
  s=re.sub('|'.join(moves),lambda m:moves[m[0]],s)
 (OUT/name).write_text(s)
subprocess.run(['clang','-O2','-I'+str(OUT),'-DMB_ALLOCATION=0x2000000u',str(ROOT/'tests/amiga/mesen/mmu_plan.c'),'-o',str(OUT/'test-mmu')],check=True)
subprocess.run([str(OUT/'test-mmu')],check=True)
flags=['-Os' ,'-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-fvisibility=hidden','-ffunction-sections','-fdata-sections','-fno-stack-protector','-DZZ_BLOCK_SIZE=0x2000000','-DZZ_CONTROL=0x180000','-DMB_MMU','-DMB_DCACHE','-DMB_ALLOCATION=0x2000000u',
 *['-I'+inside(p) for p in [OUT,OUT/'include',OUT/'src/cpu',SRC,console,shell,pc,OUT/'src/ints',BASE,common,ROOT/'amiga/arm_debug',ROOT/'amiga/arm_debug/zz9000']],'-include',inside(ROOT/'amiga/pocketsnes/libc_config.h')]
sources += [SRC/'adapter.cpp',BASE/'runtime.cpp',console/'bench.cpp' if args.console else SRC/'bench.cpp',SRC/'drive.cpp',ROOT/'amiga/ffmpeg_bench/heap.c',ROOT/'amiga/mesen_snes/system/memory.c',OUT/'worker.cpp',OUT/'mmu.c',OUT/'mmu_asm.S',ROOT/'.context/amiga/ffmpeg-bench/picolibc-1.8.12/libc/machine/arm/setjmp.S',ROOT/'.context/amiga/ffmpeg-bench/picolibc-1.8.12/libc/stdlib/srandom.c',SRC/'platform_time.c']
if args.console:sources.append(console/'transport.cpp')
if args.shell:
 sources=[shell/p.name if p in (SRC/'adapter.cpp',console/'bench.cpp') else p for p in sources if p!=SRC/'drive.cpp']
 sources += [shell/'remote_drive.cpp',shell/'frontend.cpp',shell/'cxx_runtime.cpp',shell/'vendor/gcc_list.cpp']
if args.pc:
 sources=[pc/'adapter.cpp' if p==shell/'adapter.cpp' else OUT/'frontend.cpp' if p==shell/'frontend.cpp' else pc/'bench.cpp' if p==shell/'bench.cpp' else pc/'runtime.cpp' if p==BASE/'runtime.cpp' else pc/'platform_time.c' if p==SRC/'platform_time.c' else p for p in sources]
 sources += [pc/'devices.cpp',pc/'boot.cpp',pc/'video.cpp',pc/'direct.cpp',pc/'profile.cpp']
header_digest=hashlib.sha256(b''.join(p.read_bytes() for p in sorted(OUT.rglob('*.h')) if p.name not in ('zz_payload.h','expected.h'))+b''.join(p.read_bytes() for p in sorted(SRC.glob('*.h')))+b''.join(p.read_bytes() for p in sorted(console.glob('*.h')))+b''.join(p.read_bytes() for p in sorted(shell.glob('*.h')))+b''.join(p.read_bytes() for p in sorted(pc.glob('*.h')) if p.name not in ('display.h','fullscreen.h','present_plan.h'))).digest()
objects=[];overrides={}
for i,src in enumerate(sources):
 obj=OUT/(str(i)+'-'+src.stem+'.arm.o');objects.append(obj)
 cpp=src.suffix=='.cpp'
 extra=['-std=c++17','-fno-exceptions','-fno-rtti','-fno-threadsafe-statics','-fno-use-cxa-atexit'] if cpp else []
 if cpp and args.shell:extra+=['-include',inside(shell/'cxx_config.h')]
 if cpp and ('src/cpu' not in src.as_posix() or (args.pc and src.name=='paging.cpp')):extra+=['-fno-freestanding'] # banked TLB needs malloc declarations; implementation stays our owned heap
 if args.pc and src==OUT/'src/cpu/core_normal.cpp':
  extra+=['-'+args.pc_cpu_opt];overrides[str(src.relative_to(ROOT))]=['-'+args.pc_cpu_opt]
 if args.pc and src.parent==OUT/'src/hardware' and src.stem in ('pic','vga_draw','vga_memory'):
  extra+=['-'+args.pc_hardware_opt];overrides[str(src.relative_to(ROOT))]=['-'+args.pc_hardware_opt]
 digest=hashlib.sha256(src.read_bytes()+header_digest+str(flags+extra).encode()).hexdigest();stamp=obj.with_suffix('.sha256')
 if not stamp.exists() or stamp.read_text()!=digest:
  run(['arm-none-eabi-g++' if cpp else 'arm-none-eabi-gcc',*flags,*extra,'-c',inside(src),'-o',inside(obj)])
  stamp.write_text(digest)
libc=ROOT/'.context/amiga/pocketsnes/libc.a'
assert libc.exists(),'Build PIC libc with scripts/build_pocketsnes_libc.py first'
elf=OUT/'dosbox.elf'
run(['arm-none-eabi-ld','-shared','-Bsymbolic','--gc-sections','--exclude-libs=ALL','--no-undefined','-T',inside(BASE/'payload.ld'),'-Map='+inside(OUT/'link.map'),*map(inside,objects),'--start-group',inside(libc),'/usr/lib/gcc/arm-none-eabi/15/thumb/v7-a/nofp/libgcc.a','--end-group','-o',inside(elf)])
spec=importlib.util.spec_from_file_location('pack',BASE/'elf.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
image,rel,entry=m.unpack(elf.read_bytes())
run(['arm-none-eabi-gcc',*flags,'-c',inside(ROOT/'amiga/arm_debug/zz9000/mapping_probe.S'),'-o',inside(OUT/'mapping.o')])
run(['arm-none-eabi-ld','-shared','-Bsymbolic','--no-undefined','-T',inside(ROOT/'amiga/arm_debug/zz9000/payload.ld'),inside(OUT/'mapping.o'),'-o',inside(OUT/'mapping.elf')])
mapping,mrel,point=unpack_elf((OUT/'mapping.elf').read_bytes());assert not mrel and not point and len(mapping)<=1024
def array(n,data):return 'static const unsigned char '+n+'[]={'+','.join(map(str,data))+'};\n'
(OUT/'zz_payload.h').write_text(f'#define ZZ_ENTRY {entry}u\n'+array('zz_image',image)+array('zz_mapping_probe',mapping)+'static const unsigned long zz_relocations[]={'+','.join(map(str,rel))+'};\n')
(OUT/'host.c').write_text(((pc if args.pc else shell if args.shell else console if args.console else SRC)/'host.c').read_text())
relative=OUT.relative_to(ROOT);binary=OUT/('ZZDosBox' if args.console else 'ZZDosBox-DOS')
launcher=ROOT/'amiga/arm_debug/zz9000/launcher.c'
if args.console:
 # Local staging only: consume application arguments before generic launcher CLI.
 text=launcher.read_text().replace('int main(int argc,char **argv)','extern int db_arguments(int,char **);\nint main(int argc,char **argv)')
 text=text.replace('    struct ConfigDev *gfx=0,*ram=0;', '    argc=db_arguments(argc,argv);if(argc<0)return 20;\n    struct ConfigDev *gfx=0,*ram=0;')
 launcher=OUT/'launcher.c';launcher.write_text(text)
version='0.8.0-dev' if args.pc else '0.4.0-dev' if args.shell else '0.3.0-dev' if args.console else '0.2.0-dev'
subprocess.run([*host_command,'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror',
 *(['-DFF_DIRECT'] if args.pc else []),'-DZZ_HEADLESS_BENCH','-DZZ_RELEASE','-DZZ_BLOCK_SIZE=0x2000000','-DZZ_CONTROL=0x180000','-DMB_ALLOCATION=0x2000000u','-DZZ_APP_NAME="'+binary.name+'"','-DZZ_APP_VERSION="'+version+'"','-DZZ_MIN_STACK=65536','-DMB_MMU','-DMB_DCACHE','-DZZ_HEADLESS_MMU','-DZZ_HEADLESS_ICACHE',
 '-I'+str(relative),'-Iamiga/arm_debug','-Iamiga/arm_debug/zz9000','-Iamiga/mesen_bench','-Iamiga/dosbox/dos','-Iamiga/dosbox/console','-Iamiga/dosbox/shell','-Iamiga/dosbox/pc','-Iamiga/mesen_nes',str(launcher.relative_to(ROOT)),str(relative/'host.c'),*(['amiga/dosbox/console/window.c'] if args.console else []),*(['amiga/dosbox/shell/fs_host.c'] if args.shell else []),*(['amiga/dosbox/pc/display.c'] if args.pc else []),'-lamiga','-o',str(relative/binary.name)],cwd=ROOT,check=True)
assert binary.read_bytes()[:4]==b'\0\0\x03\xf3'
record={'upstream':json.loads((BASE/'upstream.json').read_text()),'scope':('PC BIOS/PIT/keyboard/EGA prototype; no sound; no game-speed claim' if args.pc else 'Upstream DOS shell with live flat C drive; no VGA/audio/PC BIOS' if args.shell else 'Real DOS kernel, external COM/EXE loading and bounded private file drive; no VGA/audio/shell yet'), 'image_bytes':len(image),'relocations':len(rel),'allocation':0x2000000,
 'outputs':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in [elf,binary,OUT/'integration.patch',OUT/'Diagnostic.com',OUT/'dos-integration.patch',OUT/'FileTest.com',OUT/'ExeTest.exe',OUT/'INPUT.TXT',*([OUT/'Echo.com',OUT/'IoTest.com'] if args.console else []),*([OUT/'shell-integration.patch',OUT/'LoadFix.com'] if args.shell else []),*([OUT/'pc-integration.patch',OUT/'EGATEST.COM'] if args.pc else [])]},'libc_sha256':hashlib.sha256(libc.read_bytes()).hexdigest(),
 'console':args.console,'shell':args.shell,'pc':args.pc,'version':version,
 'source_hashes':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [*sources,*SRC.glob('*.*'),*console.glob('*.*'),*shell.glob('*.*'),*pc.glob('*.*'),launcher,ROOT/'scripts/build_dosbox.py',ROOT/'scripts/build_dosbox_dos.py',*([ROOT/'scripts/build_dosbox_shell.py',shell/'vendor/upstream.json'] if args.shell else []),*([ROOT/'scripts/build_dosbox_pc.py',ROOT/'amiga/dosbox/launcher/session.h'] if args.pc else []),Path(__file__),ROOT/'amiga/arm_debug/zz9000/launcher.c',ROOT/'amiga/arm_debug/zz9000/layout.h',ROOT/'amiga/arm_debug/zz9000/mapping_probe.S',OUT/'host.c',OUT/'mmu_plan.h'] if p.is_file()},
 'arm_flags':flags,'arm_overrides':overrides,'compiler':subprocess.check_output([*cmd,'arm-none-eabi-g++','--version'],text=True)}
(OUT/'build.json').write_text(json.dumps(record,indent=2)+'\n')
print('Built',binary,binary.stat().st_size,'bytes; ARM image',len(image),'relocations',len(rel))
