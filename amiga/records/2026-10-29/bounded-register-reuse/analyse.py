import json,re,statistics
from pathlib import Path
out=Path(__file__).resolve().parent
rows=[]
for p in sorted(out.glob('*-[12].log')):
 t=p.read_text(encoding='latin1')
 assert all(x in t for x in ['owned memory released','register_mismatches=00000000','witness_bad=0','context_equal=1','LAUNCHER exit=0','CPU_CYCLES applied=10000'])
 m=re.search(r'DUKECLOCK.*?rejected=(\d+) first=(\d+) last=(\d+) sim_first=(\d+) sim_last=(\d+) start=([0-9a-f]+) end=([0-9a-f]+)',t);assert m and not int(m[1])
 seconds=(int(m[7],16)-int(m[6],16))/333333333
 a=re.search(r'AUDIO_TASK queued=(\d+) submitted=(\d+) completed=(\d+).*error=0 stopped=1 gaps=(\d+)',t);assert a and a[1]==a[2]==a[3]
 j=re.search(r'JITREUSE replacements=(\d+)',t)
 rows.append(dict(run=p.stem,seconds=seconds,frames=int(m[3])-int(m[2]),fps=(int(m[3])-int(m[2]))/seconds,guest_wall=(int(m[5])-int(m[4]))/(1000*seconds),replacements=int(j[1]) if j else None,pcm=int(a[1]),late_links=int(a[4])))
means={key:statistics.mean(r['fps'] for r in rows if r['run'].startswith(key+'-')) for key in ('ref','reuse')}
result=dict(runs=rows,mean_fps=means,change_percent=100*(means['reuse']/means['ref']-1))
(out/'results.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
