#!/usr/bin/env python3
"""Native ARM64 sanitizers and internal TinyGL equivalence, no timing claims."""
import subprocess
from prepare_dosbox_matrix import ROOT
from prepare_dosbox_tinygl_bench import prepare
out=ROOT/'.context/amiga/dosbox-tinygl-native';out.mkdir(parents=True,exist_ok=True)
sources=prepare(out)+list((ROOT/'amiga/dosbox/tinygl_bench').glob('*.c'))+[ROOT/'amiga/dosbox/matrix/neon.c',ROOT/'amiga/dosbox/matrix/scalar.c',ROOT/'amiga/dosbox/opengl/messages.c',ROOT/'tests/amiga/dosbox/tinygl_internal.c']
flags=['-std=c99','-O2','-g','-ffp-contract=off','-fno-vectorize','-fsanitize=address,undefined','-fno-sanitize-recover=all']
flags+=['-I'+str(p) for p in (ROOT/'amiga/dosbox/matrix',ROOT/'amiga/dosbox/tinygl_bench',out/'tinygl/include',out/'tinygl/src')]
subprocess.run(['clang',*flags,*map(str,sources),'-lm','-o',str(out/'test')],check=True)
subprocess.run([str(out/'test')],check=True)
