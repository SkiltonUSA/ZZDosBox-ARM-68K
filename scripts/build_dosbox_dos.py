#!/usr/bin/env python3
"""Stage the real DOSBox DOS kernel and an isolated memory-drive frontend."""
import argparse,difflib,hashlib,shutil,struct,subprocess
from pathlib import Path
import build_dosbox as cpu
ROOT=cpu.ROOT;SRC=ROOT/'amiga/dosbox/dos';OUT=ROOT/'.context/amiga/dosbox-dos';UP=cpu.UP
UNITS=['dos','dos_files','dos_execute','dos_memory','dos_tables','dos_classes','dos_ioctl','dos_devices']
def prepare():
 cpu.OUT=OUT;sources=cpu.prepare()
 shutil.copytree(UP/'src/dos',OUT/'src/dos',dirs_exist_ok=True)
 patches=[]
 def change(rel,fn):
  p=OUT/rel;old=p.read_text();new=fn(old);assert new!=old,rel;p.write_text(new)
  patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 def dos(s):
  s=s[:s.index('class DOS:public Module_base')]
  s=s.replace('#include "setup.h"','').replace('#include "serialport.h"','')
  first=s.index('\tcase 0x03:');last=s.index('\tcase 0x05:',first)
  s=s[:first]+'\tcase 0x03: case 0x04: E_Exit("Serial device not connected");\n'+s[last:]
  start=s.index('static Bit16u DOS_SectorAccess');end=s.index('static Bitu DOS_25Handler',start)
  s=s[:start]+'static Bit16u DOS_SectorAccess(bool){return 0x8002;}\n'+s[end:]
  s=s.replace('#include "bios.h"','#include "bios.h"\n#define PIT_TICK_RATE 1193182')
  s+='''
void zz_dos_init() {
 const Bitu ints[]={0x20,0x21,0x25,0x26,0x27,0x28,0x29};
 const Bitu types[]={CB_IRET,CB_INT21,CB_RETF_STI,CB_RETF_STI,CB_IRET,CB_IRET,CB_INT29};
 CallBack_Handler funcs[]={DOS_20Handler,DOS_21Handler,DOS_25Handler,DOS_26Handler,DOS_27Handler,nullptr,nullptr};
 for(unsigned i=0;i<7;i++){Bitu cb=CALLBACK_Allocate();CALLBACK_Setup(cb,funcs[i],types[i],"DOS");RealSetVec(ints[i],CALLBACK_RealPointer(cb));}
 DOS_SetupFiles();DOS_SetupDevices();DOS_SetupTables();DOS_SetupMemory();
 DOS_SDA(DOS_SDA_SEG,DOS_SDA_OFS).SetDrive(2);DOS_SetDefaultDrive(2);
 dos.version.major=5;dos.version.minor=0;dos.direct_output=false;dos.internal_output=false;
}
'''
  return s
 change('src/dos/dos.cpp',dos)
 change('src/dos/dos_files.cpp',lambda s:s.replace('Drives[25]=new Virtual_Drive();','/* Drives are installed by the platform frontend. */'))
 # Header-only local/ISO/libretro drive declarations pull in unrelated shells.
 for name in ['dos.cpp','dos_files.cpp','dos_ioctl.cpp','dos_devices.cpp']:
  p=OUT/'src/dos'/name
  if '#include "drives.h"' in p.read_text():change('src/dos/'+name,lambda s:s.replace('#include "drives.h"','#include "drive_decl.h"'))
 change('src/dos/dos_devices.cpp',lambda s:s.replace('#include "dev_con.h"','#include "console.h"'))
 change('src/dos/dos_tables.cpp',lambda s:s.replace('void DOS_SetupTables(void) {','void DOS_SetupTables(void) {\n dos_memseg=DOS_PRIVATE_SEGMENT;'))
 # Retain original helper declarations without an unused std::string dependency.
 (OUT/'dos-integration.patch').write_text(''.join(patches))
 memory=(UP/'src/hardware/memory.cpp').read_text()
 (OUT/'dos-memory.cpp').write_text(memory[:memory.index('#include')]+'\n#include "paging.h"\n'+memory[memory.index('Bitu mem_strlen('):memory.index('Bitu MEM_TotalPages(void)')])
 drive=(UP/'src/dos/drives.cpp').read_text()
 (OUT/'dos-drive-helpers.cpp').write_text(drive[:drive.index('#include')]+'\n#include "dos_inc.h"\n'+drive[drive.index('bool WildFileCmp('):drive.index('DOS_Drive::DOS_Drive()')])
 support=(UP/'src/misc/support.cpp').read_text()
 (OUT/'dos-case.cpp').write_text(support[:support.index('#include')]+'\n#include <ctype.h>\n'+support[support.index('char * upcase(char * str)'):support.index('char * lowcase(char * str)')])
 for name,origin in [('filetest',0x100),('exetest',0)]:
  obj=OUT/(name+'.o');binary=OUT/(name+'.bin')
  subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/(name+'.S')),'-o',str(obj)],check=True)
  subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext',hex(origin),'--oformat=binary',str(obj),'-o',str(binary)],check=True)
  data=binary.read_bytes()
  if name=='exetest':
   symbols=subprocess.check_output(['/opt/homebrew/opt/llvm/bin/llvm-nm',str(obj)],text=True)
   at=int(next(line.split()[0] for line in symbols.splitlines() if line.endswith(' reloc_check')),16)
   size=32+len(data)
   data=struct.pack('<14H',0x5a4d,size%512,(size+511)//512,1,2,0x100,0xffff,0,0x1000,0,0,0,28,0)+struct.pack('<HH',at,0)+data
  (OUT/('FileTest.com' if name=='filetest' else 'ExeTest.exe')).write_bytes(data)
  (OUT/(name+'.h')).write_text('static const uint8_t '+name+'[]={'+','.join(map(str,data))+'};\n')
 (OUT/'INPUT.TXT').write_bytes(bytes((i*13+7)&255 for i in range(256)))
 return sources+[OUT/'dos-case.cpp',OUT/'dos-drive-helpers.cpp',OUT/'dos-memory.cpp']+[OUT/'src/dos'/f'{n}.cpp' for n in UNITS]
def console_fixtures():
 for name,output in [('echo','Echo.com'),('io_test','IoTest.com')]:
  obj=OUT/(name+'.o');binary=OUT/output
  subprocess.run(['clang','--target=i386-none-elf','-c',str(ROOT/'amiga/dosbox/console'/(name+'.S')),'-o',str(obj)],check=True)
  subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(obj),'-o',str(binary)],check=True)
  (OUT/(name+'.h')).write_text('static const uint8_t '+name+'[]={'+','.join(map(str,binary.read_bytes()))+'};\n')

