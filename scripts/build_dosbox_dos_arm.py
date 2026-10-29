#!/usr/bin/env python3
"""Build DOSBox DOS kernel and optional console / shell for ZZ9000 Core1. Does not deploy or launch."""
import argparse,hashlib,importlib.util,json,re,subprocess,sys
from pathlib import Path
import build_dosbox_dos as b
from build_zz9000_debug import unpack_elf
from amiga import PIN,container_command
ROOT=b.ROOT;OUT=b.OUT;SRC=b.SRC;BASE=ROOT/"amiga/dosbox"
import os
cmd=json.loads(os.environ.get('DOSBOX_ARM_COMMAND','["podman","--connection","nuflix-converter-root","exec","zzdosbox-toolchain"]'))
def inside(p):return '/work/'+str(Path(p).relative_to(ROOT))
def run(args):subprocess.run([*cmd,*map(str,args)],check=True)
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--container-command');parser.add_argument('--console',action='store_true');parser.add_argument('--shell',action='store_true');parser.add_argument('--pc',action='store_true');parser.add_argument('--pc-cpu-opt',choices=['Os','O2','O3'],help='PC interpreter optimisation level (default: O2 for PC)');parser.add_argument('--pc-hardware-opt',choices=['Os','O2'],help='PIC, VGA drawing and VGA memory optimisation level (default: O2 for PC)');parser.add_argument('--pc-jit-inline',action='store_true',help='Experimental emitted ARM checked-read fast path');parser.add_argument('--pc-jit-reuse',action='store_true',help='Experimental ARM adjacent CPU-register store forwarding');parser.add_argument('--pc-jit-value',action='store_true',help='Experimental checked-read value register ABI for ARMv7 JIT');parser.add_argument('--pc-jit-hot',action='store_true',help='Diagnostic JIT block entry counts during Duke sample; not FPS comparable');parser.add_argument('--pc-duke-clock',action='store_true',help='Read-only Duke 1.3D DNRATE frame observer');parser.add_argument('--pc-descent-clock',action='store_true',help='Isolated read-only Descent shareware 1.4 rendered-frame benchmark');parser.add_argument('--pc-doom-clock',action='store_true',help='Read-only observer for the supplied Doom 1.9 shareware benchmark');parser.add_argument('--pc-dynrec',action='store_true',help='Separate experimental ARMv7 JIT build');parser.add_argument('--pc-jit-mib',type=int,choices=(1,4,32),default=1);parser.add_argument('--pc-neon',action='store_true',help='NEON pixel kernels; all PC builds already preserve SIMD/FPSCR state');parser.add_argument('--pc-pipeline',action='store_true');parser.add_argument('--pc-scanlines',action='store_true');parser.add_argument('--pc-presentation',action='store_true');parser.add_argument('--pc-adapters',action='store_true');parser.add_argument('--pc-opengl',action='store_true');parser.add_argument('--pc-video-trace',action='store_true',help='Separate diagnostic build with live framebuffer phase records; not for benchmarking');parser.add_argument('--pc-pic-vfp',action='store_true',help='Experimental hardware floating-point PIC scheduler; requires NEON context preservation');parser.add_argument('--pc-neon-dispatch',action='store_true',help='Experimental per-row NEON scaler dispatch');parser.add_argument('--pc-pixel-bench',action='store_true',help='Private cached-RAM scaler microbenchmark; diagnostic only');parser.add_argument('--pc-memory-debug',action='store_true',help='Isolated guest RAM monitor/scanner build');parser.add_argument('--pc-matrix-bench',action='store_true',help='Isolated cached-RAM matrix and offscreen TinyGL benchmark');parser.add_argument('--pc-tinygl-bench',action='store_true',help='Internal TinyGL transform A/B and exclusive stage profiling');parser.add_argument('--pc-tinygl-raster',action='store_true',help='Isolated TinyGL NEON spans/clear benchmark');parser.add_argument('--pc-raster-compiler',choices=['baseline','o3','o3-vector'],default='baseline',help='Diagnostic-only optimisation of ztriangle.c, zbuffer.c and raster clear.c; requires --pc-tinygl-raster');parser.add_argument("--pc-raster-fixed",choices=["comparison","scalar","guarded","hybrid","unguarded","dispatch"],default="comparison",help="Diagnostic-only compile-time span selection for standalone comparison");parser.add_argument('--pc-hlslpp',choices=['micro','scalar','neon'],help='Isolated hlsl++ matrix or TinyGL A/B; requires --pc-matrix-bench');parser.add_argument('--pc-disk-cache-kib',type=int,choices=(128,256,512,1024,2048),default=128,help='Bounded write-through image sector cache; default 128 KiB');parser.add_argument('--pc-capture-unroll',action='store_true',help='Experimental indexed-pixel capture and video -O2');parser.add_argument('--pc-network',action='store_true',help='Isolated NE2000/Roadshow NAT build');parser.add_argument('--pc-compat',action='store_true');parser.add_argument('--pc-x87',action='store_true');parser.add_argument('--pc-expanded',action='store_true',help='64 MiB owned allocation, up to 32 MiB guest RAM');parser.add_argument('--pc-file-readahead',action='store_true',help='Experimental 4 KiB read-ahead per read-only DOS file')
parser.add_argument('--pc-row-cache',action='store_true',help='Experimental per-frame reuse of converted vertically repeated rows')
parser.add_argument('--base',type=Path,help='Inherit options from a verified recorded build; explicit flags override')
parser.add_argument('--output',type=Path,help='Isolated build directory within .context')
args=parser.parse_args()
base_record=None
if args.base:
 base_path=args.base.resolve();base_file=base_path/'build.json'
 base_raw=base_file.read_bytes();base_record=json.loads(base_raw)
 if base_record['version']!=re.search(r'#define ZZ_DOSBOX_VERSION "([^"]+)"',(BASE/'version.h').read_text())[1]:parser.error('Base product version differs; select a matching recorded build')
 if 'build_options' not in base_record:parser.error('--base requires a build with recorded options')
 for name,digest in base_record['outputs'].items():
  if hashlib.sha256((base_path/name).read_bytes()).hexdigest()!=digest:parser.error('Base artifact changed: '+name)
 if base_record['upstream']!=json.loads((BASE/'upstream.json').read_text()):parser.error('Base core pin differs')
 supplied={a.split('=')[0] for a in sys.argv[1:] if a.startswith('--')}
 for key,value in base_record['build_options'].items():
  if key not in vars(args) or key in ('base','output','container_command'):parser.error('Invalid base option: '+key)
  if '--'+key.replace('_','-') not in supplied:setattr(args,key,value)
 if not args.output or args.output.resolve()==base_path:parser.error('--base requires a separate --output directory')

