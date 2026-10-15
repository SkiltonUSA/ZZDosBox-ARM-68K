#!/usr/bin/env python3
"""Fetch per-game facts, all-comment topic overviews and bounded previews.

Caches raw pages privately. Public exports include at most 20 quoted words
per game page, with a source link; complete user discussions remain on DOSBox.com.
"""
import argparse
from datetime import datetime, timezone
import hashlib
import http.client
from html.parser import HTMLParser
import json
from pathlib import Path
import re
import time
from scrape_dosbox_compatibility import ROOT
from dosbox_discussion import overview

CACHE=ROOT/'.context/amiga/dosbox-compatibility/details'
INDEX=ROOT/'amiga/dosbox/compatibility/dosbox-compatibility.json'
class Text(HTMLParser):
    def __init__(self):super().__init__();self.parts=[]
    def handle_data(self,s):self.parts.append(s)
    def handle_starttag(self,t,a):
        if t in ('br','td','p'):self.parts.append(' ')
def plain(s):
    p=Text();p.feed(s);v=' '.join(''.join(p.parts).split())
    if 'Ã' in v or 'Â' in v:
        try:v=v.encode('latin-1').decode('utf-8')
        except (UnicodeEncodeError,UnicodeDecodeError):pass
    return v
class Comments(HTMLParser):
    def __init__(self):super().__init__();self.depth=0;self.target=0;self.kind=None;self.parts=[];self.headers=[];self.bodies=[]
    def handle_starttag(self,t,a):
        a=dict(a)
        if t=='table':
            self.depth+=1
            if not self.target and a.get('bgcolor','').lower() in ('#355787','#0d2e5d'):
                self.target=self.depth;self.kind=a['bgcolor'].lower();self.parts=[]
        if self.target and t in ('td','br','p'):self.parts.append(' ')
    def handle_data(self,s):
        if self.target:self.parts.append(s)
    def handle_endtag(self,t):
        if t=='table':
            if self.target==self.depth:
                text=' '.join(''.join(self.parts).split())
                (self.headers if self.kind=='#355787' else self.bodies).append(text)
                self.target=0
            self.depth-=1

def parse(raw,game):
    html=raw.decode('utf-8');start=html.index('Game details </div>')
    section=html[start:].split('</div><br',1)[0]
    header=section.split('<table',1)[0]
    descriptor=plain(header).removeprefix('Game details ').split('Tested By:',1)
    title_publisher=descriptor[0].strip();tester=descriptor[1].strip() if len(descriptor)>1 else ''
    years=re.search(r' \(([^()]*)\)$',title_publisher)
    before_year=title_publisher[:years.start()] if years else title_publisher
    publisher=before_year[len(game['title'])+3:] if before_year.startswith(game['title']+' - ') else (before_year.rpartition(' - ')[2] if ' - ' in before_year else '')
    versions=[]
    for block in section.split('<hr'):
        m=re.search(r'DOSBox version:</span>\s*([^<()]+)\s*\(<span class="([^"]+)">([^<]+)</span>',block)
        if not m:continue
        rating=re.search(r"alt=['\"](\d+)% \(([^)]+)\)['\"]",block)
        pct=int(rating[1]) if rating else (0 if re.search(r'>\s*0% supported\s*<',block) else None)
        if pct is not None and not 0<=pct<=100:raise ValueError('Bad percentage')
        status=m[3].strip()
        if status not in ('supported','playable','runnable','broken'):raise ValueError(status)
        versions.append(dict(dosbox_version=m[1].strip(),status=status,rating_percent=pct))
    if not versions or len(versions)!=section.count('DOSBox version:'):raise ValueError('Incomplete version extraction')
    comment_html=html[start+len(section):]
    # The site repeats the body colour in its footer; stop at the next caption.
    comment_html=re.split(r'<div[^>]*class="caption"',comment_html,maxsplit=1)[0]
    comments=Comments();comments.feed(comment_html)
    if len(comments.headers)!=len(comments.bodies):raise ValueError('Unpaired comments')
    newest=comments.headers[0] if comments.headers else ''
    stamp=re.search(r'\((\d{4}-\d{2}-\d{2} \d{2}:\d{2})\)',newest)
    # The excerpt is bounded across the entire source page, not per comment.
    words=comments.bodies[0].split() if comments.bodies else []
    excerpt=' '.join(words[:20])+(' ...' if len(words)>20 else '')
    return dict(game_id=game['game_id'],title=game['title'],publisher=publisher,
        year=years[1] if years else game['year'],tested_by=tester,
        versions=versions,comment_count=len(comments.bodies),
        discussion=overview(comments.headers,comments.bodies),
        latest_comment_at=stamp[1] if stamp else '',
        latest_comment_author=newest[stamp.end():].strip() if stamp else '',
        note_preview=excerpt,source_url=f"https://www.dosbox.com/comp_list.php?showID={game['game_id']}",
        source_sha256=hashlib.sha256(raw).hexdigest())

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--fetch',action='store_true');ap.add_argument('--limit',type=int);a=ap.parse_args()
    CACHE.mkdir(parents=True,exist_ok=True)
    index=json.loads(INDEX.read_text());games={g['game_id']:g for g in index['games']}
    if a.limit:games=dict(list(games.items())[:a.limit])
    errors={};done=[]
    connection=http.client.HTTPSConnection('www.dosbox.com',timeout=30)
    for i,(gid,game) in enumerate(games.items(),1):
        raw_path=CACHE/f'{gid}.html';data_path=CACHE/f'{gid}.json'
        try:
            if not raw_path.exists():
                if not a.fetch:raise FileNotFoundError(raw_path)
                for attempt in range(3):
                    try:
                        connection.request('GET',f'/comp_list.php?showID={gid}',headers={'User-Agent':'ZZDosBox-Compatibility-Snapshot/1.0'})
                        response=connection.getresponse();raw=response.read()
                        if response.status!=200:raise ValueError(f'HTTP {response.status}')
                        if b'Game details </div>' not in raw:raise ValueError('Missing game details')
                        raw_path.write_bytes(raw);break
                    except Exception:
                        connection.close();connection=http.client.HTTPSConnection('www.dosbox.com',timeout=30)
                        if attempt==2:raise
                        time.sleep(3*(attempt+1))
                time.sleep(.20)
            data=parse(raw_path.read_bytes(),game)
            data['fetched_at_utc']=datetime.fromtimestamp(raw_path.stat().st_mtime,timezone.utc).isoformat()
            data_path.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n');done.append(data)
        except Exception as e:errors[str(gid)]=str(e);print(f'ERROR {gid}: {e}',flush=True)
        if i%100==0 or i==len(games):
            progress=dict(processed=i,total=len(games),complete=len(done),errors=errors)
            (CACHE/'progress.json').write_text(json.dumps(progress,indent=2)+'\n');print(f'{i}/{len(games)} complete={len(done)} errors={len(errors)}',flush=True)
    connection.close()
    if errors:raise SystemExit(f'{len(errors)} incomplete records: see {CACHE}/progress.json')
    if not a.limit:
        target=ROOT/'amiga/dosbox/compatibility/dosbox-details.json'
        target.write_text(json.dumps(dict(count=len(done),games=done),ensure_ascii=False,indent=2)+'\n')
        print('Wrote',target,flush=True)
if __name__=='__main__':main()
