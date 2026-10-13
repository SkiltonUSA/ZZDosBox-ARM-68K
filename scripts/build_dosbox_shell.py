#!/usr/bin/env python3
"""Stage pinned upstream DOSBox shell on the existing DOS/console frontend."""
import argparse,difflib,hashlib,re,subprocess
from pathlib import Path
import build_dosbox_dos as dos
ROOT=dos.ROOT;OUT=ROOT/'.context/amiga/dosbox-shell';SRC=ROOT/'amiga/dosbox/shell';UP=dos.UP

def prepare():
 dos.OUT=OUT;sources=dos.prepare();dos.console_fixtures();patches=[]
 subprocess.run(['clang','--target=i386-none-elf','-c',str(SRC/'loadfix.S'),'-o',str(OUT/'loadfix.o')],check=True)
 subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(OUT/'loadfix.o'),'-o',str(OUT/'LoadFix.com')],check=True)
 def stage(rel,fn):
  old=(UP/rel).read_text();new=fn(old);p=OUT/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(new)
  patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel));return p
 def remove_function(s,signature):
  a=s.index(signature);b=s.index('{',a);depth=1;i=b+1
  # These selected bodies have balanced brace characters in their strings too.
  while depth:
   if s[i]=='{':depth+=1
   if s[i]=='}':depth-=1
   i+=1
  return s[:a]+s[i:]
 def shell(s):
  a=s.index('DOS_Shell::DOS_Shell()');b=s.index('class AUTOEXEC:')
  return s[:s.index('*/')+2]+'\n'+ '#include "shell.h"\n#include "callback.h"\n#include "support.h"\n#include "regs.h"\nBitu call_shellstop; DOS_Shell *first_shell=nullptr;\n'+s[a:b]
 sources.append(stage('src/shell/shell.cpp',shell))
 sources.append(stage('src/shell/shell_misc.cpp',lambda s:s.replace('l_history.push_front(line); it_history = l_history.begin();','l_history.push_front(line);if(l_history.size()>32)l_history.pop_back(); it_history = l_history.begin();')))
 sources.append(stage('src/shell/shell_batch.cpp',lambda s:s))
 def cmds(s):
  s=s.replace('#include "../dos/drives.h"','#include "drive_decl.h"').replace('#include "control.h"','')
  for name in ['CheckConfig','CMD_SUBST','CMD_DATE','CMD_TIME']:
   s=remove_function(s,('bool ' if name=='CheckConfig' else 'void ')+'DOS_Shell::'+name+'(' if name!='CMD_SUBST' else 'void DOS_Shell::CMD_SUBST (')
  s+='\nbool DOS_Shell::CheckConfig(char*,char*){return false;}\n'
  s=re.sub(r'^\{\s*"(?:SUBST|DATE|TIME)".*\n','',s,flags=re.M)
  return s
 sources.append(stage('src/shell/shell_cmds.cpp',cmds))
 def programs(s):
  return s[:s.index('*/')+2]+'\n'+'#include "programs.h"\n#include "support.h"\n#include <stdarg.h>\n'+s[s.index('Program::Program()'):s.index('bool MSG_Write(')]
 sources.append(stage('src/misc/programs.cpp',programs))
 sources.append(stage('src/misc/setup.cpp',lambda s:s[:s.index('*/')+2]+'\n'+'#include "programs.h"\n#include "support.h"\n'+s[s.index('bool CommandLine::FindExist('):]))
 sources=[p for p in sources if p.name!='dos-case.cpp']
 sources.append(stage('src/misc/support.cpp',lambda s:s[:s.index('*/')+2]+'\n'+'#include <algorithm>\n#include <string>\n#include <cstring>\n#include "support.h"\n'+s[s.index('void upcase(std::string'):s.index('static char buf[1024]')]))
 source=(UP/'src/shell/shell.cpp').read_text();a=source.index('\t/* Add messages */',source.index('void SHELL_Init()'));b=source.index('\t/* Regular startup */',a)
 msgs=source[a:b].replace('DOSBox %s Command Shell','ZZDosBox %s Command Shell')
 (OUT/'shell_messages.cpp').write_text(source[:source.index('*/')+2]+'\n'+'#include "support.h"\nvoid zz_shell_messages(){\n'+msgs+'}\n')
 sources.append(OUT/'shell_messages.cpp')
 (OUT/'shell-integration.patch').write_text(''.join(patches))
 return sources

def main():
 p=argparse.ArgumentParser();p.add_argument('--sanitize',action='store_true');p.add_argument('--prepare-only',action='store_true');args=p.parse_args();sources=prepare()
 if args.prepare_only:return
 flags=['-std=c++17','-O2','-g','-fno-exceptions','-fno-rtti','-ffunction-sections','-fdata-sections','-Wno-format',*['-I'+str(x) for x in [SRC,ROOT/'amiga/dosbox/dos',OUT,OUT/'include',OUT/'src/cpu',ROOT/'amiga/dosbox']]]
 if args.sanitize:flags+=['-fsanitize=address','-fno-omit-frame-pointer']
 sources += [SRC/'adapter.cpp',ROOT/'amiga/dosbox/dos/drive.cpp',SRC/'frontend.cpp',SRC/'native.cpp']
 headers=b''.join(p.read_bytes() for base in [OUT,OUT/'include',SRC,ROOT/'amiga/dosbox/dos'] for p in sorted(base.glob('*.h')))
 objects=[]
 for i,source in enumerate(sources):
  obj=OUT/(f'{i}-'+source.stem+'.native.o');stamp=obj.with_suffix('.sha256');objects.append(str(obj))
  digest=hashlib.sha256(source.read_bytes()+headers+str(flags).encode()).hexdigest()
  if not stamp.exists() or stamp.read_text()!=digest:
   subprocess.run(['clang++',*flags,'-c',str(source),'-o',str(obj)],check=True);stamp.write_text(digest)
 subprocess.run(['clang++',*flags,*objects,'-Wl,-dead_strip','-o',str(OUT/'shell-test')],check=True)
 subprocess.run([str(OUT/'shell-test')],check=True)
 fs_flags=['-std=c11','-O1','-g','-Wno-deprecated-declarations','-DZZ_CONTROL=0x180000']
 if args.sanitize:fs_flags+=['-fsanitize=address,undefined']
 fs_dirs=[ROOT/'tests/amiga/dosbox/fs_mock',SRC,ROOT/'amiga/dosbox/console',ROOT/'amiga/dosbox/dos',ROOT/'amiga/mesen_bench',ROOT/'amiga/arm_debug',ROOT/'amiga/arm_debug/zz9000']
 subprocess.run(['clang',*fs_flags,*['-I'+str(x) for x in fs_dirs],str(SRC/'fs_host.c'),str(ROOT/'tests/amiga/dosbox/fs_host_test.c'),'-o',str(OUT/'fs-host-test')],check=True)
 subprocess.run([str(OUT/'fs-host-test')],check=True)
if __name__=='__main__':main()
