#!/usr/bin/env python3
"""Check GTA cutout textures in an already-built full RasterIX simulation."""
import argparse,hashlib,json,subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1];src=root/'amiga/dosbox/glide'
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--renderer',type=Path,required=True);a=p.parse_args();out=a.renderer.resolve()
record=json.loads((out/'results.json').read_text())
harness='tests/amiga/dosbox/glide/full_renderer.v'
assert record['build_exit']==0 and record['board_renderer']
assert record['harness_sha256'][harness]==hashlib.sha256((root/harness).read_bytes()).hexdigest()
fixture=out/'gta-fixtures'
subprocess.run(['clang','-std=c11','-O2','-I'+str(src),str(root/'tests/amiga/dosbox/glide/gta_texture_fixtures.c'),*[str(src/n) for n in ('probe_scene.c','translator.c','descriptor.c')],'-lm','-o',str(fixture)],check=True)
original={n:(out/n).read_bytes() for n in ('commands.hex','scene_sizes.vh')};cases=[]
try:
 for linear in (0,1):
  r=subprocess.run([str(fixture),str(linear)],check=True,capture_output=True,text=True)
  (out/'commands.hex').write_text(r.stdout)
  (out/'scene_sizes.vh').write_text('\n'.join(f'starts[{i}]={x.split()[0]};sizes[{i}]={x.split()[1]};' for i,x in enumerate(r.stderr.splitlines())))
  run=subprocess.run([str(out/'verilated/Vfull_renderer_test'),'+gta_alpha'],cwd=out,check=True,capture_output=True,text=True)
  assert run.stdout.count('PASS full renderer:')==5
  pixels=[int(x,16) for x in (out/'pixels-3.hex').read_text().split()]
  checks={str((x,y)):pixels[(71-y)*64+x] for x,y in ((16,16),(40,16),(2,16))}
  assert list(checks.values())==[0xffe0,0x001f,0],checks
  (out/f'gta-alpha-{linear}.log').write_text(run.stdout+run.stderr)
  cases.append({'linear':bool(linear),'pixels':checks})
  print('PASS full RasterIX GTA cutout:', 'bilinear' if linear else 'nearest', checks)
finally:
 for n,b in original.items():(out/n).write_bytes(b)
(out/'gta-alpha.json').write_text(json.dumps({'scope':'Full RTL with synthetic AXI RAM, not hardware FPS','cases':cases,'simulator_sha256':hashlib.sha256((out/'verilated/Vfull_renderer_test').read_bytes()).hexdigest()},indent=2)+'\n')
