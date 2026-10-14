#!/usr/bin/env python3
"""Analyse physical, profiler-disabled TinyGL factorial raster/clear runs."""
import argparse,json,re,statistics
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('logs',nargs='+',type=Path);p.add_argument('--output',type=Path);a=p.parse_args()
names=['geometry','clipping','fill','occluded-fill','clear-zero','clear-arbitrary','clear-colour-only'];methods=['baseline','spans','clear','both'];runs=[];clock=333333333
for log in a.logs:
    text=log.read_text()
    for marker in ('RASTERBENCH magic=52535431 errors=0 count=7 quick=0','SHELL PASS','EGA/PIT PASS','guest_exit=0','context_equal=1','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released'):
        assert marker in text,(log,marker)
    rows={}
    for m in re.finditer(r'RASTERBENCH case=(\d+) n=(\d+) repeats=(\d+) hash=([0-9a-f]+)((?: t\d+=[0-9a-f]+){4})',text):
        c,n,reps,h,raw=m.groups();c=int(c);reps=int(reps)
        ticks=[int(v,16) for v in re.findall(r't\d+=([0-9a-f]+)',raw)]
        assert len(ticks)==4 and all(ticks)
        rows[c]={'name':names[c],'triangles':int(n),'repeats':reps,'hash':h,'ticks':dict(zip(methods,ticks)),'us':dict(zip(methods,[v/clock*1e6/reps for v in ticks]))}
    assert len(rows)==7;runs.append({'log':str(log),'cases':rows})
summary=[]
for c in range(7):
    rows=[r['cases'][c] for r in runs];assert len({r['hash'] for r in rows})==1
    med={m:statistics.median(r['us'][m] for r in rows) for m in methods}
    summary.append({'name':names[c],'hash':rows[0]['hash'],'triangles':rows[0]['triangles'],'median_us':med,'range_us':{m:[min(r['us'][m] for r in rows),max(r['us'][m] for r in rows)] for m in methods},'throughput_gain_percent':{m:(med['baseline']/med[m]-1)*100 for m in methods[1:]},'time_reduction_percent':{m:(1-med[m]/med['baseline'])*100 for m in methods[1:]}})
result={'timer_hz':clock,'scope':'private 640x480 native TinyGL; scalar transforms; profiler disabled; no DOS game/display FPS inference','runs':runs,'summary':summary}
if a.output:a.output.write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(summary,indent=2))
