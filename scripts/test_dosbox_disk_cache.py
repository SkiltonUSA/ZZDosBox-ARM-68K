#!/usr/bin/env python3
"""Check every supported sector-cache capacity against authoritative disk bytes."""
import subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'.context/amiga/dosbox-cache-tests';out.mkdir(parents=True,exist_ok=True)
for kib in (128,256,512,1024,2048):
 binary=out/('cache-'+str(kib))
 subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DDB_DISK_CACHE_SETS='+str(kib//16),'-I'+str(root/'amiga/dosbox/shell'),str(root/'tests/amiga/dosbox/disk_cache.cpp'),'-o',str(binary)],check=True)
 subprocess.run([str(binary)],check=True)
 print('PASS cache capacity',kib,'KiB',flush=True)
