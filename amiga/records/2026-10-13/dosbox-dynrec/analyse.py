#!/usr/bin/env python3
"""Validate this physical dynrec run and calculate real-time Doom throughput."""
import json
from pathlib import Path
import re

HERE = Path(__file__).resolve().parent
s = (HERE / 'physical-doom.log').read_text()
game, real = map(int, re.search(r'timed (\d+) gametics in (\d+) realtics', s).groups())
fields = dict(re.findall(r'(\w+)=([0-9a-f]+)', re.search(r'DOOMCLOCK .*', s).group()))
jit = dict(re.findall(r'(\w+)=([0-9a-f]+)', re.search(r'JIT .*', s).group()))
assert game == 2134 and real > 0
assert fields['magic'] == '44424d31' and fields['signature'] == '1' and fields['rejected'] == '0'
assert int(fields['first']) in (0, 1) and int(fields['last']) == game
assert jit['magic'] == '4a495431' and jit['witness'] == '1'
assert int(jit['dispatches']) > 0 and int(jit['commits']) > 3
for expected in ('SHELL result=0 guest_exit=0', 'SHELL PASS',
                 'register_mismatches=00000000', 'witness_bad=0',
                 'LAUNCHER exit=0; owned memory released'):
    assert expected in s, expected
seconds = (int(fields['end'], 16) - int(fields['start'], 16)) / 333333333
assert seconds > 0
fps = (int(fields['last']) - int(fields['first'])) / seconds
baseline = json.loads((HERE.parent / 'dosbox-doom/result.json').read_text())
result = {
    'gametics': game,
    'guest_realtics': real,
    'guest_clock_fps_not_hardware_score': 35 * game / real,
    'a9_timer_hz_nominal': 333333333,
    'observer': fields,
    'jit': jit,
    'measured_seconds_approx': seconds,
    'measured_game_fps_approx': fps,
    'interpreter_game_fps_approx': baseline['measured_game_fps_approx'],
    'speed_ratio_approx': fps / baseline['measured_game_fps_approx'],
    'conditions': dict(baseline['conditions'], cpu_core='ARMv7 dynamic recompiler'),
    'endpoint_limit': baseline['endpoint_limit'],
    'clean_teardown': True,
}
(HERE / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