def main():
 p=argparse.ArgumentParser();p.add_argument('--prepare-only',action='store_true');p.add_argument('--sanitize',action='store_true');p.add_argument('--console',action='store_true');args=p.parse_args()
 global OUT
 if args.console:OUT=ROOT/'.context/amiga/dosbox-console'
 sources=prepare()
 if args.console:console_fixtures()
 if args.prepare_only:return
 flags=['-std=c++17','-O2','-g','-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-Wno-format',*['-I'+str(x) for x in [SRC,OUT,OUT/'include',OUT/'src/cpu',ROOT/'amiga/dosbox']]]
 if args.sanitize:flags+=['-fsanitize=address','-fno-omit-frame-pointer']
 sources += [SRC/'adapter.cpp',SRC/'drive.cpp',ROOT/'amiga/dosbox/console/native.cpp' if args.console else SRC/'native.cpp']
 digest_headers=hashlib.sha256(b''.join(p.read_bytes() for p in sorted((OUT/'include').glob('*.h')))+b''.join(p.read_bytes() for p in sorted(SRC.glob('*.h')))+b''.join(p.read_bytes() for p in sorted(OUT.glob('*.h')) if p.name in ['fixture.h','filetest.h','exetest.h','echo.h','io_test.h'])).digest()
 objects=[]
 for i,source in enumerate(sources):
  obj=OUT/(f'{i}-'+source.stem+'.native.o');stamp=obj.with_suffix('.sha256');objects.append(str(obj))
  digest=hashlib.sha256(source.read_bytes()+digest_headers+str(flags).encode()).hexdigest()
  if not stamp.exists() or stamp.read_text()!=digest:
   subprocess.run(['clang++',*flags,'-c',str(source),'-o',str(obj)],check=True);stamp.write_text(digest)
 subprocess.run(['clang++',*flags,*objects,'-Wl,-dead_strip','-o',str(OUT/'dos-kernel-test')],check=True)
 subprocess.run([str(OUT/'dos-kernel-test')],check=True)
if __name__=='__main__':main()
