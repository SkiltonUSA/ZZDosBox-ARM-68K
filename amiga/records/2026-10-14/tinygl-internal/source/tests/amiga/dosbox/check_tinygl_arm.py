#!/usr/bin/env python3
"""Run the linked internal TinyGL diagnostic on the A9 instruction model."""
import argparse
from pathlib import Path
import check_pc_arm as pc
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build',required=True,type=Path);a=p.parse_args();pc.OUT=a.build.resolve()
for mode in ('fixture','copy-stop','neon-denied'):
    pc.run(0x150020c0,mode,True,options=(8,0,3000),inherited_fpscr=0x03c00000)
pc.run(0x12000040,'fixture',True,options=(8,0,3000),inherited_fpscr=0x03c00000,matrix_stop=True)
