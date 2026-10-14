#!/usr/bin/env python3
"""Native ARM64 exact integer span, clear and complete-scene sanitizer tests."""
import argparse, subprocess
from prepare_dosbox_matrix import ROOT
from prepare_dosbox_tinygl_raster import prepare
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--compiler-profile',choices=['baseline','o3','o3-vector'],default='baseline')
p.add_argument("--fixed-mode",choices=["comparison","scalar","guarded","hybrid"],default="comparison")
a=p.parse_args()
out=ROOT/('.context/amiga/dosbox-tinygl-raster-native-'+a.compiler_profile+'-'+a.fixed_mode);out.mkdir(parents=True,exist_ok=True)
sources=prepare(out,a.fixed_mode)+list((ROOT/'amiga/dosbox/tinygl_raster').glob('*.c'))+[ROOT/'amiga/dosbox/tinygl_bench/profile.c',ROOT/'amiga/dosbox/matrix/neon.c',ROOT/'amiga/dosbox/matrix/scalar.c',ROOT/'amiga/dosbox/opengl/messages.c',ROOT/'tests/amiga/dosbox/tinygl_raster.c']
flags=['-std=c99','-O2','-g','-ffp-contract=off','-fno-vectorize','-fno-strict-aliasing','-fsanitize=address,undefined','-fno-sanitize-recover=all']
flags+=['-I'+str(p) for p in (ROOT/'amiga/dosbox/matrix',ROOT/'amiga/dosbox/tinygl_bench',ROOT/'amiga/dosbox/tinygl_raster',out/'tinygl/include',out/'tinygl/src')]
objects=[]
for i,src in enumerate(sources):
    extra=[]
    if a.compiler_profile!='baseline' and (src.name in ('ztriangle.c','zbuffer.c') or src.parent==ROOT/'amiga/dosbox/tinygl_raster' or src==ROOT/'tests/amiga/dosbox/tinygl_raster.c'):
        extra=['-O3']
        extra+=['-fvectorize','-fslp-vectorize'] if a.compiler_profile=='o3-vector' else ['-fno-vectorize','-fno-slp-vectorize']
    obj=out/(str(i)+'.o');objects.append(obj)
    subprocess.run(['clang',*flags,*extra,'-c',str(src),'-o',str(obj)],check=True)
subprocess.run(['clang',*flags,*map(str,objects),'-lm','-o',str(out/'test')],check=True)
subprocess.run([str(out/'test')],check=True)