if args.pc_row_cache and not args.pc:parser.error('--pc-row-cache requires --pc')
if args.pc_file_readahead and not args.pc:parser.error('--pc-file-readahead requires --pc')
if args.pc_expanded and not (args.pc_compat and args.pc_x87 and args.pc_dynrec and args.pc_jit_mib in (4,32)):parser.error('--pc-expanded requires Compat, x87 and 4/32 MiB JIT')
if args.pc_jit_mib==32 and not args.pc_expanded:parser.error('32 MiB cache requires --pc-expanded')
allocation=0x4000000 if args.pc_expanded else 0x2000000
if args.pc_x87 and not args.pc:parser.error('--pc-x87 requires --pc')
if args.pc_compat and not args.pc:parser.error('--pc-compat requires --pc')
if args.pc_network and not args.pc:parser.error('--pc-network requires --pc')
if args.pc_expanded and args.pc_network:parser.error('expanded COM1 and NE2000/IPX use separate runtime socket owners')
if args.pc_network and args.pc_memory_debug:parser.error('network and memory monitor currently share a control region; separate builds required')
if (args.pc_neon or args.pc_pipeline or args.pc_scanlines or args.pc_presentation or args.pc_jit_mib!=1) and not args.pc_dynrec:parser.error('tuning options require --pc-dynrec')
if args.pc_adapters and (not args.pc_dynrec or args.pc_jit_mib not in (4,32)):parser.error('--pc-adapters requires --pc-dynrec and 4/32 MiB JIT')
if args.pc_opengl and not (args.pc_adapters and args.pc_neon):parser.error('--pc-opengl requires --pc-adapters --pc-neon')
if (args.pc_jit_value or args.pc_jit_reuse or args.pc_jit_inline) and not args.pc_dynrec:parser.error('JIT experiments require dynrec')
if args.pc_jit_inline and args.pc_jit_value:parser.error('Choose either value ABI or inline reads for isolated comparison')
if args.pc_jit_hot and not (args.pc_duke_clock and args.pc_dynrec):parser.error('JIT hot counters require Duke clock and dynrec')
if (args.pc_descent_clock or args.pc_duke_clock) and (not args.pc_expanded or args.pc_doom_clock or args.pc_video_trace or args.pc_pixel_bench or args.pc_matrix_bench or (args.pc_descent_clock and args.pc_duke_clock)):parser.error('game clocks require expanded and exclusive diagnostic report space')
if args.pc_doom_clock and not args.pc:parser.error('--pc-doom-clock requires --pc')
if args.pc_video_trace and not args.pc_dynrec:parser.error('--pc-video-trace requires --pc-dynrec')
if args.pc_memory_debug and (not args.pc_dynrec or args.pc_video_trace or args.pc_pixel_bench or args.pc_opengl):parser.error('--pc-memory-debug requires dynrec and excludes trace/pixel-bench/OpenGL')
if args.pc_neon_dispatch and not args.pc_neon:parser.error('--pc-neon-dispatch requires --pc-neon')
if args.pc_pixel_bench and (not args.pc_neon_dispatch or args.pc_video_trace or args.pc_opengl):parser.error('--pc-pixel-bench requires dispatch and excludes video trace/OpenGL')
if args.pc_raster_fixed!='comparison' and not args.pc_tinygl_raster:parser.error('--pc-raster-fixed requires --pc-tinygl-raster')
if args.pc_raster_compiler!='baseline' and not args.pc_tinygl_raster:parser.error('--pc-raster-compiler requires --pc-tinygl-raster')
if args.pc_hlslpp and (not args.pc_matrix_bench or args.pc_tinygl_raster or args.pc_tinygl_bench!=(args.pc_hlslpp!='micro')):parser.error('hlslpp micro requires matrix only; scalar/neon require internal TinyGL, no raster')
if args.pc_tinygl_raster and not args.pc_tinygl_bench:parser.error('--pc-tinygl-raster requires --pc-tinygl-bench')
if args.pc_tinygl_bench and not args.pc_matrix_bench:parser.error('--pc-tinygl-bench requires --pc-matrix-bench')
if args.pc_matrix_bench and (not args.pc or not args.pc_pic_vfp or args.pc_pixel_bench or args.pc_memory_debug or args.pc_opengl or args.pc_video_trace):parser.error('--pc-matrix-bench requires PC/PIC-VFP and excludes other diagnostic/OpenGL builds')
if args.pc_pic_vfp and not args.pc_neon:parser.error('--pc-pic-vfp requires --pc-neon')
if (args.pc_disk_cache_kib!=128 or args.pc_capture_unroll) and not args.pc:parser.error('PC cache/capture options require --pc')
if args.pc_dynrec and not args.pc:parser.error('--pc-dynrec requires --pc')
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
  if args.pc_dynrec:pb.OUT=ROOT/('.context/amiga/dosbox-dynrec'+('-'+str(args.pc_jit_mib)+'m' if args.pc_jit_mib!=1 else '')+('-neon' if args.pc_neon else '')+('-pipe' if args.pc_pipeline else '')+('-scan' if args.pc_scanlines else '')+('-present' if args.pc_presentation else '')+('-adapters' if args.pc_adapters else '')+('-gl' if args.pc_opengl else '')+('-trace' if args.pc_video_trace else '')+('-pic-vfp' if args.pc_pic_vfp else '')+('-dispatch' if args.pc_neon_dispatch else '')+('-pixel-bench' if args.pc_pixel_bench else '')+('-memory' if args.pc_memory_debug else '')+('-matrix-bench' if args.pc_matrix_bench else '')+('-tinygl-internal' if args.pc_tinygl_bench else '')+('-raster' if args.pc_tinygl_raster else '')+('-hlslpp-'+args.pc_hlslpp if args.pc_hlslpp else ''))
  if args.pc_disk_cache_kib!=128:pb.OUT=Path(str(pb.OUT)+'-cache'+str(args.pc_disk_cache_kib))
  if args.pc_capture_unroll:pb.OUT=Path(str(pb.OUT)+'-capture')
  if args.pc_network:pb.OUT=Path(str(pb.OUT)+'-network')
  if args.pc_compat:pb.OUT=Path(str(pb.OUT)+'-compat')
  if args.pc_x87:pb.OUT=Path(str(pb.OUT)+'-x87')
  if args.pc_expanded:pb.OUT=Path(str(pb.OUT)+'-expanded')
  if args.pc_row_cache:pb.OUT=Path(str(pb.OUT)+'-rows')
  if args.pc_file_readahead:pb.OUT=Path(str(pb.OUT)+'-read-ahead')
  if args.pc_descent_clock:pb.OUT=Path(str(pb.OUT)+'-descent-clock')
  if args.pc_duke_clock:pb.OUT=Path(str(pb.OUT)+'-duke-clock')
  if args.pc_jit_hot:pb.OUT=Path(str(pb.OUT)+'-jit-hot')
  if args.pc_jit_value:pb.OUT=Path(str(pb.OUT)+'-jit-value')
  if args.pc_jit_reuse:pb.OUT=Path(str(pb.OUT)+'-jit-reuse')
  if args.pc_jit_inline:pb.OUT=Path(str(pb.OUT)+'-jit-inline')
  if args.output:
   pb.OUT=args.output.resolve()
   if not pb.OUT.is_relative_to(ROOT/'.context'):parser.error('--output must be within workspace .context')
  OUT=pb.OUT;b.OUT=OUT;sources=pb.prepare()
  if args.pc_x87:
   from prepare_dosbox_x87 import prepare as x87_prepare
   sources+=x87_prepare(OUT)
  if args.pc_compat:
   from prepare_dosbox_compat import prepare as memory_prepare
   from prepare_dosbox_audio import prepare as audio_prepare
   sources+=memory_prepare(OUT)+audio_prepare(OUT)+[pc/'compat.cpp',pc/'audio.cpp']
  if args.pc_expanded:
   from prepare_dosbox_expanded import prepare as expanded_prepare
   sources+=expanded_prepare(OUT)+[pc/'serial.cpp']
  if args.pc_dynrec:
   from prepare_dosbox_dynrec import prepare
   sources+=prepare(OUT)
 else:OUT=sb.OUT;b.OUT=OUT;sources=sb.prepare()
