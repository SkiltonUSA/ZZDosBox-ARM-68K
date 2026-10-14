#!/usr/bin/env python3
"""Linked guest memory protocol and external JIT edit tests; model only."""
import argparse
from pathlib import Path
import check_pc_arm as pc
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build',required=True,type=Path)
args=parser.parse_args();pc.OUT=args.build.resolve()
def check_paused(fields,payload):assert fields[1]==1
def check_value(fields,payload):assert fields[3]==4 and payload[:4]==bytes.fromhex('78563412')
def check_one(fields,payload):assert fields[3]==1
def check_match(fields,payload):assert fields[3]==1 and payload[:8]==bytes.fromhex('0018000112345679')
requests=[
 ((2,0,0,0,0,0,0,0),0,check_paused),
 ((5,0x180001,0,4,0x12345678,0,0,0),0,check_one),
 ((4,0x180001,4,0,0,0,0,0),0,check_value),
 ((6,0x180000,16,4,0x12345678,0,0,0),0,check_one),
 ((5,0x180001,0,2,0x5679,0,0,0x5678),0,check_one),
 ((7,0,0,0,0,2,0,0),0,check_one),
 ((8,0,0,0,0,0,0,0),0,check_match),
 ((4,0x9ffff,2,0,0,0,0,0),3,None),
 ((5,0x180001,0,4,0,0,0,0),1,None),
 ((3,0,0,0,0,0,0,0),0,None),
 ((5,0x180001,0,1,0,0,0,0),2,None)]
pc.run(0x12000040,'fixture',True,options=(8,0,3000),memory_requests=requests)
pc.run(0x150020c0,'fixture',True,options=(8,0,3000),memory_requests=[requests[0]],memory_stop=True)
pc.run(0x150020c0,'copy-stop',True,options=(8,0,3000))
pc.run(0x12000040,'memory-smc',True,options=(8,0,3000))
print('PASS memory linked model tests',flush=True)
