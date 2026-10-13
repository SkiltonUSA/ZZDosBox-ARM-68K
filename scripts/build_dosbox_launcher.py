#!/usr/bin/env python3
"""Build the MUI launcher and disk helper; does not deploy or start Core1."""
import hashlib,json,subprocess
from pathlib import Path
from amiga import PIN,container_command
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/dosbox-launcher';OUT.mkdir(exist_ok=True,parents=True)
subprocess.run(['python3',str(ROOT/'scripts/prepare_dosbox_icon.py')],check=True,cwd=ROOT)
subprocess.run([*container_command(),'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','amiga/dosbox/launcher/main.c','-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosBox-Launcher'],check=True,cwd=ROOT)
subprocess.run([*container_command(),'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-Wall','-Wextra','-Werror','-I.context/amiga/dosbox-launcher','amiga/dosbox/launcher/icon.c','-lamiga','-o','.context/amiga/dosbox-launcher/MakeLauncherIcon'],check=True,cwd=ROOT)
subprocess.run([*container_command(),'run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-Wframe-larger-than=2048','-fstack-usage','amiga/dosbox/launcher/disk.c','-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosDisk'],check=True,cwd=ROOT)
(ROOT/'disk.su').replace(OUT/'disk-stack.su')
p=OUT/'ZZDosBox-Launcher';assert p.read_bytes()[:4]==bytes.fromhex('000003f3')
record={'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'sources':{str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in (ROOT/'amiga/dosbox/launcher').glob('*') if f.is_file()}}
(OUT/'launcher-build.json').write_text(json.dumps(record,indent=2)+'\n');print(record['bytes'],record['sha256'])