else:sources=b.prepare()
if args.console:b.console_fixtures()
common=ROOT/'amiga/common/arm'
for name in ['worker.cpp','mmu.c','mmu.h','mmu_asm.S','mmu_plan.h']:
 s=(common/name).read_text()
 if name=='worker.cpp':
  s=s.replace('extern "C" void mesen_progress', 'extern "C" volatile uint8_t *db_shared(){return shared;}\nextern "C" void db_output(const char *text) { for(unsigned i=0;i<160;i++){shared[MB_REPLY+128+i]=(uint8_t)text[i];if(!text[i])break;} barrier(); }\nextern "C" void mesen_progress')
 if name=='mmu_plan.h':
  moves={'0x40000u':'0x100000u','0x50000u':'0x118000u','0x60000u':'0x120000u','0x80000u':'0x180000u','0x84000u':'0x1c0000u','0x82000u':'0x182000u','0x82400u':'0x182400u'}
  if args.pc_expanded:moves.update({'0x40000u':'0x140000u','0x50000u':'0x160000u','0x60000u':'0x168000u'})
  s=re.sub('|'.join(moves),lambda m:moves[m[0]],s)
  if args.pc_dynrec:
   s=s.replace('#include <stdint.h>', '#include <stdint.h>\n#include \"dynrec_plan.h\"')
   s=s.replace('if(page>=base+MB_TABLE_BEGIN)attr|=1u;', 'if(page>=base+MB_TABLE_BEGIN && !db_jit_page(base,page))attr|=1u;')
 (OUT/name).write_text(s)
subprocess.run(['clang','-O2','-I'+str(OUT),'-I'+str(pc),*(['-DDB_DYNREC','-DDB_JIT_MIB='+str(args.pc_jit_mib)] if args.pc_dynrec else []),'-DMB_ALLOCATION='+hex(allocation),*(['-DDB_EXPANDED'] if args.pc_expanded else []),str(ROOT/'tests/amiga/common/mmu_plan.c'),'-o',str(OUT/'test-mmu')],check=True)
subprocess.run([str(OUT/'test-mmu')],check=True)
flags=['-Os' ,'-mcpu=cortex-a9','-marm','-mfloat-abi=soft','-mno-unaligned-access','-ffreestanding','-fno-builtin','-fPIC','-fvisibility=hidden','-ffunction-sections','-fdata-sections','-fno-stack-protector','-DZZ_BLOCK_SIZE='+hex(allocation),'-DZZ_CONTROL=0x180000','-DMB_MMU','-DMB_DCACHE','-DMB_ALLOCATION='+hex(allocation),
 *['-I'+inside(p) for p in [OUT,OUT/'include',OUT/'src/cpu',SRC,console,shell,pc,OUT/'src/ints',BASE,common,ROOT/'amiga/arm_debug',ROOT/'amiga/arm_debug/zz9000']],'-include',inside(ROOT/'amiga/common/libc/libc_config.h')]
if args.pc_file_readahead:flags+=['-DDB_FILE_READAHEAD=1']
if args.pc_expanded:flags+=['-DDB_EXPANDED']
if args.pc_compat:flags+=['-DDB_COMPAT']
if args.pc_dynrec:flags+=['-DDB_DYNREC','-DDB_JIT_MIB='+str(args.pc_jit_mib)]
if args.pc_video_trace:flags+=['-DDB_VIDEO_TRACE']
if args.pc:flags+=['-DDB_SIMD_CONTEXT']
if args.pc_neon:flags+=['-DDB_NEON']
if args.pc_row_cache:flags+=['-DDB_ROW_CACHE']
if args.pc_memory_debug:flags+=['-DDB_MEMORY_DEBUG','-I/work/amiga/dosbox/developer']
if args.pc_pic_vfp:flags+=['-DDB_PIC_VFP']
if args.pc_neon_dispatch:flags+=['-DDB_NEON_DISPATCH']
if args.pc_pixel_bench:flags+=['-DDB_PIXEL_BENCH'];sources.append(pc/'pixel_bench.cpp')
if args.pc_scanlines:flags+=['-DDB_SCANLINE_TEST']
if args.pc_presentation:flags+=['-DDB_PRESENT_TEST']
sources += [SRC/'adapter.cpp',BASE/'runtime.cpp',console/'bench.cpp' if args.console else SRC/'bench.cpp',SRC/'drive.cpp',ROOT/'amiga/common/arm/heap.c',ROOT/'amiga/common/arm/memory.c',OUT/'worker.cpp',OUT/'mmu.c',OUT/'mmu_asm.S',ROOT/'.context/amiga/dosbox-toolchain/picolibc-1.8.12/libc/machine/arm/setjmp.S',ROOT/'.context/amiga/dosbox-toolchain/picolibc-1.8.12/libc/stdlib/srandom.c',SRC/'platform_time.c']
if args.console:sources.append(console/'transport.cpp')
if args.shell:
 sources=[shell/p.name if p in (SRC/'adapter.cpp',console/'bench.cpp') else p for p in sources if p!=SRC/'drive.cpp']
 sources += [shell/'remote_drive.cpp',shell/'frontend.cpp',shell/'cxx_runtime.cpp',shell/'vendor/gcc_list.cpp']
if args.pc:
 sources=[pc/'adapter.cpp' if p==shell/'adapter.cpp' else OUT/'frontend.cpp' if p==shell/'frontend.cpp' else pc/'bench.cpp' if p==shell/'bench.cpp' else pc/'runtime.cpp' if p==BASE/'runtime.cpp' else pc/'platform_time.c' if p==SRC/'platform_time.c' else p for p in sources]
 sources += [pc/'devices.cpp',pc/'boot.cpp',pc/'video.cpp',pc/'direct.cpp',pc/'profile.cpp',pc/'cd.cpp',pc/'midi.cpp']
if args.pc_network:
 from prepare_dosbox_network import prepare_ne2000
 from prepare_dosbox_ipx import prepare as prepare_ipx
 sources += [prepare_ne2000(OUT),BASE/'network/arm.cpp',*prepare_ipx(OUT)]
 flags += ['-DDB_NETWORK','-I'+inside(BASE/'network')]
if args.pc_jit_inline:
 from prepare_dosbox_jit_inline import prepare as prepare_jit_inline
 prepare_jit_inline(OUT)
if args.pc_jit_reuse:
 from prepare_dosbox_jit_reuse import prepare as prepare_jit_reuse
 prepare_jit_reuse(OUT)
 flags+=['-DDB_JIT_REUSE']
