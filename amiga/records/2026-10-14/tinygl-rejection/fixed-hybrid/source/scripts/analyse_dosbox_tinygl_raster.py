#!/usr/bin/env python3
"""Analyse physical TinyGL raster runs; reject mixed report revisions."""
import argparse,json,re,statistics,zlib
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('logs',nargs='+',type=Path);p.add_argument('--output',type=Path);p.add_argument("--fixed-build",type=Path,help="Dedicated build: all four columns repeat one fixed kernel");a=p.parse_args()
names=['geometry','clipping','fill','occluded-fill','clear-zero','clear-arbitrary','clear-colour-only','mixed-fill'];methods=['baseline','revision2-neon','guarded-neon','scalar-prefix-neon'];runs=[];clock=333333333
revision=None
fixed_mode=None
if a.fixed_build:
    fixed_mode=json.loads((a.fixed_build/'build.json').read_text())['raster_fixed']
    assert fixed_mode in ('scalar','guarded','hybrid')
    methods=['repeat-'+str(i) for i in range(4)]
for log in a.logs:
    text=log.read_text()
    magic=re.search(r'RASTERBENCH magic=(5253543[12]) errors=0 count=([78]) quick=0',text)
    assert magic,(log,'missing successful raster report')
    current=int(magic[1][-1]);count=int(magic[2]);assert count==6+current
    if revision is None:
        revision=current
        if revision==1:methods=['baseline','spans','clear','both']
    assert current==revision,'Do not combine different experiment revisions'
    if fixed_mode:assert revision==2 
    for marker in ('SHELL PASS','EGA/PIT PASS','guest_exit=0','context_equal=1','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released'):
        assert marker in text,(log,marker)
    rows={}
    for m in re.finditer(r'RASTERBENCH case=(\d+) n=(\d+) repeats=(\d+) hash=([0-9a-f]+)((?: t\d+=[0-9a-f]+){4})',text):
        c,n,reps,h,raw=m.groups();c=int(c);reps=int(reps)
        ticks=[int(v,16) for v in re.findall(r't\d+=([0-9a-f]+)',raw)]
        assert len(ticks)==4 and all(ticks)
        rows[c]={'name':names[c],'triangles':int(n),'repeats':reps,'hash':h,'ticks':dict(zip(methods,ticks)),'us':dict(zip(methods,[v/clock*1e6/reps for v in ticks]))}
    assert len(rows)==count;runs.append({'log':str(log),'cases':rows})
summary=[]
for c in range(count):
    rows=[r['cases'][c] for r in runs];assert len({r['hash'] for r in rows})==1
    med={m:statistics.median(r['us'][m] for r in rows) for m in methods}
    summary.append({'name':names[c],'hash':rows[0]['hash'],'triangles':rows[0]['triangles'],'median_us':med,'range_us':{m:[min(r['us'][m] for r in rows),max(r['us'][m] for r in rows)] for m in methods},'throughput_gain_percent':{} if fixed_mode else {m:(med['baseline']/med[m]-1)*100 for m in methods[1:]},'time_reduction_percent':{} if fixed_mode else {m:(1-med[m]/med['baseline'])*100 for m in methods[1:]}})
result={'fixed_mode':fixed_mode,'build_crc32':f'{zlib.crc32((a.fixed_build/"ZZDosBox-Dynrec").read_bytes()):08X}' if fixed_mode else None,'report_revision':revision,'timer_hz':clock,'scope':'private 640x480 native TinyGL; scalar transforms; profiler disabled; no DOS game/display FPS inference','runs':runs,'summary':summary}
if a.output:a.output.write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(summary,indent=2))
