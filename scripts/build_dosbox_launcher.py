#!/usr/bin/env python3
"""Build the MUI launcher and disk helper; does not deploy or start Core1."""
import hashlib,json,subprocess
from prepare_dosbox_disasm import prepare
from pathlib import Path
from amiga import PIN
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'.context/amiga/dosbox-launcher';OUT.mkdir(exist_ok=True,parents=True)
subprocess.run(['python3',str(ROOT/'scripts/build_dosbox_compatibility_details.py')],check=True,cwd=ROOT)
subprocess.run(['python3',str(ROOT/'scripts/prepare_dosbox_network_guide.py')],check=True,cwd=ROOT)
subprocess.run(['python3',str(ROOT/'scripts/prepare_dosbox_changelog.py')],check=True,cwd=ROOT)
disasm=prepare(OUT)
subprocess.run(['python3',str(ROOT/'scripts/prepare_dosbox_icon.py')],check=True,cwd=ROOT)
subprocess.run(['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-fstack-usage','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-fwrapv','-Iamiga/dosbox/developer','-I.context/amiga/dosbox-launcher','amiga/dosbox/launcher/main.c','amiga/dosbox/pc/cd_host.c',str(disasm.relative_to(ROOT)),'-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosBox-Launcher'],check=True,cwd=ROOT)
subprocess.run(['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-Wall','-Wextra','-Werror','-I.context/amiga/dosbox-launcher','amiga/dosbox/launcher/icon.c','-lamiga','-o','.context/amiga/dosbox-launcher/MakeLauncherIcon'],check=True,cwd=ROOT)
subprocess.run(['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-Wframe-larger-than=2048','-fstack-usage','amiga/dosbox/launcher/disk.c','-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosDisk'],check=True,cwd=ROOT)
subprocess.run(['podman','--connection','nuflix-converter-root','run','--rm','--platform','linux/amd64','-v',f'{ROOT}:/work','-w','/work',PIN['image'],'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-Wframe-larger-than=2048','-fstack-usage','amiga/dosbox/launcher/create_disk.c','-lamiga','-o','.context/amiga/dosbox-launcher/ZZDosCreate'],check=True,cwd=ROOT)
(ROOT/'create_disk.su').replace(OUT/'create-disk-stack.su')
(ROOT/'main.su').replace(OUT/'launcher-stack.su')
(ROOT/'cd_host.su').replace(OUT/'cd-stack.su')
(ROOT/'disk.su').replace(OUT/'disk-stack.su')
p=OUT/'ZZDosBox-Launcher';assert p.read_bytes()[:4]==bytes.fromhex('000003f3')
record={'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'sources':{str(f.relative_to(ROOT)):hashlib.sha256(f.read_bytes()).hexdigest() for f in [*(ROOT/'amiga/dosbox/launcher').glob('*'), ROOT/'amiga/dosbox/pc/welcome.S', ROOT/'scripts/build_dosbox_launcher.py', ROOT/'amiga/dosbox/assets/zzdosbox-logo.png', ROOT/'scripts/prepare_dosbox_icon.py', ROOT/'scripts/prepare_dosbox_network_guide.py', ROOT/'docs/amiga-dosbox-network-setup.md', ROOT/'scripts/prepare_dosbox_changelog.py', ROOT/'amiga/dosbox/CHANGELOG.md', ROOT/'amiga/dosbox/compatibility/data.h', ROOT/'amiga/dosbox/compatibility/details_meta.h', ROOT/'amiga/dosbox/network/config.h', ROOT/'amiga/dosbox/network/control.h'] if f.is_file()}}
(OUT/'launcher-build.json').write_text(json.dumps(record,indent=2)+'\n');print(record['bytes'],record['sha256'])

import shutil
shutil.copy2(ROOT/".context/amiga/dosbox-pc/ZZDOSCD.SYS",OUT/"ZZDOSCD.SYS")
subprocess.run(['clang','--target=i386-none-elf','-c',str(ROOT/'amiga/dosbox/pc/welcome.S'),'-o',str(OUT/'welcome.o')],check=True)
subprocess.run(['/opt/homebrew/opt/lld/bin/ld.lld','-m','elf_i386','--image-base=0','-Ttext','0x100','--oformat=binary',str(OUT/'welcome.o'),'-o',str(OUT/'ZZHELLO2.COM')],check=True)

(OUT/"Compatibility.md").write_text((ROOT/"docs/amiga-dosbox-compatibility.md").read_text().replace("amiga-dosbox-game-details.md", "Compatibility-Details.md"))

(OUT/"Compatibility-Details.md").write_text((ROOT/"docs/amiga-dosbox-game-details.md").read_text().replace("amiga-dosbox-compatibility.md", "Compatibility.md"))

record['assets']={name:{'bytes':(OUT/name).stat().st_size,'sha256':hashlib.sha256((OUT/name).read_bytes()).hexdigest()} for name in ['Network.guide','Compatibility.dat','Compatibility.md','Compatibility-Details.md','ZZDosDisk','ZZDosCreate','ZZHELLO2.COM']}
(OUT/'launcher-build.json').write_text(json.dumps(record,indent=2)+'\n')