if args.pc_jit_value:
 from prepare_dosbox_jit_value import prepare as prepare_jit_value
 prepare_jit_value(OUT)
if args.pc_dynrec:sources.append(pc/'dynrec.cpp')
if args.pc_jit_hot:
 sources.append(pc/'jit_hot.cpp')
 p=OUT/'src/cpu/core_dynrec/decoder.h';s=p.read_text();marker='\tdecode.cycles=0;'
 assert marker in s
 s=s.replace(marker,'\t{extern unsigned *db_jit_hot_counter(unsigned);gen_add_direct_word(db_jit_hot_counter(start),1,true); }\n'+marker)
 p.write_text(s)

if args.pc:sources += [pc/'neon_context.cpp',pc/'neon_state.S']
if args.pc_neon:sources.append(pc/'pixels_neon.cpp')
if args.pc_doom_clock:
 text=(pc/'bench.cpp').read_text()
 text=text.replace('#include "dosbox.h"','#include "dosbox.h"\n#include "doom_clock.h"\nstatic DoomClock doom_clock;')
 text=text.replace('void db_pc_tick(){','void db_pc_tick(){\n doom_clock.tick(MemBase,db_memory_mb,PIC_Ticks,pc_clock_ticks());')
 text=text.replace(' pc_prof_finish();',' pc_prof_finish();\n doom_clock.publish(b);')
 bench=OUT/'bench-doom.cpp';bench.write_text(text)
 sources=[bench if src==pc/'bench.cpp' else src for src in sources]
if args.pc_descent_clock or args.pc_duke_clock:
 clock_name='duke' if args.pc_duke_clock else 'descent'
 text=(pc/'bench.cpp').read_text()
 text=text.replace('#include "dosbox.h"','#include "dosbox.h"\n#include "descent_clock.h"\nstatic DescentClock descent_clock;\nstatic uint64_t descent_prof_first[6],descent_prof_last[6];\nextern void pc_prof_snapshot(uint64_t *);')
 text=text.replace('void db_pc_tick(){','void db_pc_tick(){\n bool started=descent_clock.start!=0,ended=descent_clock.end!=0;\n descent_clock.tick(MemBase,db_memory_mb*1048576u,PIC_Ticks,pc_clock_ticks());\n if(!started&&descent_clock.start)pc_prof_snapshot(descent_prof_first);\n if(!ended&&descent_clock.end)pc_prof_snapshot(descent_prof_last);\n if(descent_clock.end)ad_put(db_shared(),PC_DONE,1);')
 text=text.replace(' pc_prof_finish();',' pc_prof_finish();\n {static_assert(PC_VIDEO+0xe00+96<=PC_BOOT_TEXT,"Descent report bounds");const unsigned o=PC_VIDEO+0xe00;uint32_t words[12];descent_clock.report(words);for(unsigned i=0;i<12;i++)ad_put(b,o+i*4,words[i]);for(unsigned i=0;i<6;i++){uint64_t delta=descent_prof_last[i]-descent_prof_first[i];ad_put(b,o+48+i*8,delta>>32);ad_put(b,o+52+i*8,delta);}}')
 if args.pc_duke_clock:text=text.replace('descent_clock','duke_clock').replace('DescentClock','DukeClock')
 if args.pc_jit_hot:
  text='#include "jit_hot.h"\n'+text
  assert text.count('pc_prof_snapshot(descent_prof_first);')==1
  text=text.replace('pc_prof_snapshot(descent_prof_first);','{pc_prof_snapshot(descent_prof_first);db_jit_hot_reset();}')
  text=text.replace(' pc_prof_finish();',' db_jit_hot_report();\n pc_prof_finish();')
 bench=OUT/('bench-'+clock_name+'.cpp');bench.write_text(text)
 sources=[bench if src==pc/'bench.cpp' else src for src in sources]
if args.pc_opengl:
 from prepare_dosbox_opengl import prepare as prepare_gl
 sources += prepare_gl(OUT)+[BASE/'opengl/renderer.c',BASE/'opengl/messages.c']
 flags += ['-DDB_OPENGL','-I'+inside(OUT/'tinygl/include'),'-I'+inside(BASE/'opengl')]
if args.pc_matrix_bench:
 from prepare_dosbox_matrix import prepare as prepare_matrix, prepare_scene
 if args.pc_hlslpp:
  from prepare_dosbox_hlslpp import prepare as prepare_hlslpp
  sources += prepare_hlslpp(OUT,args.pc_hlslpp)
  flags += ['-I'+inside(OUT/'hlslpp/include')]
  if args.pc_tinygl_bench:flags += ['-DDB_TINYGL_BENCH','-I'+inside(BASE/'tinygl_bench')]
 elif args.pc_tinygl_bench:
  from prepare_dosbox_tinygl_bench import prepare as prepare_tgl
  if args.pc_tinygl_raster:
   from prepare_dosbox_tinygl_raster import prepare as prepare_raster
   sources += prepare_raster(OUT,args.pc_raster_fixed)+[BASE/'opengl/messages.c',BASE/'matrix/neon.c',BASE/'matrix/scalar.c',BASE/'tinygl_bench/profile.c']+[p for p in sorted((BASE/'tinygl_raster').glob('*.c')) if p.name!='scene.c']+sorted((BASE/'tinygl_raster').glob('*.cpp'))
   flags += ['-DDB_TINYGL_RASTER','-I'+inside(BASE/'tinygl_raster')]
  else:sources += prepare_tgl(OUT)+[BASE/'opengl/messages.c',BASE/'matrix/neon.c',BASE/'matrix/scalar.c']+sorted((BASE/'tinygl_bench').glob('*.c'))+sorted((BASE/'tinygl_bench').glob('*.cpp'))
  flags += ['-DDB_TINYGL_BENCH','-I'+inside(BASE/'tinygl_bench')]
 else:sources += prepare_matrix(OUT)+prepare_scene(OUT)+[BASE/'opengl/messages.c']+sorted((BASE/'matrix').glob('*.c'))+sorted((BASE/'matrix').glob('*.cpp'))
 flags += ['-DDB_MATRIX_BENCH','-I'+inside(BASE/'matrix'),'-I'+inside(OUT/'tinygl/include'),'-I'+inside(OUT/'cmsis-dsp/Include'),'-I'+inside(OUT/'cmsis-dsp/PrivateInclude')]
