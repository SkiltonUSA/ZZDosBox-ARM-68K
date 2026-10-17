from pathlib import Path
import re,json,statistics
root=Path(__file__).parent
rows=[]
for p in sorted(root.glob('*.log')):
 if not re.fullmatch(r'(vette|gods|fantasies|windows)-(base|ahead|byte|bulk)-\d+\.log',p.name):continue
 text=p.read_text();m=re.search(r'PC frames=(\d+) emulated_ms=(\d+) wall_ms=(\d+) timed_done=(\d+)',text)
 if not m:continue
 game,variant,index=p.stem.split('-')
 row=dict(game=game,variant=variant,run=int(index),frames=int(m[1]),guest_ms=int(m[2]),wall_ms=int(m[3]),complete=m[4]=='1' and 'LAUNCHER exit=0; owned memory released' in text)
 m=re.search(r'FILES_TIME calls=(\d+) read_us=(\d+) write_us=(\d+) other_us=(\d+)',text)
 if m:row.update(dict(zip(('fs_calls','read_us','write_us','other_us'),map(int,m.groups()))))
 row['ops']={m[0]:dict(calls=int(m[1]),us=int(m[2]))for m in re.findall(r'FILES_OP op=(\w+) calls=(\d+) total_us=(\d+)',text)}
 m=re.search(r'FILES requests=(\d+) read=(\d+) written=(\d+)',text)
 if m:row.update(read_bytes=int(m[2]),written_bytes=int(m[3]))
 rows.append(row)
(root/'measurements.json').write_text(json.dumps(rows,indent=2)+'\n')
for game in dict.fromkeys(r['game']for r in rows):
 print(game)
 for v in dict.fromkeys(r['variant']for r in rows if r['game']==game):
  rr=[r for r in rows if r['game']==game and r['variant']==v]
  print(v,[r['wall_ms']for r in rr],'mean',statistics.mean(r['wall_ms']for r in rr),'reads',[r['ops'].get('read',r['ops'].get('disk_read'))for r in rr])
