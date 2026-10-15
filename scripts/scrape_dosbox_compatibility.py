#!/usr/bin/env python3
"""Snapshot DOSBox's public directory, then generate offline reference assets.

--fetch requests 27 directory pages sequentially, one second apart. Individual
reviews/comments are not downloaded. Without --fetch, rebuild from cached HTML.
"""
import argparse
import csv
import hashlib
from html.parser import HTMLParser
import json
from pathlib import Path
import re
import string
import time
from urllib.parse import parse_qs, urljoin, urlparse
from urllib.request import Request, urlopen

ROOT = Path(__file__).resolve().parents[1]
BASE = 'https://www.dosbox.com/comp_list.php'
BUCKETS = list(string.ascii_uppercase) + ['num']

class Directory(HTMLParser):
    def __init__(self, bucket):
        super().__init__(convert_charrefs=True)
        self.source = BASE + '?letter=' + bucket
        self.bucket = bucket
        self.active = False
        self.cell = None
        self.href = None
        self.rows = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag == 'table' and attrs.get('class') == 'tablecomp':
            self.active = True
        if not self.active:
            return
        if tag == 'tr':
            self.cells, self.href, self.rating = [], None, None
        elif tag == 'td':
            self.cell = []
        elif tag == 'a' and 'showID=' in attrs.get('href', ''):
            self.href = attrs['href']
        elif tag == 'img' and attrs.get('class') == 'status':
            self.rating = attrs['alt']

    def handle_data(self, data):
        if self.active and self.cell is not None:
            self.cell.append(data)

    def handle_endtag(self, tag):
        if not self.active:
            return
        if tag == 'td' and self.cell is not None:
            self.cells.append(''.join(self.cell).strip())
            self.cell = None
        elif tag == 'tr' and self.href:
            if len(self.cells) != 7:
                raise ValueError('Unexpected directory columns')
            title, year, _, version, _, status, _ = self.cells
            match = re.fullmatch(r'(\d+)% \(([^)]+)\)', self.rating or '')
            if not (status == 'broken' and self.rating is None) and (not match or match[2] != status or not 0 <= int(match[1]) <= 100):
                raise ValueError('Invalid status: ' + repr(self.cells))
            source_title = title
            if 'Ã' in title or 'Â' in title:
                try:
                    title = title.encode('latin-1').decode('utf-8')
                except (UnicodeEncodeError, UnicodeDecodeError):
                    pass
            title = ' '.join(title.split())
            detail = urljoin(BASE, self.href)
            self.rows.append(dict(game_id=int(parse_qs(urlparse(detail).query)['showID'][0]),
                letter=self.bucket, title=title, source_title=source_title,
                year=year, dosbox_version=version, status=status,
                rating_percent=int(match[1]) if match else None, detail_url=detail, source_url=self.source))
        elif tag == 'table':
            self.active = False


def parse(raw, bucket):
    text = raw.decode('utf-8')
    parser = Directory(bucket)
    parser.feed(text)
    counts = dict(re.findall(r'<option value=[\'\"]([^\'\"]+)[\'\"][^>]*>[^<]*\((\d+)\)</option>', text))
    expected = int(counts[bucket])
    anchors = len(re.findall(r'<a href="comp_list.php\?showID=', text))
    if expected != len({r['game_id'] for r in parser.rows}) or anchors != len(parser.rows):
        raise ValueError(f'{bucket}: expected {expected}, links {anchors}, parsed {len(parser.rows)}')
    return parser.rows


def c_string(value):
    # Workbench's default fonts use an 8-bit character set, not UTF-8.
    raw = value.encode('latin-1', errors='replace')
    if any(b < 32 or b == 127 for b in raw):
        raise ValueError('Control character in directory data')
    return '"' + ''.join(chr(b) if 32 <= b < 127 and b not in (34, 63, 92)
                         else f'\\{b:03o}' for b in raw) + '"'


def markdown(value):
    return value.replace('\\', '\\\\').replace('|', '\\|').replace('[', '\\[').replace(']', '\\]').replace('<', '&lt;').replace('>', '&gt;')


