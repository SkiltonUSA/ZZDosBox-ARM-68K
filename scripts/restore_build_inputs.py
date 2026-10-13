#!/usr/bin/env python3
"""Restore saved development inputs locally; no deployment or hardware access."""
import json,shutil,subprocess,tarfile
from pathlib import Path
from verify_backup import verify
ROOT=Path(__file__).resolve().parents[1]
def main():
 verify()
 checkout=ROOT/'.context/research/dosbox-libretro';checkout.parent.mkdir(parents=True,exist_ok=True)
 pin=json.loads((ROOT/'amiga/dosbox/upstream.json').read_text())
 if not checkout.exists():
  subprocess.run(['git','clone',str(ROOT/'sources/dosbox-libretro.bundle'),str(checkout)],check=True)
  subprocess.run(['git','-C',str(checkout),'checkout','--detach',pin['revision']],check=True)
 assert subprocess.check_output(['git','-C',str(checkout),'rev-parse','HEAD'],text=True).strip()==pin['revision']
 assert not subprocess.check_output(['git','-C',str(checkout),'status','--porcelain'],text=True).strip()
 for source,target in [('artifacts/0.8.0-dev/pc','.context/amiga/dosbox-pc'),('artifacts/0.8.0-dev/launcher','.context/amiga/dosbox-launcher')]:
  out=ROOT/target;out.mkdir(parents=True,exist_ok=True)
  for p in (ROOT/source).iterdir():
   q=out/p.name
   if not q.exists():shutil.copy2(p,q)
 for source,target in [('libc.a','.context/amiga/pocketsnes/libc.a'),('libc-seed.a','.context/amiga/ffmpeg-bench/libc-pic.a')]:
  p=ROOT/target;p.parent.mkdir(parents=True,exist_ok=True)
  if not p.exists():shutil.copy2(ROOT/'artifacts/build-support'/source,p)
 base=ROOT/'.context/amiga/ffmpeg-bench'
 if not (base/'picolibc-1.8.12').exists():
  with tarfile.open(ROOT/'sources/picolibc_1.8.12.orig.tar.gz') as archive:archive.extractall(base,filter='data')
 drive=ROOT/'.context/amiga/dosbox-shell/CDrive';drive.mkdir(parents=True,exist_ok=True)
 for name in ['Diagnostic.com','FileTest.com','ExeTest.exe','INPUT.TXT']:
  target=drive/name.upper()
  if not target.exists():shutil.copy2(ROOT/'artifacts/0.8.0-dev/pc'/name,target)
 print('Saved inputs restored; existing generated outputs were preserved.')
if __name__=='__main__':main()
