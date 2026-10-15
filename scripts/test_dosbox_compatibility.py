#!/usr/bin/env python3
"""Check extraction edge cases and the native offline search model."""
import json
from pathlib import Path
import subprocess
from scrape_dosbox_compatibility import parse, c_string
ROOT=Path(__file__).resolve().parents[1]
# The site's letter count is unique game IDs, not version-report rows.
html='''<select><option value="C">C (1)</option></select>
<table class="tablecomp"><tr><th>Game</th></tr>
<tr><td><a href="comp_list.php?showID=1&amp;letter=C">C &amp; C (???)</a></td><td>1995</td><td></td><td>0.72</td><td></td><td>supported</td><td><img class="status" alt="100% (supported)"></td></tr>
<tr><td><a href="comp_list.php?showID=1&amp;letter=C">C &amp; C (???)</a></td><td>1995</td><td></td><td>0.74-3</td><td></td><td>broken</td><td></td></tr></table>'''
rows=parse(html.encode(),'C')
assert len(rows)==2 and rows[0]['title']=='C & C (???)'
assert rows[0]['rating_percent']==100 and rows[1]['rating_percent'] is None
assert '?' not in c_string('C (???)') # Prevent C trigraph reinterpretation.
try:parse(html.replace('C (1)','C (2)').encode(),'C')
except ValueError:pass
else:raise AssertionError('Missing source entries were accepted')
snapshot=json.loads((ROOT/'amiga/dosbox/compatibility/dosbox-compatibility.json').read_text())
assert snapshot['count']==len(snapshot['games'])
assert snapshot['unique_games']==len({r['game_id'] for r in snapshot['games']})
details=json.loads((ROOT/'amiga/dosbox/compatibility/dosbox-details.json').read_text())
assert {g['game_id'] for g in details['games']}=={g['game_id'] for g in snapshot['games']}
assert all(len(g['note_preview'].removesuffix(' ...').split())<=20 for g in details['games'])
for g in details['games']:
    d=g['discussion'];assert d['indexed_comments']==g['comment_count']==len(d['entries'])
    assert d['unclassified']==sum(not e['topics'] for e in d['entries'])
    assert bool(d['summary'])==bool(g['comment_count'])
    assert all(t['comments']==sum(t['id'] in e['topics'] for e in d['entries']) for t in d['topics'])
    assert len(d['summary'].split())<180
assert len(snapshot['pages'])==27
assert sum(p['count'] for p in snapshot['pages'])==snapshot['count']
out=ROOT/'.context/amiga/dosbox-compatibility';out.mkdir(parents=True,exist_ok=True)
subprocess.run(['clang','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-Iamiga/dosbox/launcher','tests/amiga/dosbox/compatibility.c','-o',str(out/'test-compatibility')],cwd=ROOT,check=True)
subprocess.run([str(out/'test-compatibility'),str(ROOT/'.context/amiga/dosbox-launcher/Compatibility.dat')],check=True)
print('PASS directory extraction, source counts, duplicate-version preservation and C escaping')
