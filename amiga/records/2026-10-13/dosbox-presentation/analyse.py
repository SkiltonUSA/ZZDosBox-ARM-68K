#!/usr/bin/env python3
"""Analyse completed physical runs. Never infer game FPS from VGA frames."""
import json,re
from pathlib import Path
root=Path(__file__).resolve().parent
rows=[]
for path in sorted(root.glob('physical-doom-*.log')):
 text=path.read_text()
 def fields(prefix):
  line=re.search(r'^'+prefix+r' .*',text,re.M)
  assert line,(path.name,prefix)
  return dict(re.findall(r'(\w+)=([a-f0-9]+)',line.group()))
 for required in ('timed 2134 gametics','SHELL result=0 guest_exit=0','SHELL PASS','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released','CPU_CYCLES applied=10000'):
  assert required in text,(path.name,required)
 d,p,n,j=map(fields,('DOOMCLOCK','PRESENT','NEON','JIT'))
 assert d['signature']=='1' and d['rejected']=='0' and d['last']=='2134' and int(d['first']) in (0,1)
 assert n['witness']=='42' and n['context_equal']=='1' and j['witness']=='1'
 assert p.pop('magic')=='50525331'
 p['total_copy']=int(p['total_copy'],16)
 p={k:int(v) for k,v in p.items()}
 assert p['source']==p['shown']+p['skipped'] and 1<=p['divisor']<=4
 pc=fields('PC');display=fields('DISPLAY')
 assert p['source']==int(pc['frames']) and p['shown']==int(display['frames'])
 seconds=(int(d['end'],16)-int(d['start'],16))/333333333
 profile={}
 for name,ticks,calls in re.findall(r'^PROFILE bucket=(\w+) ticks=([0-9a-f]+) calls=(\d+)',text,re.M):
  profile[name]={'seconds':int(ticks,16)/333333333,'calls':int(calls)}
 assert not profile or set(profile)=={'other','cpu','events','capture','grant','copy'}
 rows.append({'run':path.stem.removeprefix('physical-doom-'),'seconds':seconds,'game_fps':(2134-int(d['first']))/seconds,'presentation':p,'whole_session_wall_seconds':int(pc['wall_ms'])/1000,'profile':profile,'profiling':bool(profile),'clean_teardown':True})
if rows:
 for row in rows:
  control='off' if row['profiling'] else 'off-no-profile'
  baseline=next((r['game_fps'] for r in rows if r['run']==control),None)
  if baseline:row['gain_percent_vs_matched_off']=(row['game_fps']/baseline-1)*100
 (root/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
 for r in rows:print(f"{r['run']}: {r['seconds']:.3f}s, {r['game_fps']:.4f} real game FPS, shown={r['presentation']['shown']}, skipped={r['presentation']['skipped']}, divisor={r['presentation']['divisor']}")
