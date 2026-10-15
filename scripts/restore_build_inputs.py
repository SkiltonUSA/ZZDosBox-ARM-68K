#!/usr/bin/env python3
"""Restore saved development inputs locally; no deployment or hardware access."""
import argparse,json,shutil,subprocess,tarfile
from pathlib import Path
from verify_backup import verify
ROOT=Path(__file__).resolve().parents[1]
def main():
 parser=argparse.ArgumentParser()
 group=parser.add_mutually_exclusive_group()
 group.add_argument("--windows-experiment",action="store_true")
 group.add_argument("--mouse-build",action="store_true")
 group.add_argument("--neon-build",action="store_true")
 group.add_argument("--scanline-build",action="store_true")
 group.add_argument("--pic-vfp-build",action="store_true")
 args=parser.parse_args()
 pc_source="artifacts/development/mouse/pc" if args.mouse_build else "artifacts/experiments/wfw311-small-tlb/pc" if args.windows_experiment else "artifacts/0.8.0-dev/pc"
 pc_target=".context/amiga/dosbox-pc"
 if args.neon_build or args.scanline_build:
  pc_source="artifacts/experiments/scanline-copy/pc" if args.scanline_build else "artifacts/development/neon-dynrec/pc"
  pc_target=".context/amiga/dosbox-dynrec-4m-neon-pipe"+("-scan" if args.scanline_build else "")
 if args.pic_vfp_build:
  pc_source="artifacts/development/pic-vfp/pc"
  pc_target=".context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp"
 verify()
 checkout=ROOT/'.context/research/dosbox-libretro';checkout.parent.mkdir(parents=True,exist_ok=True)
 pin=json.loads((ROOT/'amiga/dosbox/upstream.json').read_text())
 if not checkout.exists():
  subprocess.run(['git','clone',str(ROOT/'sources/dosbox-libretro.bundle'),str(checkout)],check=True)
  subprocess.run(['git','-C',str(checkout),'checkout','--detach',pin['revision']],check=True)
 assert subprocess.check_output(['git','-C',str(checkout),'rev-parse','HEAD'],text=True).strip()==pin['revision']
 assert not subprocess.check_output(['git','-C',str(checkout),'status','--porcelain'],text=True).strip()
 for source,target in [(pc_source,pc_target),('artifacts/0.10.1-dev/launcher' if args.mouse_build else 'artifacts/0.8.0-dev/launcher','.context/amiga/dosbox-launcher')]:
  out=ROOT/target;out.mkdir(parents=True,exist_ok=True)
  for p in (ROOT/source).iterdir():
   q=out/p.name
   if not q.exists():shutil.copy2(p,q)
 for source,target in [('libc.a','.context/amiga/dosbox-libc/libc.a'),('libc-seed.a','.context/amiga/dosbox-toolchain/libc-pic.a')]:
  p=ROOT/target;p.parent.mkdir(parents=True,exist_ok=True)
  if not p.exists():shutil.copy2(ROOT/'artifacts/build-support'/source,p)
 base=ROOT/'.context/amiga/dosbox-toolchain'
 if not (base/'picolibc-1.8.12').exists():
  with tarfile.open(ROOT/'sources/picolibc_1.8.12.orig.tar.gz') as archive:archive.extractall(base,filter='data')
 if args.pic_vfp_build:
  out=ROOT/'.context/amiga/dosbox-pic-vfp-baseline';out.mkdir(parents=True,exist_ok=True)
  for p in (ROOT/'artifacts/development/pic-vfp/baseline').iterdir():
   q=out/p.name
   if not q.exists():shutil.copy2(p,q)
 drive=ROOT/'.context/amiga/dosbox-shell/CDrive';drive.mkdir(parents=True,exist_ok=True)
 for name in ['Diagnostic.com','FileTest.com','ExeTest.exe','INPUT.TXT']:
  target=drive/name.upper()
  if not target.exists():shutil.copy2(ROOT/'artifacts/0.8.0-dev/pc'/name,target)
 print('Saved inputs restored; existing generated outputs were preserved.')
if __name__=='__main__':main()
