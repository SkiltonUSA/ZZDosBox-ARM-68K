#!/usr/bin/env python3
"""Render the networking Markdown master as an offline AmigaGuide document."""
from pathlib import Path
import re
import textwrap
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'docs/amiga-dosbox-network-setup.md'
OUT = ROOT / '.context/amiga/dosbox-launcher/Network.guide'


def plain(text):
    text = re.sub(r'\[([^]]+)\]\(([^)]+)\)', r'\1', text)
    text = text.replace('**', '').replace('`', '')
    for old, new in [('—', ' - '), ('–', '-'), ('→', ' -> '), ('’', "'"), ('×', 'x')]:
        text = text.replace(old, new)
    return unicodedata.normalize('NFKD', text).encode('ascii', 'ignore').decode()


def body(text):
    paragraphs, pending = [], []
    def flush():
        if pending:
            paragraphs.append(textwrap.fill(' '.join(pending), width=76))
            pending.clear()
    for raw in text.splitlines():
        line = plain(raw.strip())
        if not line:
            flush()
        elif line.startswith('|'):
            flush()
            cells = [c.strip() for c in line.strip('|').split('|')]
            if all(re.fullmatch(r'[-: ]+', c) for c in cells):
                continue
            paragraphs.append(textwrap.fill(': '.join(cells), width=76, subsequent_indent='  '))
        elif re.match(r'(- |\d+\. )', line):
            flush()
            pending.append(line)
        else:
            pending.append(line)
    flush()
    return '\n\n'.join(paragraphs)


def build():
    sections = re.split(r'^## ', SOURCE.read_text(), flags=re.MULTILINE)
    intro = sections.pop(0).split('\n', 1)[1]
    pages = [s.split('\n', 1) for s in sections]
    guide = '@database Network.guide\n@node MAIN "ZZDosBox networking"\n'
    guide += '@{b}ZZDosBox networking@{ub}\n\n' + body(intro) + '\n\n'
    for i, (title, _) in enumerate(pages):
        guide += '@{"' + plain(title) + '" link PAGE' + str(i) + '}\n'
    guide += '@endnode\n'
    for i, (title, text) in enumerate(pages):
        guide += f'@node PAGE{i} "{plain(title)}"\n@{{b}}{plain(title)}@{{ub}}\n\n'
        guide += body(text) + '\n\n@{"Contents" link MAIN}\n@endnode\n'
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_bytes(guide.encode('ascii'))
    print(f'Built {OUT.name}: {len(pages)} sections, {OUT.stat().st_size} bytes')


if __name__ == '__main__':
    build()
