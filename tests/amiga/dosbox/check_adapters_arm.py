#!/usr/bin/env python3
"""Original fixtures on the linked ARM payload, no physical speed claims."""
import argparse
from pathlib import Path
import check_pc_arm as pc
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);a=p.parse_args();pc.OUT=a.out.resolve()
cases=[(3,'S3V101',640,480,8,0xffffff),(3,'S3V103',800,600,8,0xffffff),
 (3,'S3V105',1024,768,8,0xffffff),(3,'S3V110',640,480,15,0x00ff00),
 (3,'S3V111',640,480,16,0x00ff00),(3,'S3V114',800,600,16,0x00ff00),
 (3,'S3V10F',320,200,32,0xffffff),(4,'TANTEST',320,200,8,0xaa00aa),
 (5,'JRTTEST',320,200,8,0xaa00aa),(6,'HERCTEST',720,348,8,0xaaaaaa)]
for n,case in enumerate(cases):
 pc.run(0x12000040 if n%2==0 else 0x150020c0,'adapter',True,options=(8,case[0],3000),adapter=case[1:])
pc.run(0x12000040,'invalid-options',False,options=(8,7,3000))

for colour,pixel in [(1,0xaa7f00),(2,0x22aa22),(3,0xaaa18f)]:
 pc.run(0x12000040 if colour!=2 else 0x150020c0,'adapter',True,options=(8,6,3000,colour),adapter=('HERCTEST',720,348,8,pixel))
pc.run(0x12000040,'invalid-options',False,options=(8,6,3000,4))
