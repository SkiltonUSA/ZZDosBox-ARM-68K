#!/usr/bin/env python3
"""Optional TinyGL payload: pixels, relocation, SIMD state, stop and cleanup."""
import argparse
from pathlib import Path
import check_pc_arm as pc
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);a=p.parse_args();pc.OUT=a.out.resolve()
for mode,base,options,fixture in [
 ('adapter',0x12000040,(8,6,3000,1),('HERCTEST',720,348,8,0xaa7f00)),
 ('adapter',0x150020c0,(8,3,3000),('S3V105',1024,768,8,0xffffff)),
 ('fixture',0x12000040,(8,0,3000),None),
 ('copy-stop',0x12000040,(8,0,3000),None),
 ('neon-denied',0x12000040,(8,0,3000),None)]:
 pc.run(base,mode,True,options=options,adapter=fixture)
