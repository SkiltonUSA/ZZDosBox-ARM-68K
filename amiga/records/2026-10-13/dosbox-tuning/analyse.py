#!/usr/bin/env python3
"""Compare completed hardware Doom runs; never use the guest clock as FPS."""
import json,re
from pathlib import Path
HERE=Path(__file__).resolve().parent
baseline=json.loads((HERE.parent/'dosbox-dynrec/result.json').read_text())
rows=[]
for label,name,cache,neon in (
 ('1 MiB, scalar, serial','../dosbox-dynrec/physical-doom.log',1048576,False),
 ('1 MiB, scalar, overlap','physical-pipeline.log',1048576,False),
 ('4 MiB, scalar, overlap','physical-4m-doom.log',4194304,False),
 ('4 MiB, NEON, overlap','physical-neon-doom.log',4194304,True)):
 p=HERE/name
 if not p.exists():continue
 s=p.read_text()
 def fields(prefix):
  return dict(re.findall(r'(\w+)=([a-f0-9]+)',re.search(r'^'+prefix+r' .*',s,re.M).group()))
 d,j,pipe=fields('DOOMCLOCK'),fields('JIT'),fields('PIPELINE')
 assert d['magic']=='44424d31' and d['signature']=='1' and d['rejected']=='0'
 assert int(d['first']) in (0,1) and int(d['last'])==2134
 assert j['witness']=='1' and int(j['cache_bytes'])==cache and int(j['dispatches'])>0
 if neon:
  n=fields('NEON');assert n['magic']=='4e454f31' and n['witness']=='42' and n['context_equal']=='1'
 for expected in ('timed 2134 gametics','SHELL result=0 guest_exit=0','SHELL PASS','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released'):
  assert expected in s,(name,expected)
 seconds=(int(d['end'],16)-int(d['start'],16))/333333333
 assert seconds>0
 fps=(2134-int(d['first']))/seconds
 rows.append(dict(label=label,seconds_approx=seconds,game_fps_approx=fps,
  ratio_to_initial_dynrec=fps/baseline['measured_game_fps_approx'],
  jit=j,pipeline=pipe,display=fields('DISPLAY'),copy=fields('COPY'),clean_teardown=True))
(HERE/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
for row in rows:print(f"{row['label']}: {row['game_fps_approx']:.4f} FPS, {row['seconds_approx']:.3f} seconds, {row['ratio_to_initial_dynrec']:.4f}x")
