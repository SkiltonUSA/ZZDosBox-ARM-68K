#!/usr/bin/env python3
"""Bounded guest ABI tests. No Amiga connection or supplied game required."""
import subprocess,sys
from pathlib import Path
import build_dosbox_glide_ovl as ovl
root=Path(__file__).resolve().parents[1];out=ovl.OUT;out.mkdir(parents=True,exist_ok=True)
subprocess.run([sys.executable,str(root/'scripts/build_dosbox_glide_ovl.py'),'--generate-only'],check=True)
subprocess.run([sys.executable,str(root/'tests/amiga/dosbox/glide/test_ovl.py')],check=True)
subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',
 '-I'+str(root/'amiga/dosbox/glide'),'-I'+str(out),str(root/'tests/amiga/dosbox/glide/guest.c'),
 str(root/'amiga/dosbox/glide/guest.c'),'-o',str(out/'guest-test')],check=True)
subprocess.run([str(out/'guest-test')],check=True)
subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',
 '-I'+str(root/'amiga/dosbox/glide'),str(root/'tests/amiga/dosbox/glide/vertices.c'),
 str(root/'amiga/dosbox/glide/guest_vertex.c'),str(root/'amiga/dosbox/glide/clip.c'),
 '-o',str(out/'vertices-test')],check=True)
subprocess.run([str(out/'vertices-test')],check=True)
