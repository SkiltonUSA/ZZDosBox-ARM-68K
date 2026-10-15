#!/usr/bin/env python3
"""Physical A9 internal TinyGL times; profiled breakdown is explicitly perturbed."""
import argparse,json,re,statistics
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('logs',nargs='+',type=Path);p.add_argument('--output',type=Path);p.add_argument('--baseline',choices=['scalar','neon'],default='scalar');p.add_argument('--candidate',choices=['neon','hlsl'],default='neon');a=p.parse_args();methods=[a.baseline,a.candidate];assert methods[0]!=methods[1]
clock=333333333.0;names=['geometry','clipping','fill'];stages=['transform','clip','raster','clear','other'];runs=[]
for log in a.logs:
    text=log.read_text()
    if a.candidate=="hlsl":assert "HLSLPP mode="+a.baseline+" revision=" in text,log
    for marker in ('TGLBENCH magic=54474c31 errors=0 count=3 quick=0','context_equal=1','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released','SHELL PASS','EGA/PIT PASS'):
        assert marker in text,(log,marker)
    rows={}
    for m in re.finditer(r'TGLBENCH case=(\d+) n=(\d+) repeats=(\d+) hash=([0-9a-f]+) '+methods[0]+r'=([0-9a-f]+) '+methods[1]+r'=([0-9a-f]+)',text):
        c,n,reps,h,s,v=m.groups();c=int(c);reps=int(reps)
        rows[c]={'name':names[c],'triangles':int(n),'repeats':reps,'hash':h,methods[0]+'_us':int(s,16)/clock*1e6/reps,methods[1]+'_us':int(v,16)/clock*1e6/reps,'profile':{}}
    for m in re.finditer(r'TGLPROFILE case=(\d+) method=(\d+)((?: t\d+=[0-9a-f]+){6})',text):
        c,method,raw=m.groups();values=[int(x,16) for x in re.findall(r't\d+=([0-9a-f]+)',raw)]
        assert values[0]==sum(values[1:]) and all(values),values
        row=rows[int(c)];name=methods[int(method)]
        row['profile'][name]={'wall_us':values[0]/clock*1e6,'stages_us':dict(zip(stages,[v/clock*1e6 for v in values[1:]])),'shares_percent':dict(zip(stages,[v/values[0]*100 for v in values[1:]])),'instrumentation_slowdown':values[0]/clock*1e6/row[name+'_us']}
    assert len(rows)==3 and all(len(r['profile'])==2 for r in rows.values())
    runs.append({'log':str(log),'cases':rows})
summary=[]
for c in range(3):
    rows=[run['cases'][c] for run in runs];assert len({r['hash'] for r in rows})==1
    scalar=statistics.median(r[methods[0]+'_us'] for r in rows);neon=statistics.median(r[methods[1]+'_us'] for r in rows)
    summary.append({'name':names[c],'triangles':rows[0]['triangles'],'hash':rows[0]['hash'],methods[0]+'_us':scalar,methods[1]+'_us':neon,'throughput_gain_percent':(scalar/neon-1)*100,'time_reduction_percent':(1-neon/scalar)*100,methods[0]+'_range_us':[min(r[methods[0]+'_us'] for r in rows),max(r[methods[0]+'_us'] for r in rows)],methods[1]+'_range_us':[min(r[methods[1]+'_us'] for r in rows),max(r[methods[1]+'_us'] for r in rows)],'profile_median_us':{m:{stage:statistics.median(r['profile'][m]['stages_us'][stage] for r in rows) for stage in stages} for m in methods},'profile_slowdown':{m:statistics.median(r['profile'][m]['instrumentation_slowdown'] for r in rows) for m in methods}})
result={'methods':methods,'timer_hz':clock,'scope':'private 640x480 offscreen native TinyGL, no DOS/game/scanout FPS claim','profiling_caveat':'Exclusive stages include timer/hook overhead; totals are measured separately with profiling disabled. Raw stage percentages are not unbiased production shares.','runs':runs,'summary':summary}
text=json.dumps(result,indent=2)+'\n'
if a.output:a.output.write_text(text)
print(json.dumps(summary,indent=2))
