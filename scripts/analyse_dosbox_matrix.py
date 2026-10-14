#!/usr/bin/env python3
"""Summarize closed physical matrix logs; offscreen scenes are not game FPS."""
import argparse,json,re,statistics
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('logs',nargs='+',type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
labels=['4x4 matrix product','16 vertex transforms','256 vertex transforms','1024 vertex transforms','128 triangles, 640x480 offscreen','1024 triangles, 640x480 offscreen']
pattern=re.compile(r'MATRIXBENCH case=(\d+) n=(\d+) repeats=(\d+) hash=([0-9a-f]+) scalar=([0-9a-f]+) cmsis=([0-9a-f]+) neon=([0-9a-f]+)')
records=[]
for path in a.logs:
    text=path.read_text()
    for marker in ('MATRIXBENCH magic=4d415431 errors=0 count=6 quick=0','context_equal=1','witness_bad=0','register_mismatches=00000000','LAUNCHER exit=0; owned memory released','EGA/PIT PASS'):
        assert marker in text,(path,marker)
    rows=[]
    for m in pattern.finditer(text):
        case,n,reps=map(int,m.group(1,2,3));times=dict(zip(('scalar','cmsis','neon'),[int(v,16) for v in m.group(5,6,7)]))
        rows.append(dict(case=case,n=n,repeats=reps,hash=m[4],ticks=times,us={k:v*1e6/(333333333*reps) for k,v in times.items()}))
    assert len(rows)==6 and [r['case'] for r in rows]==list(range(6))
    records.append(dict(log=path.name,cases=rows))
summary=[]
for i,label in enumerate(labels):
    rows=[r['cases'][i] for r in records]
    assert len({r['hash'] for r in rows})==1,'Image/output changed between runs'
    median={k:statistics.median(r['us'][k] for r in rows) for k in ('scalar','cmsis','neon')}
    gain={k:100*(median['scalar']/median[k]-1) for k in ('cmsis','neon')}
    summary.append(dict(case=i,label=label,median_us=median,throughput_gain_percent=gain,hash=rows[0]['hash'],range_us={k:[min(r['us'][k] for r in rows),max(r['us'][k] for r in rows)] for k in median}))
    print(f"{label}: scalar {median['scalar']:.3f} us, CMSIS {median['cmsis']:.3f} us ({gain['cmsis']:+.2f}%), specialised NEON {median['neon']:.3f} us ({gain['neon']:+.2f}%)")
a.output.write_text(json.dumps(dict(timer_hz=333333333,scope='Cached private RAM; offscreen scene includes transform, clear, TinyGL submission, clipping/depth/rasterization; excludes XACP presentation/scanout. No DOS game FPS claim.',runs=records,summary=summary),indent=2)+'\n')
