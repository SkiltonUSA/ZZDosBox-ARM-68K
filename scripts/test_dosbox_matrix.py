#!/usr/bin/env python3
"""Host ARM64 sanitizer tests; physical timings require the A9 diagnostic build."""
import subprocess
from prepare_dosbox_matrix import ROOT,prepare,prepare_scene
out=ROOT/'.context/amiga/dosbox-matrix-native';out.mkdir(parents=True,exist_ok=True)
sources=prepare(out)+prepare_scene(out)
sources+=list((ROOT/'amiga/dosbox/matrix').glob('*.c'))+[ROOT/'amiga/dosbox/opengl/messages.c',ROOT/'tests/amiga/dosbox/matrix.c']
flags=['-std=c99','-O2','-g','-ffp-contract=off','-fsanitize=address,undefined','-fno-sanitize-recover=all','-DARM_MATH_NEON','-DARM_MATH_MATRIX_CHECK','-D__GNUC_PYTHON__','-DARM_MATH_L1_CACHE_SIZE=16384','-DARM_MATH_L2_CACHE_SIZE=32768','-DARM_MATH_L3_CACHE_SIZE=32768']
flags+=['-I'+str(p) for p in (ROOT/'amiga/dosbox/matrix',out/'cmsis-dsp/Include',out/'cmsis-dsp/PrivateInclude',out/'tinygl/include')]
subprocess.run(['clang',*flags,*map(str,sources),'-lm','-o',str(out/'test')],check=True)
subprocess.run([str(out/'test')],check=True)
