#!/usr/bin/env python3
"""Differential linked-image scheduler checks; not physical speed measurements."""
import argparse,json
from pathlib import Path
import check_pc_arm as pc

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--baseline',required=True,type=Path)
p.add_argument('--candidate',required=True,type=Path)
p.add_argument('--record',required=True,type=Path)
a=p.parse_args()
assert not json.loads((a.baseline/'build.json').read_text()).get('pic_vfp')
assert json.loads((a.candidate/'build.json').read_text())['pic_vfp']
results=[]
for n,(mode,cycles) in enumerate([('fixture',500),('fixture',3000),('fixture',10000),('speed',6000)]):
 pair=[]
 for out in [a.baseline,a.candidate]:
  pc.OUT=out.resolve();trace={}
  result=pc.run(0x12000040 if n%2==0 else 0x150020c0,mode,True,
                options=(8,0,cycles),scheduler_trace=trace,
                inherited_fpscr=0x03c00000)
  assert trace['calls']>100,trace
  result['output']=result['output'].decode('ascii')
  pair.append(dict(scheduler=trace,result=result))
 assert pair[0]==pair[1],(mode,cycles,pair)
 results.append(dict(mode=mode,cycles=cycles,**pair[0]))
 print('PASS identical PIC scheduling, guest output/time and pixels:',mode,cycles,flush=True)
 a.record.write_text(json.dumps({'differential':results,'remaining':'lifecycle checks pending'},indent=2)+'\n')
pc.OUT=a.candidate.resolve()
for mode in ('smc','paging','boot','copy-stop','fs-stop','neon-denied'):
 pc.run(0x150020c0,mode,True,options=(8,0,3000),inherited_fpscr=0x03c00000)
a.record.write_text(json.dumps({'differential':results,'lifecycle':'PASS smc/paging/boot/copy-stop/fs-stop/neon-denied','hardware':'not measured by this test'},indent=2)+'\n')
