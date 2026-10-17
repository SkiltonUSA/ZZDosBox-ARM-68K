#!/usr/bin/env python3
"""Sanitizer checks for bounded DOS file read-ahead and mutation coherence."""
import subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
out=root/'.context/amiga/dosbox-file-tests';out.mkdir(parents=True,exist_ok=True)
binary=out/'read-ahead'
subprocess.run(['clang++','-std=c++17','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(root/'amiga/dosbox/shell'),str(root/'tests/amiga/dosbox/file_read_ahead.cpp'),'-o',str(binary)],check=True)
subprocess.run([str(binary)],check=True)
