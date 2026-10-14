#!/usr/bin/env python3
"""Build the MUI launcher and disk helper; does not deploy or start Core1."""
import hashlib,json,subprocess
from prepare_dosbox_disasm import prepare
from pathlib import Path
from amiga import PIN,container_command
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/dosbox-launcher';OUT.mkdir(exist_ok=True,parents=True)
disasm=prepare(OUT)
subprocess.run(['python3',str(ROOT/'scripts/prepare_dosbox_icon.py')],check=True,cwd=ROOT)
subprocess.run([*container_command(),'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-fstack-usage','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-fwrapv','-Iamiga/dosbox/developer','amiga/dosbox/launcher/main.c','amiga/dosbox/pc/cd_host.c',str(disasm.relative_to(ROOT)),'-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosBox-Launcher'],check=True,cwd=ROOT)
subprocess.run([*container_command(),'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-Wall','-Wextra','-Werror','-I.context/amiga/dosbox-launcher','amiga/dosbox/launcher/icon.c','-lamiga','-o','.context/amiga/dosbox-launcher/MakeLauncherIcon'],check=True,cwd=ROOT)
subprocess.run([*container_command(),'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-Wframe-larger-than=2048','-fstack-usage','amiga/dosbox/launcher/disk.c','-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosDisk'],check=True,cwd=ROOT)
(ROOT/'main.su').replace(OUT/'launcher-stack.su')
(ROOT/'cd_host.su').replace(OUT/'cd-stack.su')
(ROOT/'disk.su').replace(OUT/'disk-stack.su')
p=OUT/'ZZDosBox-Launcher';assert p.read_bytes()[:4]==bytes.fromhex('000003f3')
record={'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'sources':{str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in (ROOT/'amiga/dosbox/launcher').glob('*') if f.is_file()}}
(OUT/'launcher-build.json').write_text(json.dumps(record,indent=2)+'\n');print(record['bytes'],record['sha256'])

import shutil
shutil.copy2(ROOT/".context/amiga/dosbox-pc/ZZDOSCD.SYS",OUT/"ZZDOSCD.SYS")
shutil.copy2(ROOT/".context/amiga/dosbox-pc/ZZHELLO.COM",OUT/"ZZHELLO2.COM")
