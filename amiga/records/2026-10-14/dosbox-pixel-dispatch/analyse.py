#!/usr/bin/env python3
"""Physical A9 timer results, not emulator-reported VGA refresh counts."""
import json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parent
rows=[]
for name in ('doom-a1','doom-b1','doom-b2','doom-a2'):
 s=(ROOT/(name+'.log')).read_text()
 for check in ('SHELL PASS','context_equal=1','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released'):assert check in s
 m=re.search(r'DOOMCLOCK magic=44424d31 signature=1 rejected=0 first=(\d+) last=(\d+) sim_first=\d+ sim_last=\d+ start=([0-9a-f]+) end=([0-9a-f]+)',s);assert m
 frames=int(m[2])-int(m[1]);assert frames==2134
 elapsed=(int(m[4],16)-int(m[3],16))/333333333
 rows.append(dict(run=name,frames=frames,seconds=elapsed,fps=frames/elapsed))
a=(rows[0]['fps']+rows[3]['fps'])/2;b=(rows[1]['fps']+rows[2]['fps'])/2
kernels=[]
s=(ROOT/'kernel-1.log').read_text();assert 'PIXELBENCH magic=50495831 errors=0 count=8' in s
for w,t,f,n,a_ticks,b_ticks in re.findall(r'PIXELBENCH width=(\d+) target=(\d+) format=(\d+) rows=(\d+) baseline=([0-9a-f]+) candidate=([0-9a-f]+)',s):
 x=int(a_ticks,16);y=int(b_ticks,16)
 kernels.append(dict(width=int(w),target=int(t),format=int(f),rows=int(n),baseline_us=x/333.333333/int(n),candidate_us=y/333.333333/int(n),throughput_percent=(x/y-1)*100))
print(json.dumps(dict(doom=rows,baseline_mean=a,candidate_mean=b,percent=(b/a-1)*100,cached_ram_kernels=kernels),indent=2))