def generate(cache):
    rows, pages = [], []
    fetched = json.loads((cache/'fetched.json').read_text())
    for bucket in BUCKETS:
        raw = (cache/(bucket+'.html')).read_bytes()
        items = parse(raw, bucket)
        rows.extend(items)
        pages.append(dict(letter=bucket, count=len(items), source_url=BASE+'?letter='+bucket,
                          sha256=hashlib.sha256(raw).hexdigest(), fetched_at_utc=fetched[bucket]))
    unique_games = len({r['game_id'] for r in rows})
    out = ROOT/'amiga/dosbox/compatibility'
    out.mkdir(parents=True, exist_ok=True)
    snapshot = dict(source='DOSBox community compatibility directory',
                    scope='Upstream DOSBox reports, not verified ZZDosBox compatibility or performance.',
                    count=len(rows), unique_games=unique_games, pages=pages, games=rows)
    (out/'dosbox-compatibility.json').write_text(json.dumps(snapshot, ensure_ascii=False, indent=2)+'\n')
    with (out/'dosbox-compatibility.csv').open('w', newline='', encoding='utf-8-sig') as f:
        writer=csv.DictWriter(f, fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
    date = max(fetched.values())[:10]
    md = ['# DOSBox compatibility reference', '',
          '[Per-game details, version history and comment previews](amiga-dosbox-game-details.md)', '',
          f'{len(rows):,} reports for {unique_games:,} games across A–Z and 0–9. Snapshot: {date}.', '',
          'Source: [DOSBox community compatibility directory](https://www.dosbox.com/comp_list.php?letter=a).', '',
          'These are upstream DOSBox reports for the listed versions, **not verified ZZDosBox results**.',
          'Percentages are compatibility ratings, not FPS or performance measurements.',
          'Separate reports with the same game title are retained. Each title links to its offline details, which include the original report link.', '',
          'The launcher provides this snapshot offline under **Compatibility**, with letter and title filters.',
          'CSV and JSON copies are in `amiga/dosbox/compatibility/`. Rebuild with',
          '`python3 scripts/scrape_dosbox_compatibility.py`; add `--fetch` to refresh the source pages.', '',
          'Some source titles contain broken text encoding; reversible repairs are applied for display.',
          'JSON/CSV retain the original title as `source_title`. The Amiga uses Latin-1 display text.', '']
    header=['/* Generated by scripts/scrape_dosbox_compatibility.py; do not edit. */',
            '#ifndef ZZ_DOSBOX_COMPATIBILITY_DATA_H', '#define ZZ_DOSBOX_COMPATIBILITY_DATA_H',
            f'#define COMPAT_SNAPSHOT "{date}"',
            'struct compat_entry { const char *title,*version,*status,*year; unsigned id; unsigned char percent,bucket; };',
            'static const struct compat_entry compat_entries[] = {']
    for bucket in BUCKETS:
        md += ['## '+('0–9' if bucket=='num' else bucket), '',
               '| Game | Year | DOSBox | Status | Rating |', '| --- | --- | --- | --- | ---: |']
        for r in (r for r in rows if r['letter']==bucket):
            rating = str(r['rating_percent'])+'%' if r['rating_percent'] is not None else '—'
            md.append(f"| [{markdown(r['title'])}](amiga-dosbox-game-details.md#game-{r['game_id']}) | {r['year']} | {r['dosbox_version']} | {r['status']} | {rating} |")
            strings=','.join(c_string(r[k]) for k in ('title','dosbox_version','status','year'))
            header.append('{'+strings+f",{r['game_id']},{r['rating_percent'] if r['rating_percent'] is not None else 255},{BUCKETS.index(bucket)+1}"+'},')
        md.append('')
    header += ['};','#define COMPAT_COUNT (sizeof(compat_entries)/sizeof(compat_entries[0]))','#endif','']
    (out/'data.h').write_text('\n'.join(header))
    (ROOT/'docs/amiga-dosbox-compatibility.md').write_text('\n'.join(md))
    print(f'Validated {len(rows)} reports for {unique_games} games across {len(pages)} pages.')
    print('Statuses:', sorted(set(r['status'] for r in rows)))
    print('Maximum title length:', max(len(r['title']) for r in rows))


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--fetch', action='store_true')
    p.add_argument('--resume', action='store_true', help='Retain already downloaded pages during --fetch')
    p.add_argument('--cache', type=Path, default=ROOT/'.context/amiga/dosbox-compatibility/pages')
    args=p.parse_args();args.cache.mkdir(parents=True, exist_ok=True)
    if args.fetch:
        from datetime import datetime, timezone
        fetched=json.loads((args.cache/'fetched.json').read_text()) if args.resume and (args.cache/'fetched.json').exists() else {}
        for bucket in BUCKETS:
            if bucket in fetched and (args.cache/(bucket+'.html')).exists():
                parse((args.cache/(bucket+'.html')).read_bytes(),bucket)
                continue
            url=BASE+'?letter='+bucket
            request=Request(url, headers={'User-Agent':'ZZDosBox-Compatibility-Snapshot/1.0'})
            with urlopen(request, timeout=30) as response:raw=response.read()
            rows=parse(raw,bucket)
            (args.cache/(bucket+'.html')).write_bytes(raw)
            fetched[bucket]=datetime.now(timezone.utc).isoformat()
            (args.cache/'fetched.json').write_text(json.dumps(fetched,indent=2)+'\n')
            print(bucket,len(rows),flush=True)
            time.sleep(1)
    generate(args.cache)

if __name__=='__main__':main()
