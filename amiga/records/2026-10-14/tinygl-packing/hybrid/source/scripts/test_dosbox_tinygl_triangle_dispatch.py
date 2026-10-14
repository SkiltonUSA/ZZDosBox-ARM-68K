#!/usr/bin/env python3
"""Host-only dispatch coverage, never included in timed ARM builds."""
import subprocess
from prepare_dosbox_matrix import ROOT
from prepare_dosbox_tinygl_raster import prepare
out=ROOT/'.context/amiga/tinygl-followup/dispatch-coverage';out.mkdir(parents=True,exist_ok=True)
sources=prepare(out,'dispatch')
p=out/'tinygl/src/ztriangle.c';s=p.read_text();s='unsigned zz_small_calls,zz_large_calls;\n'+s
for dt in range(2):
 for dw in range(2):
  n=f'ZB_fillTriangleSmoothNOBLEND_DT{dt}_DW{dw}'
  old=f'if(maxx-minx<32 || maxy-miny<8 || area<(int64_t)32*(maxy-miny)){n}_scalar(zb,p0,p1,p2);\n    else {n}_fast(zb,p0,p1,p2);'
  new=f'if(maxx-minx<32 || maxy-miny<8 || area<(int64_t)32*(maxy-miny)){{zz_small_calls++;{n}_scalar(zb,p0,p1,p2);}}\n    else {{zz_large_calls++;{n}_fast(zb,p0,p1,p2);}}'
  assert old in s;s=s.replace(old,new)
p.write_text(s)
test=out/'test.c';test.write_text('''#include "experiment.h"
#include "raster.h"
#include <assert.h>
#include <stdio.h>
extern void zz_tgl_scene_prepare(unsigned,unsigned);
extern unsigned zz_small_calls,zz_large_calls;
uint64_t zz_tgl_clock(void){return 0;}
int main(void){
 assert(zz_matrix_scene_open());
 for(unsigned c=0;c<8;c++){
  unsigned n=c==0?1024:c==1?128:32;zz_small_calls=zz_large_calls=0;
  zz_tgl_scene_prepare(c,n);zz_matrix_scene_draw(c,n);
  printf("case=%u scalar=%u neon=%u hash=%08x\\n",c,zz_small_calls,zz_large_calls,zz_matrix_scene_hash());
  if(c==0)assert(zz_small_calls==1024&&!zz_large_calls);
  if(c==1)assert(zz_small_calls>zz_large_calls && zz_small_calls+zz_large_calls==256);
  if(c==2||c==3||c==7)assert(zz_large_calls);
  if(c==4||c==5)assert(!zz_small_calls&&!zz_large_calls);
 }
 zz_matrix_scene_close();return 0;
}
''')
sources += [ROOT/p for p in ('amiga/dosbox/tinygl_raster/clear.c','amiga/dosbox/tinygl_bench/profile.c','amiga/dosbox/matrix/neon.c','amiga/dosbox/matrix/scalar.c','amiga/dosbox/opengl/messages.c')]+[test]
flags=['-std=c99','-O2','-ffp-contract=off','-fno-vectorize','-fno-strict-aliasing','-fsanitize=address,undefined','-fno-sanitize-recover=all']
flags+=['-I'+str(p) for p in (ROOT/'amiga/dosbox/matrix',ROOT/'amiga/dosbox/tinygl_bench',ROOT/'amiga/dosbox/tinygl_raster',out/'tinygl/include',out/'tinygl/src')]
subprocess.run(['clang',*flags,*map(str,sources),'-lm','-o',str(out/'test')],check=True)
subprocess.run([str(out/'test')],check=True)
