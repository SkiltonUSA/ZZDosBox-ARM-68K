#!/usr/bin/env python3
"""Embed versioned changelog entries in the launcher's native scroll box."""
import json
import re
from pathlib import Path
from prepare_dosbox_network_guide import plain

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'amiga/dosbox/CHANGELOG.md'
OUT = ROOT / '.context/amiga/dosbox-launcher/changelog_text.h'


def build():
    entries = []
    for section in re.split(r'^## ', SOURCE.read_text(), flags=re.MULTILINE)[1:]:
        heading, body = section.split('\n', 1)
        if not re.match(r'\d+\.\d+\.\d+', heading):
            continue
        body = re.sub(r'^<a id=.*?</a>\s*$', '', body, flags=re.MULTILINE)
        # Markdown wraps prose across source lines; let Floattext wrap to the UI.
        paragraphs = []
        for paragraph in re.split(r'\n\s*\n', body.strip()):
            paragraph = re.sub(r'\n[ \t]+', ' ', paragraph)
            if not paragraph.startswith('- '):
                paragraph = paragraph.replace('\n', ' ')
            paragraphs.append(plain(paragraph))
        entries.append(plain(heading) + '\n' + '\n\n'.join(paragraphs))
    text = '\n\n'.join(entries) + '\n'
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text('/* Generated from amiga/dosbox/CHANGELOG.md. */\n'
                   'static const char dl_changelog_text[] =\n' +
                   '\n'.join(json.dumps(line + '\n') for line in text.splitlines()) + ';\n')
    print(f'Embedded changelog: {len(entries)} versions, {len(text)} bytes')


if __name__ == '__main__':
    build()
