#!/usr/bin/env python3
"""Recalculate published Duke game FPS; reject incomplete physical samples."""
from pathlib import Path
import json,re
root=Path(__file__).resolve().parent
for row in json.loads((root/'results.json').read_text()):
 name=row['name'];s=(root/(name+'.log')).read_text()
 for marker in ('owned memory released','register_mismatches=00000000','witness_bad=0','context_equal=1','SHELL PASS'):
  assert marker in s,(name,marker)
 a=re.search(r'AUDIO_TASK queued=(\d+) submitted=(\d+) completed=(\d+).*?error=(\d+) stopped=(\d+)',s)
 assert a and a[1]==a[2]==a[3]and a[4]=='0'and a[5]=='1',name
 m=re.search(r'DUKECLOCK.*?rejected=(\d+) first=(\d+) last=(\d+) sim_first=(\d+) sim_last=(\d+) start=([0-9a-f]+) end=([0-9a-f]+)',s)
 assert m and int(m[1])==0 and int(m[3])>int(m[2]),name
 seconds=(int(m[7],16)-int(m[6],16))/333333333
 fps=(int(m[3])-int(m[2]))/seconds
 assert abs(fps-row['fps'])<0.000001,(name,fps,row['fps'])
 assert abs((int(m[5])-int(m[4]))/(1000*seconds)-row['guest_speed'])<0.000001,name
 if name=='hot':assert 'scoped=1'in s;print(name,'diagnostic only, excluded from FPS comparisons')
 else:print(f'{name}: {fps:.3f} game FPS, {row["guest_speed"]*100:.1f}% emulated-time progress; clean teardown')
