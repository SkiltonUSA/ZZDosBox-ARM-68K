#!/usr/bin/env python3
"""Native sanitizers for hlsl++ kernels and three-method TinyGL output."""
import subprocess
from prepare_dosbox_hlslpp import ROOT, BASE, prepare
out=ROOT/'.context/amiga/hlslpp-native';out.mkdir(parents=True,exist_ok=True)
sources=prepare(out,'scalar')
common=['-O2','-g','-ffp-contract=off','-fno-vectorize','-fsanitize=address,undefined','-fno-sanitize-recover=all']
common+=['-I'+str(p) for p in (out,BASE/'matrix',BASE/'tinygl_bench',out/'tinygl/include',out/'tinygl/src',out/'hlslpp/include')]
def build(sources,name):
    objects=[]
    for i,src in enumerate(sources):
        cpp=src.suffix=='.cpp';obj=out/(name+str(i)+'.o');objects.append(obj)
        subprocess.run(['clang++' if cpp else 'clang','-std=c++17' if cpp else '-std=c99',*common,'-c',str(src),'-o',str(obj)],check=True)
    binary=out/name
    subprocess.run(['clang++',*common,*map(str,objects),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
build([BASE/'matrix/scalar.c',BASE/'matrix/neon.c',BASE/'hlslpp/kernels.cpp',ROOT/'tests/amiga/dosbox/hlslpp.cpp'],'kernels-test')
text=(ROOT/'tests/amiga/dosbox/tinygl_internal.c').read_text()
text=text.replace('m<2','m<3').replace('zz_tgl_neon=1-m','zz_tgl_neon=(m+1)%3')
(out/'internal-test.c').write_text(text)
build([p for p in sources if p.name!='hlsl-bench.cpp']+[out/'internal-test.c'],'internal-test')
