#!/usr/bin/env python3
"""Host-only instrumentation proves the rejection workloads exercise their labels.
The instrumented header exists only in this separate native build directory.
Never use these counters in a timed ARM executable.
"""
import subprocess
from prepare_dosbox_matrix import ROOT
from prepare_dosbox_tinygl_raster import prepare
out=ROOT/'.context/amiga/tinygl-rejection/coverage'
out.mkdir(parents=True,exist_ok=True)
sources=prepare(out)
header=(ROOT/'amiga/dosbox/tinygl_raster/span.h').read_text()
header=header.replace('#include <arm_neon.h>','#include <arm_neon.h>\nextern unsigned long long coverage[3];')
header=header.replace('        if(bits){', '        if(dt)coverage[!bits?0:bits==~(uint64_t)0?1:2]++;\n        if(bits){',1)
(out/'coverage-span.h').write_text(header)
p=out/'tinygl/src/ztriangle.c';p.write_text(p.read_text().replace('#include "span.h"','#include "coverage-span.h"',1))
test=out/'coverage.c';test.write_text('''#include "experiment.h"
#include "raster.h"
#include <stdio.h>
#include <assert.h>
unsigned long long coverage[3];
extern void zz_tgl_scene_prepare(unsigned,unsigned);
uint64_t zz_tgl_clock(void){return 0;}
int main(void){
 assert(zz_matrix_scene_open());zz_tgl_raster=1;zz_tgl_clear=0;
 const unsigned cases[]={2,3,7};
 for(unsigned i=0;i<3;i++){
  for(unsigned j=0;j<3;j++)coverage[j]=0;
  zz_tgl_scene_prepare(cases[i],32);zz_matrix_scene_draw(cases[i],32);
  printf("case=%u rejected=%llu visible=%llu mixed=%llu hash=%08x\\n",cases[i],coverage[0],coverage[1],coverage[2],zz_matrix_scene_hash());
  assert(coverage[1]);
  if(i==1)assert(coverage[0]>coverage[1]);
  if(i==2)assert(coverage[0]&&coverage[2]);
 }
 zz_matrix_scene_close();return 0;
}
''')
sources += [ROOT/p for p in ('amiga/dosbox/tinygl_raster/clear.c','amiga/dosbox/tinygl_bench/profile.c','amiga/dosbox/matrix/neon.c','amiga/dosbox/matrix/scalar.c','amiga/dosbox/opengl/messages.c')]+[test]
flags=['-std=c99','-O2','-ffp-contract=off','-fno-vectorize','-fno-strict-aliasing','-fsanitize=address,undefined','-fno-sanitize-recover=all']
flags+=['-I'+str(p) for p in (out,ROOT/'amiga/dosbox/matrix',ROOT/'amiga/dosbox/tinygl_bench',ROOT/'amiga/dosbox/tinygl_raster',out/'tinygl/include',out/'tinygl/src')]
subprocess.run(['clang',*flags,*map(str,sources),'-lm','-o',str(out/'test')],check=True)
subprocess.run([str(out/'test')],check=True)
