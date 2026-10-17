#!/usr/bin/env python3
"""Check the linked x87 ARM payload at two addresses and inherited FP policies."""
import argparse,importlib.util
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('build',type=Path);a=p.parse_args()
s=importlib.util.spec_from_file_location('pc_check',root/'tests/amiga/dosbox/check_pc_arm.py')
m=importlib.util.module_from_spec(s);s.loader.exec_module(m);m.OUT=a.build.resolve()
m.run(0x12000040,'x87',options=(8,0,6000),inherited_fpscr=0x03c09f00)
m.run(0x150020c0,'x87',options=(8,0,6000),inherited_fpscr=0x01000000)
