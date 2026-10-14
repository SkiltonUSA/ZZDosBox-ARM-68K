#!/usr/bin/env python3
"""Native ARM64 exact integer span, clear and complete-scene sanitizer tests."""
import subprocess
from prepare_dosbox_matrix import ROOT
from prepare_dosbox_tinygl_raster import prepare
out=ROOT/'.context/amiga/dosbox-tinygl-raster-native';out.mkdir(parents=True,exist_ok=True)
sources=prepare(out)+list((ROOT/'amiga/dosbox/tinygl_raster').glob('*.c'))+[ROOT/'amiga/dosbox/tinygl_bench/profile.c',ROOT/'amiga/dosbox/matrix/neon.c',ROOT/'amiga/dosbox/matrix/scalar.c',ROOT/'amiga/dosbox/opengl/messages.c',ROOT/'tests/amiga/dosbox/tinygl_raster.c']
flags=['-std=c99','-O2','-g','-ffp-contract=off','-fno-vectorize','-fno-strict-aliasing','-fsanitize=address,undefined','-fno-sanitize-recover=all']
flags+=['-I'+str(p) for p in (ROOT/'amiga/dosbox/matrix',ROOT/'amiga/dosbox/tinygl_bench',ROOT/'amiga/dosbox/tinygl_raster',out/'tinygl/include',out/'tinygl/src')]
subprocess.run(['clang',*flags,*map(str,sources),'-lm','-o',str(out/'test')],check=True)
subprocess.run([str(out/'test')],check=True)
