#!/usr/bin/env python3
"""Offline diagnostics: linked ARM renderer, error unwinding and phase ring."""
import argparse
import json
from pathlib import Path
import check_pc_arm as pc

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--out', required=True, type=Path)
a = p.parse_args()
pc.OUT = a.out.resolve()
assert json.loads((pc.OUT/'build.json').read_text())['video_trace']
for mode, base, options, fixture in [
    ('adapter', 0x12000040, (8, 6, 3000, 1), ('HERCTEST', 720, 348, 8, 0xaa7f00)),
    ('adapter', 0x150020c0, (8, 3, 3000), ('S3V105', 1024, 768, 8, 0xffffff)),
    ('badnonce', 0x12000040, (8, 0, 3000), None),
    ('copy-stop', 0x12000040, (8, 0, 3000), None),
    ('resize', 0x12000040, (8, 0, 3000), None),
    ('scan-transition', 0x12000040, (8, 0, 3000), None),
    ('neon-denied', 0x12000040, (8, 0, 3000), None),
]:
    pc.run(base, mode, True, options=options, adapter=fixture)

for phase in (1, 2):
    for actions in (['next', 'continue'], ['stop']):
        pc.run(0x12000040, 'fixture', True, options=(8, 0, 3000),
               developer={'phase': phase, 'actions': actions, 'trace': actions != ['stop']})
