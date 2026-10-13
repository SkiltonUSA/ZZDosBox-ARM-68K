#!/usr/bin/env python3
"""Validate completed scanline experiments, using real A9 timer endpoints."""
import json,re
from pathlib import Path
root=Path(__file__).resolve().parent
rows=[]
for mode in ('off','full','skip'):
 p=root/('physical-'+mode+'.log')
 if not p.exists():continue
 s=p.read_text()
 def fields(prefix):
  line=re.search(r'^'+prefix+r' .*',s,re.M)
  assert line,prefix
  return dict(re.findall(r'(\w+)=([a-f0-9]+)',line.group()))
 for text in ('timed 2134 gametics','SHELL result=0 guest_exit=0','SHELL PASS','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released','CPU_CYCLES applied=10000'):
  assert text in s,(mode,text)
 d,j,n,c=map(fields,('DOOMCLOCK','JIT','NEON','SCANLINES'))
 assert d['signature']=='1' and d['rejected']=='0' and d['last']=='2134' and int(d['first']) in (0,1)
 assert j['cache_bytes']=='4194304' and j['witness']=='1'
 assert n['witness']=='42' and n['context_equal']=='1'
 assert c.pop('magic')=='53434e31';c={k:int(v) for k,v in c.items()}
 assert c['rendered_rows']+c['cleared_rows']+c['skipped_rows']==c['frames']*480
 if mode=='off':assert c['rendered_rows']==c['frames']*480 and c['nonce_repairs']==0
 else:
  assert c['rendered_rows']==c['frames']*240 and c['nonce_repairs']==c['frames']
  if mode=='full':assert c['cleared_rows']==c['frames']*240 and not c['skipped_rows']
  else:assert c['cleared_rows']>=720 and c['skipped_rows']>c['cleared_rows']
 seconds=(int(d['end'],16)-int(d['start'],16))/333333333
 rows.append(dict(mode=mode,seconds=seconds,game_fps=(2134-int(d['first']))/seconds,rows=c,clean_teardown=True))
if rows:
 base=next((r['game_fps'] for r in rows if r['mode']=='off'),None)
 for row in rows:
  if base:row['gain_percent_vs_off']=(row['game_fps']/base-1)*100
 (root/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
 for r in rows:print(f"{r['mode']}: {r['seconds']:.3f} seconds, {r['game_fps']:.4f} real FPS")
