#!/usr/bin/env python3
"""Compare dedicated scalar/hybrid builds; four report columns repeat one kernel."""
import argparse,json,statistics
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--scalar',required=True,type=Path)
p.add_argument('--hybrid',required=True,type=Path)
p.add_argument('--scalar-build',required=True,type=Path)
p.add_argument('--hybrid-build',required=True,type=Path)
p.add_argument('--output',required=True,type=Path)
a=p.parse_args()
data={};objects={}
for mode in ('scalar','hybrid'):
    build=getattr(a,mode+'_build')
    metadata=json.loads((build/'build.json').read_text())
    assert metadata['raster_fixed']==mode and metadata['raster_compiler']=='baseline'
    assert metadata['tinygl']['raster_experiment']['fixed_mode']==mode
    data[mode]=json.loads(getattr(a,mode).read_text())
    assert data[mode]['fixed_mode']==mode
    assert data[mode]['build_crc32']==json.loads((build/'binary.json').read_text())['crc32']
    assert data[mode]['report_revision']==2 and len(data[mode]['runs'])==3
    objects[mode]=json.loads((build/'object-hashes.json').read_text())
assert objects['scalar'].keys()==objects['hybrid'].keys()
changed=[n for n in objects['scalar'] if objects['scalar'][n]!=objects['hybrid'][n]]
assert len(changed)==1 and changed[0].endswith('-ztriangle.arm.o'),changed
summary=[]
for c in range(8):
    samples={};hashes=set();counts=set();reps=set()
    for mode in ('scalar','hybrid'):
        samples[mode]=[]
        for run in data[mode]['runs']:
            row=run['cases'][str(c)]
            assert len(row['us'])==4
            samples[mode].append(sum(row['us'].values())/4)
            hashes.add(row['hash']);counts.add(row['triangles']);reps.add(row['repeats'])
    assert len(hashes)==len(counts)==len(reps)==1
    med={m:statistics.median(v) for m,v in samples.items()}
    summary.append({'name':row['name'],'hash':next(iter(hashes)),
        'median_us':med,'per_run_us':samples,
        'throughput_gain_percent':(med['scalar']/med['hybrid']-1)*100,
        'time_reduction_percent':(1-med['hybrid']/med['scalar'])*100})
result={'scope':'Dedicated private 640x480 native TinyGL builds, original clear, O2; no game/display FPS claim',
        'method':'Mean of four equal-repeat columns per run, then median of three physical runs per build',
        'changed_objects':changed,'summary':summary}
a.output.write_text(json.dumps(result,indent=2)+'\n')
for row in summary:
    print(row['name'],{k:round(v/1000,3) for k,v in row['median_us'].items()},round(row['throughput_gain_percent'],2),'% throughput')
