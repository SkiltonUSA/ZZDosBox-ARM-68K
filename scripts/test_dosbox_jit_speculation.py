#!/usr/bin/env python3
"""Run the ARM JIT lookahead and self-modifying-code regressions at two bases."""
import argparse
import importlib.util
from pathlib import Path

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('build', type=Path, help='Built Expanded ARM runtime directory')
args = parser.parse_args()
spec = importlib.util.spec_from_file_location('model', root / 'tests/amiga/dosbox/check_pc_arm.py')
model = importlib.util.module_from_spec(spec)
spec.loader.exec_module(model)
model.OUT = args.build.resolve()
for base in (0x12000040, 0x150020c0):
    for name, message in (
        ('JITSPEC', 'JIT speculative invalid bytes PASS'),
        ('SMCTEST', 'JIT self-modifying guest code PASS'),
    ):
        model.run(base, 'fixture', hardware={'cpu': 1}, fixture=(name, message))