header_digest=hashlib.sha256(b''.join(p.read_bytes() for p in sorted((BASE/'network').glob('*.h')) if args.pc_network)+b''.join(p.read_bytes() for p in sorted((BASE/'tinygl_raster').glob('*.h')) if args.pc_tinygl_raster)+b''.join(p.read_bytes() for p in sorted((BASE/'tinygl_bench').glob('*.h')) if args.pc_tinygl_bench)+b''.join(p.read_bytes() for p in sorted(OUT.rglob('*.h')) if p.name not in ('zz_payload.h','expected.h'))+b''.join(p.read_bytes() for p in sorted(SRC.glob('*.h')))+b''.join(p.read_bytes() for p in sorted((BASE/'developer').glob('*.h')) if args.pc_memory_debug)+b''.join(p.read_bytes() for p in sorted((BASE/'matrix').glob('*.h')) if args.pc_matrix_bench)+b''.join(p.read_bytes() for p in sorted(console.glob('*.h')))+b''.join(p.read_bytes() for p in sorted(shell.glob('*.h')))+b''.join(p.read_bytes() for p in sorted(pc.glob('*.h')) if p.name not in ('display.h','fullscreen.h','windowed.h','window_geometry.h','present_plan.h','video_trace_host.h','video_debug_host.h'))).digest()
objects=[];overrides={}
for i,src in enumerate(sources):
 obj=OUT/(str(i)+'-'+src.stem+'.arm.o');objects.append(obj)
 cpp=src.suffix=='.cpp'
 extra=['-std=c++17','-fno-exceptions','-fno-rtti','-fno-threadsafe-statics','-fno-use-cxa-atexit'] if cpp else []
 if cpp and args.shell:extra+=['-include',inside(shell/'cxx_config.h')]
 if cpp and ('src/cpu' not in src.as_posix() or (args.pc and src.name in ('paging.cpp','core_dynrec.cpp'))):extra+=['-fno-freestanding'] # banked TLB needs malloc declarations; implementation stays our owned heap
 if args.pc and src==shell/'remote_drive.cpp':
  extra+=['-DDB_DISK_CACHE_SETS='+str(args.pc_disk_cache_kib//16)];overrides[str(src.relative_to(ROOT))]=extra.copy()
 if args.pc_capture_unroll and src==pc/'video.cpp':
  extra+=['-DDB_CAPTURE_UNROLL','-O2'];overrides[str(src.relative_to(ROOT))]=extra.copy()
 if args.pc and src in (OUT/'src/cpu/core_normal.cpp',OUT/'src/cpu/core_dynrec.cpp'):
  extra+=['-'+args.pc_cpu_opt];overrides[str(src.relative_to(ROOT))]=['-'+args.pc_cpu_opt]
 if args.pc and src.parent==OUT/'src/hardware' and src.stem in ('pic','vga_draw','vga_memory'):
  extra+=['-'+args.pc_hardware_opt];overrides[str(src.relative_to(ROOT))]=['-'+args.pc_hardware_opt]
 if args.pc_compat and src.parent==OUT/'src/hardware' and src.stem in ('mixer','sblaster','adlib','dbopl','pcspeaker'):
  extra+=['-O2','-mfloat-abi=softfp','-mfpu=vfpv3','-ffp-contract=off'];overrides[str(src.relative_to(ROOT))]=extra.copy()
 if args.pc_x87 and src in (OUT/'src/fpu/fpu.cpp',OUT/'src/cpu/core_dynrec.cpp'):
  extra+=['-O2','-mfloat-abi=softfp','-mfpu=vfpv3','-ffp-contract=off','-fno-strict-aliasing'];overrides[str(src.relative_to(ROOT))]=extra.copy()
 if args.pc_pic_vfp and src==OUT/'src/hardware/pic.cpp':
  extra+=['-mfloat-abi=softfp','-mfpu=vfpv3','-ffp-contract=off'];overrides[str(src.relative_to(ROOT))]=['-'+args.pc_hardware_opt,'-mfloat-abi=softfp','-mfpu=vfpv3','-ffp-contract=off']
 if (args.pc_opengl or args.pc_matrix_bench) and (src.parent==OUT/'tinygl/src' or src.parent==BASE/'opengl' or src==OUT/'raster-scene.c'):
  extra+=['-std=c99','-O2','-mfloat-abi=softfp','-mfpu=neon','-DNO_DEBUG_OUTPUT']
  if args.pc_tinygl_bench:extra+=['-ffp-contract=off','-fno-tree-vectorize']
 if src.name=='pixels_neon.cpp':
  extra+=['-O2','-mfloat-abi=softfp','-mfpu=neon'];overrides[str(src.relative_to(ROOT))]=['-O2','-mfloat-abi=softfp','-mfpu=neon']
 if args.pc_matrix_bench and (src.parent==BASE/'matrix' or src.parent==BASE/'tinygl_bench' or src.parent==BASE/'tinygl_raster' or src.parent==BASE/'hlslpp' or src==OUT/'hlsl-bench.cpp' or src.parent==OUT/'cmsis-dsp/Source/MatrixFunctions'):
  extra+=['-O2','-mfloat-abi=softfp','-mfpu=vfpv3' if src.name=='scalar.c' else '-mfpu=neon','-ffp-contract=off','-fno-tree-vectorize','-DARM_MATH_NEON','-DARM_MATH_MATRIX_CHECK','-D__GNUC_PYTHON__','-DARM_MATH_L1_CACHE_SIZE=16384','-DARM_MATH_L2_CACHE_SIZE=32768','-DARM_MATH_L3_CACHE_SIZE=32768']
  overrides[str(src.relative_to(ROOT))]=extra
 if args.pc_tinygl_raster and (src.parent==OUT/'tinygl/src' or src.parent==BASE/'tinygl_raster' or src==OUT/'raster-scene.c'):extra+=['-fno-strict-aliasing']
 if args.pc_tinygl_raster and src in (OUT/'tinygl/src/ztriangle.c',OUT/'tinygl/src/zbuffer.c',BASE/'tinygl_raster/clear.c'):
  # Restrict compiler experiments to integer raster/clear hot routines. Keep ABI,
  # transforms, scheduler, benchmark and floating-point semantics unchanged.
  if args.pc_raster_compiler!='baseline':
   extra+=['-O3']
   if args.pc_raster_compiler=='o3-vector':extra+=['-ftree-loop-vectorize','-ftree-slp-vectorize','-fvect-cost-model=dynamic']
   else:extra+=['-fno-tree-vectorize']
   report=OUT/(src.stem+'-vectorization.txt')
   extra+=['-fopt-info-vec-all='+inside(report)]
  overrides[str(src.relative_to(ROOT))]=extra.copy()
 private_gl_headers=(BASE/'opengl/renderer.h').read_bytes() if args.pc_opengl and (src.parent==BASE/'opengl' or src==pc/'direct.cpp') else b''
 private_matrix_sources=b''.join(p.read_bytes() for p in sorted(src.parent.glob('*.c'))) if args.pc_matrix_bench and src.parent==OUT/'cmsis-dsp/Source/MatrixFunctions' else b''
 digest=hashlib.sha256(src.read_bytes()+header_digest+private_gl_headers+private_matrix_sources+str(flags+extra).encode()).hexdigest();stamp=obj.with_suffix('.sha256')
 if not stamp.exists() or stamp.read_text()!=digest:
  run(['arm-none-eabi-g++' if cpp else 'arm-none-eabi-gcc',*flags,*extra,'-c',inside(src),'-o',inside(obj)])
  stamp.write_text(digest)
libc=ROOT/'.context/amiga/dosbox-libc/libc.a'
assert libc.exists(),'Build PIC libc with scripts/build_dosbox_libc.py first'
elf=OUT/'dosbox.elf'
linker=BASE/'payload.ld'
if args.pc_dynrec:
 linker=OUT/'payload.ld';linker.write_text((BASE/'payload.ld').read_text().replace('0x1fd0000',hex(allocation-0x100000-args.pc_jit_mib*0x100000)))
if args.pc_expanded:linker.write_text(linker.read_text().replace('0x100000,', '0x140000,'))
if args.pc_matrix_bench:linker.write_text(linker.read_text().replace('*(.bss*) *(COMMON)', '*(.bss*) *(.noinit*) *(COMMON)'))
run(['arm-none-eabi-ld','-shared','-Bsymbolic','--gc-sections','--exclude-libs=ALL','--no-undefined','-T',inside(linker),'-Map='+inside(OUT/'link.map'),*map(inside,objects),'--start-group',inside(libc),'/usr/lib/gcc/arm-none-eabi/15/thumb/v7-a/nofp/libgcc.a','--end-group','-o',inside(elf)])
spec=importlib.util.spec_from_file_location('pack',BASE/'elf.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
image,rel,entry=m.unpack(elf.read_bytes(),expanded=args.pc_expanded,jit_mib=args.pc_jit_mib if args.pc_dynrec else 0)
run(['arm-none-eabi-gcc',*flags,'-c',inside(ROOT/'amiga/arm_debug/zz9000/mapping_probe.S'),'-o',inside(OUT/'mapping.o')])
run(['arm-none-eabi-ld','-shared','-Bsymbolic','--no-undefined','-T',inside(ROOT/'amiga/arm_debug/zz9000/payload.ld'),inside(OUT/'mapping.o'),'-o',inside(OUT/'mapping.elf')])
mapping,mrel,point=unpack_elf((OUT/'mapping.elf').read_bytes());assert not mrel and not point and len(mapping)<=1024
def array(n,data):return 'static const unsigned char '+n+'[]={'+','.join(map(str,data))+'};\n'
(OUT/'zz_payload.h').write_text(f'#define ZZ_ENTRY {entry}u\n'+array('zz_image',image)+array('zz_mapping_probe',mapping)+'static const unsigned long zz_relocations[]={'+','.join(map(str,rel))+'};\n')
(OUT/'host.c').write_text(((pc if args.pc else shell if args.shell else console if args.console else SRC)/'host.c').read_text())
if args.pc_dynrec:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xd40;syncio->pull(shared+o,28,NULL);printf("JIT magic=%08x commits=%u witness=%u dispatches=%u cache_bytes=%u heap_peak=%u paged_fallbacks=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24));}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xd60;syncio->pull(shared+o,28,NULL);printf("NEON magic=%08x witness=%u context_equal=%u cpacr=%08x fpexc=%08x mvfr0=%08x mvfr1=%08x\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24));ok=ok&&ad_get(shared,o)==0x4e454f31&&ad_get(shared,o+4)==42&&ad_get(shared,o+8)==1;}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_matrix_bench and not args.pc_tinygl_bench:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xe00;syncio->pull(shared+o,256,NULL);printf("MATRIXBENCH magic=%08x errors=%u count=%u quick=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12));ok=ok&&ad_get(shared,o)==0x4d415431&&ad_get(shared,o+4)==0&&ad_get(shared,o+8)==6&&ad_get(shared,o+12)==0;for(unsigned i=0;i<6;i++){unsigned r=o+16+i*40;printf("MATRIXBENCH case=%u n=%u repeats=%u hash=%08x scalar=%08x%08x cmsis=%08x%08x neon=%08x%08x\\n",ad_get(shared,r),ad_get(shared,r+4),ad_get(shared,r+8),ad_get(shared,r+12),ad_get(shared,r+16),ad_get(shared,r+20),ad_get(shared,r+24),ad_get(shared,r+28),ad_get(shared,r+32),ad_get(shared,r+36));}}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_tinygl_bench and not args.pc_tinygl_raster:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xe00;syncio->pull(shared+o,400,NULL);printf("TGLBENCH magic=%08x errors=%u count=%u quick=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12));ok=ok&&ad_get(shared,o)==0x54474c31&&ad_get(shared,o+4)==0&&ad_get(shared,o+8)==3&&ad_get(shared,o+12)==0;for(unsigned i=0;i<3;i++){unsigned r=o+16+i*128;printf("TGLBENCH case=%u n=%u repeats=%u hash=%08x scalar=%08x%08x neon=%08x%08x\\n",ad_get(shared,r),ad_get(shared,r+4),ad_get(shared,r+8),ad_get(shared,r+12),ad_get(shared,r+16),ad_get(shared,r+20),ad_get(shared,r+24),ad_get(shared,r+28));for(unsigned m=0;m<2;m++){printf("TGLPROFILE case=%u method=%u",i,m);for(unsigned j=0;j<6;j++)printf(" t%u=%08x%08x",j,ad_get(shared,r+32+m*48+j*8),ad_get(shared,r+36+m*48+j*8));printf("\\n");}}}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_tinygl_raster:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xe00;syncio->pull(shared+o,400,NULL);printf("RASTERBENCH magic=%08x errors=%u count=%u quick=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12));ok=ok&&ad_get(shared,o)==0x52535433&&ad_get(shared,o+4)==0&&ad_get(shared,o+8)==8&&ad_get(shared,o+12)==0;for(unsigned i=0;i<8;i++){unsigned r=o+16+i*48;printf("RASTERBENCH case=%u n=%u repeats=%u hash=%08x",ad_get(shared,r),ad_get(shared,r+4),ad_get(shared,r+8),ad_get(shared,r+12));for(unsigned m=0;m<4;m++)printf(" t%u=%08x%08x",m,ad_get(shared,r+16+m*8),ad_get(shared,r+20+m*8));printf("\\n");}}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_hlslpp:
 text=(OUT/'host.c').read_text()
 if args.pc_hlslpp=='micro':text=text.replace(' cmsis=%08x%08x',' hlsl=%08x%08x')
 else:text=text.replace(' scalar=%08x%08x neon=%08x%08x',' '+args.pc_hlslpp+'=%08x%08x hlsl=%08x%08x')
 marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 assert marker in text
 text=text.replace(marker,'   printf("HLSLPP mode='+args.pc_hlslpp+' revision=3d9af2e77a89ddd645928f1600a0688dc898dedc\\n");\n'+marker)
 (OUT/'host.c').write_text(text)
if args.pc_pixel_bench:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xe00;syncio->pull(shared+o,272,NULL);printf("PIXELBENCH magic=%08x errors=%u count=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8));ok=ok&&ad_get(shared,o)==0x50495831&&ad_get(shared,o+4)==0&&ad_get(shared,o+8)==8;for(unsigned i=0;i<8;i++){unsigned r=o+16+i*32;printf("PIXELBENCH width=%u target=%u format=%u rows=%u baseline=%08x%08x candidate=%08x%08x\\n",ad_get(shared,r),ad_get(shared,r+4),ad_get(shared,r+8),ad_get(shared,r+12),ad_get(shared,r+16),ad_get(shared,r+20),ad_get(shared,r+24),ad_get(shared,r+28));}}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_scanlines:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xd80;syncio->pull(shared+o,24,NULL);printf("SCANLINES magic=%08x frames=%u rendered_rows=%u cleared_rows=%u skipped_rows=%u nonce_repairs=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20));}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_doom_clock:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xd00;syncio->pull(shared+o,44,NULL);printf("DOOMCLOCK magic=%08x signature=%u rejected=%u first=%u last=%u sim_first=%u sim_last=%u start=%08x%08x end=%08x%08x\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24),ad_get(shared,o+28),ad_get(shared,o+32),ad_get(shared,o+36),ad_get(shared,o+40));}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
if args.pc_descent_clock or args.pc_duke_clock:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xe00;syncio->pull(shared+o,96,NULL);printf("DESCENTCLOCK magic=%08x address=%08x rejected=%u first=%u last=%u sim_first=%u sim_last=%u start=%08x%08x end=%08x%08x samples=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+16),ad_get(shared,o+20),ad_get(shared,o+24),ad_get(shared,o+28),ad_get(shared,o+32),ad_get(shared,o+36),ad_get(shared,o+40),ad_get(shared,o+44));for(unsigned i=0;i<6;i++)printf("DESCENTPROFILE bucket=%u ticks=%08x%08x\\n",i,ad_get(shared,o+48+i*8),ad_get(shared,o+52+i*8));}\n'
 if args.pc_duke_clock:addition=addition.replace('DESCENTCLOCK','DUKECLOCK').replace('DESCENTPROFILE','DUKEPROFILE')
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
relative=OUT.relative_to(ROOT);binary=OUT/('ZZDosBox-Net-JIT' if args.pc_network and args.pc_compat and args.pc_dynrec else 'ZZDosBox-Expanded' if args.pc_expanded else 'ZZDosBox-Compat' if args.pc_compat else 'ZZDosBox-Dynrec' if args.pc_dynrec else 'ZZDosBox' if args.console else 'ZZDosBox-DOS')
launcher=ROOT/'amiga/arm_debug/zz9000/launcher.c'
if args.console:
 # Local staging only: consume application arguments before generic launcher CLI.
 text=launcher.read_text().replace('int main(int argc,char **argv)','extern int db_arguments(int,char **);\nint main(int argc,char **argv)')
 text=text.replace('    struct ConfigDev *gfx=0,*ram=0;', '    argc=db_arguments(argc,argv);if(argc<0)return 20;\n    struct ConfigDev *gfx=0,*ram=0;')
 launcher=OUT/'launcher.c';launcher.write_text(text)
if args.pc_jit_reuse:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 assert marker in text
 (OUT/'host.c').write_text(text.replace(marker,'   printf("JITREUSE replacements=%u\\n",receive(PC_VIDEO+0xd5c));\n'+marker))
if args.pc_jit_hot:
 text=(OUT/'host.c').read_text();marker='   printf("CPU_CYCLES applied=%u\\n",receive(PC_CYCLE_APPLIED));'
 addition='   {unsigned o=PC_VIDEO+0xe60;syncio->pull(shared+o,404,NULL);printf("JITHOT magic=%08x used=%u dropped=%u total=%u scoped=%u\\n",ad_get(shared,o),ad_get(shared,o+4),ad_get(shared,o+8),ad_get(shared,o+12),ad_get(shared,o+400));for(unsigned i=0;i<16;i++){unsigned r=o+16+i*24;printf("JITHOT pc=%08x count=%u bytes=%08x%08x%08x%08x\\n",ad_get(shared,r),ad_get(shared,r+4),ad_get(shared,r+8),ad_get(shared,r+12),ad_get(shared,r+16),ad_get(shared,r+20));}}\n'
 assert marker in text;(OUT/'host.c').write_text(text.replace(marker,addition+marker))
version='0.11.0-gl-experiment' if args.pc_opengl else '0.11.0-dev' if args.pc_adapters else '0.10.1-dev' if args.pc else '0.4.0-dev' if args.shell else '0.3.0-dev' if args.console else '0.2.0-dev'
if args.pc_pic_vfp:version+='-pic-vfp'
if args.pc_matrix_bench:version+='-matrix-bench'
if args.pc_tinygl_bench:version+='-tinygl-internal'
if args.pc_tinygl_raster:version+='-raster'
if args.pc_hlslpp:version+='-hlslpp-'+args.pc_hlslpp
if args.pc_memory_debug:version='0.12.0-dev-memory'
if args.pc_video_trace:version='0.11.1-gl-trace' if args.pc_opengl else '0.11.1-dev-trace'
if args.pc_network:version+='-network-test'
if args.pc_compat:version='0.22.2-dev-compat'
if args.pc_x87:version+='-x87'
if args.pc_expanded:version='0.22.2-dev-expanded'
if args.pc_network and args.pc_compat and args.pc_dynrec:version='0.22.2-dev-network-jit'
if args.pc_jit_inline and not args.pc_duke_clock:version='0.22.5-dev-inline-read'
if args.pc:
 version=re.search(r'#define ZZ_DOSBOX_VERSION "([^"]+)"',(BASE/'version.h').read_text())[1]
if args.pc_network:subprocess.run(['python3',str(ROOT/'scripts/build_dosbox_network.py'),'--library'],check=True,cwd=ROOT)
subprocess.run([*host_command,'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror',
 *(['-DDB_EXPANDED'] if args.pc_expanded else []),*(['-DDB_COMPAT'] if args.pc_compat else []),*(['-DDB_NETWORK'] if args.pc_network else []),*(['-DDB_MEMORY_DEBUG','-Iamiga/dosbox/developer'] if args.pc_memory_debug else []),*(['-DDB_VIDEO_TRACE'] if args.pc_video_trace else []),*(['-DDB_OPENGL'] if args.pc_opengl else []),*(['-DDB_SCANLINE_TEST'] if args.pc_scanlines else []),*(['-DDB_PRESENT_TEST'] if args.pc_presentation else []),*(['-DDB_PIPELINE_DEFAULT=1'] if args.pc_pipeline else []),*(['-DFF_DIRECT','-DZZ_PC_CD'] if args.pc else []),'-DZZ_HEADLESS_BENCH','-DZZ_RELEASE','-DZZ_BLOCK_SIZE='+hex(allocation),'-DZZ_CONTROL=0x180000','-DMB_ALLOCATION='+hex(allocation),'-DDB_JIT_MIB='+str(args.pc_jit_mib),'-DZZ_APP_NAME="'+binary.name+'"','-DZZ_APP_VERSION="'+version+'"','-DZZ_MIN_STACK=65536','-DMB_MMU','-DMB_DCACHE','-DZZ_HEADLESS_MMU','-DZZ_HEADLESS_ICACHE',
 '-I'+str(relative),'-Iamiga/dosbox/pc','-Iamiga/arm_debug','-Iamiga/arm_debug/zz9000','-Iamiga/common/arm','-Iamiga/dosbox/dos','-Iamiga/dosbox/console','-Iamiga/dosbox/shell','-Iamiga/dosbox/pc',str(launcher.relative_to(ROOT)),str(relative/'host.c'),*(['amiga/dosbox/console/window.c'] if args.console else []),*(['amiga/dosbox/shell/fs_host.c'] if args.shell else []),*(['amiga/dosbox/pc/display.c','amiga/dosbox/pc/cd_host.c','amiga/dosbox/pc/midi_host.c'] if args.pc else []),*(['amiga/dosbox/pc/audio_host.c'] if args.pc_compat else []),*(['amiga/dosbox/pc/serial_host.c'] if args.pc_expanded else []),*(['.context/amiga/dosbox-network/amiga/libzznetwork.a'] if args.pc_network else []),'-lamiga','-o',str(relative/binary.name)],cwd=ROOT,check=True)
assert binary.read_bytes()[:4]==b'\0\0\x03\xf3'
record={'expanded':args.pc_expanded,'guest_ram_max_mb':32 if args.pc_expanded and args.pc_jit_mib!=32 else 8,'x87':args.pc_x87,'upstream':json.loads((BASE/'upstream.json').read_text()),'scope':('EMS/XMS, SBPro2/DBOPL and batched AHI compatibility runtime' if args.pc_compat else 'PC BIOS/PIT/keyboard/EGA, CD and external CAMD MIDI prototype; no PCM audio; no game-speed claim' if args.pc else 'Upstream DOS shell with live flat C drive; no VGA/audio/PC BIOS' if args.shell else 'Real DOS kernel, external COM/EXE loading and bounded private file drive; no VGA/audio/shell yet'), 'image_bytes':len(image),'relocations':len(rel),'allocation':allocation,
 'outputs':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in [elf,binary,OUT/'integration.patch',OUT/'Diagnostic.com',OUT/'dos-integration.patch',OUT/'FileTest.com',OUT/'ExeTest.exe',OUT/'INPUT.TXT',*([OUT/'Echo.com',OUT/'IoTest.com'] if args.console else []),*([OUT/'shell-integration.patch',OUT/'LoadFix.com'] if args.shell else []),*([OUT/'pc-integration.patch',OUT/'EGATEST.COM'] if args.pc else []),*([OUT/'dynrec-integration.patch'] if args.pc_dynrec else []),*([OUT/'jit-value-integration.patch'] if args.pc_jit_value else []),*([OUT/'jit-reuse-integration.patch'] if args.pc_jit_reuse else []),*([OUT/'jit-inline-integration.patch'] if args.pc_jit_inline else [])]},'libc_sha256':hashlib.sha256(libc.read_bytes()).hexdigest(),
 'row_cache':args.pc_row_cache,'file_readahead_bytes':4096 if args.pc_file_readahead else 0,'network':args.pc_network,'simd_context':args.pc,'disk_cache_kib':args.pc_disk_cache_kib,'capture_unroll':args.pc_capture_unroll,'raster_fixed':args.pc_raster_fixed,'raster_compiler':args.pc_raster_compiler,'tinygl_raster':args.pc_tinygl_raster,'tinygl_bench':args.pc_tinygl_bench,'matrix_bench':args.pc_matrix_bench,'hlslpp':json.loads((OUT/'hlslpp-provenance.json').read_text()) if args.pc_hlslpp else None,'cmsis':json.loads((OUT/'cmsis-provenance.json').read_text()) if args.pc_matrix_bench and not args.pc_tinygl_bench and not args.pc_hlslpp else None,'memory_debug':args.pc_memory_debug,'neon_dispatch':args.pc_neon_dispatch,'pixel_bench':args.pc_pixel_bench,'pic_vfp':args.pc_pic_vfp,'video_trace':args.pc_video_trace,'software_opengl':args.pc_opengl,'tinygl':json.loads((OUT/'tinygl-provenance.json').read_text()) if (args.pc_opengl or args.pc_matrix_bench) else None,'video_adapters':['vga','ega','cga','s3','tandy','pcjr','hercules'] if args.pc else [],'jit_inline':args.pc_jit_inline,'jit_reuse':args.pc_jit_reuse,'jit_value':args.pc_jit_value,'jit_hot':args.pc_jit_hot,'duke_clock':args.pc_duke_clock,'descent_clock':args.pc_descent_clock,'doom_clock':args.pc_doom_clock,'dynrec':args.pc_dynrec,'jit_mib':args.pc_jit_mib,'neon':args.pc_neon,'pipeline_default':args.pc_pipeline,'scanline_test':args.pc_scanlines,'presentation_test':args.pc_presentation,'console':args.console,'shell':args.shell,'pc':args.pc,'version':version,
 'source_hashes':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [*sources,*SRC.glob('*.*'),*console.glob('*.*'),*shell.glob('*.*'),*pc.glob('*.*'),ROOT/'amiga/common/flip_plan.h',BASE/'version.h',BASE/'elf.py',launcher,ROOT/'scripts/build_dosbox.py',ROOT/'scripts/build_dosbox_dos.py',*([ROOT/'scripts/build_dosbox_shell.py',shell/'vendor/upstream.json'] if args.shell else []),*([ROOT/'scripts/build_dosbox_pc.py',ROOT/'amiga/dosbox/launcher/session.h'] if args.pc else []),*([ROOT/'scripts/prepare_dosbox_dynrec.py'] if args.pc_dynrec else []),*([ROOT/'scripts/prepare_dosbox_jit_value.py'] if args.pc_jit_value else []),*([ROOT/'scripts/prepare_dosbox_jit_reuse.py'] if args.pc_jit_reuse else []),*([ROOT/'scripts/prepare_dosbox_jit_inline.py'] if args.pc_jit_inline else []),*([ROOT/'scripts/prepare_dosbox_x87.py'] if args.pc_x87 else []),*([ROOT/'scripts/prepare_dosbox_expanded.py'] if args.pc_expanded else []),*([ROOT/'scripts/prepare_dosbox_compat.py',ROOT/'scripts/prepare_dosbox_audio.py'] if args.pc_compat else []),Path(__file__),ROOT/'amiga/arm_debug/zz9000/launcher.c',ROOT/'amiga/arm_debug/zz9000/layout.h',ROOT/'amiga/arm_debug/zz9000/mapping_probe.S',OUT/'host.c',OUT/'mmu_plan.h'] if p.is_file()},
 'arm_flags':flags,'arm_overrides':overrides,'compiler':subprocess.check_output([*cmd,'arm-none-eabi-g++','--version'],text=True)}
if args.pc_network:
 record['network_backend']=json.loads((ROOT/'.context/amiga/dosbox-network/amiga/library-build.json').read_text())
 record['network_sources']={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [*(BASE/'network').rglob('*'),ROOT/'scripts/prepare_dosbox_network.py',ROOT/'scripts/build_dosbox_network.py',ROOT/'scripts/prepare_dosbox_ipx.py'] if p.is_file()}
record['build_options']={k:v for k,v in vars(args).items() if k not in ('base','output','container_command')}
record['base_manifest_sha256']=hashlib.sha256(base_raw).hexdigest() if base_record else None
record['product_version_source_sha256']=hashlib.sha256((BASE/'version.h').read_bytes()).hexdigest()
(OUT/'build.json').write_text(json.dumps(record,indent=2)+'\n')
print('Built',binary,binary.stat().st_size,'bytes; ARM image',len(image),'relocations',len(rel))
