#!/usr/bin/env python3
"""Check the experimental ARM checked-read register ABI in the linked model."""
import argparse
import json
import importlib.util
from pathlib import Path

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('build', type=Path)
args = parser.parse_args()
spec = importlib.util.spec_from_file_location('model', root / 'tests/amiga/dosbox/check_pc_arm.py')
model = importlib.util.module_from_spec(spec)
spec.loader.exec_module(model)
model.OUT = args.build.resolve()
if json.loads((model.OUT / 'build.json').read_text()).get('jit_reuse'):
    assert (model.OUT / 'JITREUSE.COM').exists(), 'Build the forwarding integration fixture'
for base in (0x12000040, 0x150020c0):
    model.run(base, 'fixture', True, options=(8, 0, 10000), hardware={'cpu': 1},
              fixture=('JITREAD', 'JIT checked reads PASS'))
    model.run(base, 'fixture', True, options=(8, 0, 10000), hardware={'cpu': 1})
    if (model.OUT / 'JITREUSE.COM').exists():
        model.run(base, 'fixture', True, options=(8, 0, 10000), hardware={'cpu': 1},
                  fixture=('JITREUSE', 'JIT register reuse PASS'))
for mode in ('cpu', 'smc', 'paging', 'high-bank-bound'):
    model.run(0x12000040, mode, True, options=(8, 0, 10000), hardware={'cpu': 1})
