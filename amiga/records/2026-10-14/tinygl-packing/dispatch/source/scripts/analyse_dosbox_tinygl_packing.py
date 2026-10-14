#!/usr/bin/env python3
"""Compare four dedicated physical builds of the textured RST3 scene suite."""
import argparse,json,re,statistics
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--root',required=True,type=Path);p.add_argument('--output',required=True,type=Path);a=p.parse_args()
modes=('scalar','hybrid','unguarded','dispatch');data={};objects={};reference={}
for mode in modes:
    root=a.root/mode;build=json.loads((root/'build.json').read_text())
    assert build['raster_fixed']==mode and build['raster_compiler']=='baseline'
    data[mode]=json.loads((root/'results.json').read_text())
    assert data[mode]['fixed_mode']==mode and data[mode]['report_revision']==3
    assert data[mode]['build_crc32']==json.loads((root/'binary.json').read_text())['crc32']
    assert len(data[mode]['runs'])==3
    objects[mode]=json.loads((root/'object-hashes.json').read_text())
    reference[mode]={int(c):h for c,h in re.findall(r'SCENE flags=3 kind=(\d+) hash=([0-9a-f]+)',(root/'native.log').read_text())}
    assert len(reference[mode])==8
for mode in modes[1:]:
    assert objects[mode].keys()==objects['scalar'].keys()
    changed=[n for n in objects[mode] if objects[mode][n]!=objects['scalar'][n]]
    assert len(changed)==1 and changed[0].endswith('-ztriangle.arm.o'),(mode,changed)
summary=[]
for c in range(8):
    times={};hashes=set();reps=set()
    for mode in modes:
        times[mode]=[]
        for run in data[mode]['runs']:
            row=run['cases'][str(c)];assert len(row['us'])==4
            times[mode].append(sum(row['us'].values())/4)
            assert row['hash']==reference[mode][c],(mode,c,'physical/native image mismatch')
            hashes.add(row['hash']);reps.add(row['repeats'])
    assert len(hashes)==len(reps)==1
    med={m:statistics.median(v) for m,v in times.items()}
    summary.append({'name':row['name'],'hash':next(iter(hashes)),'median_us':med,'per_run_us':times,
        'throughput_gain_percent':{m:(med['scalar']/med[m]-1)*100 for m in modes[1:]}})
result={'scope':'Native private-buffer TinyGL scenes; no DOS game or display FPS claim','method':'Four equal batches averaged per run, median of three physical runs per build','summary':summary}
a.output.write_text(json.dumps(result,indent=2)+'\n')
for c in summary:print(c['name'],{m:round(t/1000,3) for m,t in c['median_us'].items()},c['throughput_gain_percent'])
