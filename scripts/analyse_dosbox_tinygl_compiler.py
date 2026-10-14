#!/usr/bin/env python3
"""Compare validated raster result JSON across compiler profiles, like for like."""
import argparse
import json
from pathlib import Path

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--baseline', required=True, type=Path)
p.add_argument('--o3', required=True, type=Path)
p.add_argument('--vector', required=True, type=Path)
p.add_argument('--output', required=True, type=Path)
a = p.parse_args()
inputs = {'baseline': a.baseline, 'o3': a.o3, 'o3-vector': a.vector}
data = {name: json.loads(path.read_text()) for name, path in inputs.items()}
for name, result in data.items():
    assert result['timer_hz'] == 333333333 and len(result['summary']) == 7, name
    assert len(result['runs']) >= 3, (name, 'three physical runs required')
rows = []
for i, baseline in enumerate(data['baseline']['summary']):
    cases = {name: result['summary'][i] for name, result in data.items()}
    assert all(row['name'] == baseline['name'] and row['hash'] == baseline['hash']
               for row in cases.values()), 'Output differs between compiler profiles'
    row = {'name': baseline['name'], 'hash': baseline['hash'], 'methods': {}}
    for method, reference in baseline['median_us'].items():
        row['methods'][method] = {
            name: {'median_us': case['median_us'][method],
                   'range_us': case['range_us'][method],
                   'throughput_gain_percent': (reference / case['median_us'][method] - 1) * 100,
                   'time_reduction_percent': (1 - case['median_us'][method] / reference) * 100}
            for name, case in cases.items()}
    rows.append(row)
result = {'scope': 'Native offscreen TinyGL only; same rendering method across compiler builds; no DOS game FPS claim',
          'inputs': {name: str(path) for name, path in inputs.items()}, 'cases': rows}
a.output.write_text(json.dumps(result, indent=2) + '\n')
for row in rows:
    for method in ('baseline', 'both'):
        print(row['name'], method, ' | '.join(
            f"{name}: {v['median_us']/1000:.3f} ms ({v['throughput_gain_percent']:+.2f}%)"
            for name, v in row['methods'][method].items()))
