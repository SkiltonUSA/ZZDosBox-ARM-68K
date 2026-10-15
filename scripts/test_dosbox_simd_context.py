#!/usr/bin/env python3
"""Exercise real PIT library math and SIMD-state isolation in linked ARM builds."""
import argparse,importlib.util
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('build',type=Path);a=p.parse_args()
s=importlib.util.spec_from_file_location('pc_check',root/'tests/amiga/dosbox/check_pc_arm.py')
m=importlib.util.module_from_spec(s);s.loader.exec_module(m);m.OUT=a.build.resolve()
# Nondefault rounding, FTZ/DN and writable exception controls must not leak into math.
m.run(0x12000040,'pit-math',options=(8,0,3000),inherited_fpscr=0x03c09f00)
m.run(0x150020c0,'pit-math',options=(8,0,3000),inherited_fpscr=0x01000000)
m.run(0x150020c0,'neon-denied',options=(8,0,3000))
