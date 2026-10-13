#!/usr/bin/env python3
"""Summarise optional ZZDosBox ARM timings; ticks are not a CPU clock rating."""
import argparse
import json
import re
from pathlib import Path


def analyse(path):
    text = path.read_text()
    match = re.search(r'PROFILE total=([0-9a-f]{16}) unwound=(\d+)', text)
    if not match or 'PROFILE invalid' in text:
        raise ValueError(f'{path}: missing or invalid profile')
    total = int(match[1], 16)
    if not total:
        raise ValueError(f'{path}: timer did not advance')
    buckets = {}
    for name, ticks, calls in re.findall(
        r'PROFILE bucket=(\w+) ticks=([0-9a-f]{16}) calls=(\d+)', text
    ):
        if name in buckets:
            raise ValueError(f'{path}: duplicate bucket {name}')
        ticks = int(ticks, 16)
        buckets[name] = {'ticks': ticks, 'percent': round(ticks * 100 / total, 3),
                         'calls': int(calls)}
    if set(buckets) != {'other', 'cpu', 'events', 'capture', 'grant', 'copy'}:
        raise ValueError(f'{path}: incomplete buckets')
    if sum(b['ticks'] for b in buckets.values()) != total:
        raise ValueError(f'{path}: exclusive times do not sum to total')
    result = {'log': path.name, 'total_ticks': total,
              'unwound_scopes': int(match[2]), 'buckets': buckets}
    run = re.search(r'PC frames=(\d+) emulated_ms=(\d+) wall_ms=(\d+)', text)
    if run:
        frames, emulated, wall = map(int, run.groups())
        result.update(frames=frames, emulated_ms=emulated, wall_ms=wall,
                      startup_fps=frames * 1000 / wall if wall else None)
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('logs', type=Path, nargs='+')
    args = parser.parse_args()
    print(json.dumps([analyse(path) for path in args.logs], indent=2))
