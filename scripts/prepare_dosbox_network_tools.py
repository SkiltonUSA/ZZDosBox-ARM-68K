#!/usr/bin/env python3
"""Stage hash-pinned guest utilities from local upstream inputs; no deployment."""
import argparse
import hashlib
import json
import re
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--mtcp', type=Path, required=True, help='Official mTCP 2026-10-10 ZIP')
p.add_argument('--ne2000-source', type=Path, required=True, help='Pinned DOSBox-X src/builtin/ne2000bin.cpp')
p.add_argument('--output', type=Path, default=ROOT/'.context/amiga/dosbox-launcher/Network')
a = p.parse_args()
pins = {a.mtcp: 'bc201d7c0afe6de9ce54576345b79b7e692b7bd40de8e5362c5823b73117dd16',
        a.ne2000_source: '52574a058b68d0f1f0328d552706bd3995c316ea965e85c3a57926bf1088d524'}
for path, digest in pins.items():
    if hashlib.sha256(path.read_bytes()).hexdigest() != digest:
        raise SystemExit(f'Unexpected upstream input: {path}')
a.output.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(a.mtcp) as z:
    for name in ['DHCP.EXE', 'PING.EXE', 'DNSTEST.EXE', 'HTGET.EXE', 'COPYING.TXT']:
        data = z.read(name if name == 'COPYING.TXT' else name.lower())
        if len(data) > 256*1024:
            raise SystemExit('Guest utility exceeds staging bound')
        (a.output/name).write_bytes(data)
source = a.ne2000_source.read_text().split('bin_ne2000_com[] = {', 1)[1].split('};', 1)[0]
(a.output/'NE2000.COM').write_bytes(bytes(int(x, 16) for x in re.findall(r'0x([a-fA-F0-9]{2})', source)))
(a.output/'SOURCES.TXT').write_text(
    'mTCP by Michael B. Brutman, GPLv3; see COPYING.TXT.\r\n'
    'https://www.brutman.com/mTCP/\r\nhttps://github.com/mbbrutman/mTCP\r\n'
    'NE2000 packet driver by Crynwr Software. Source and license:\r\n'
    'http://www.crynwr.com/drivers/\r\n'
    'Embedded upstream binary: https://github.com/joncampbell123/dosbox-x\r\n'
    'Revision 378e6f8a25768594df87c9f5c99b5df9b3f6a8b0\r\n')
record = {path.name: {'bytes': path.stat().st_size,
                    'sha256': hashlib.sha256(path.read_bytes()).hexdigest()}
          for path in sorted(a.output.iterdir()) if path.is_file()}
(a.output.parent/'network-tools.json').write_text(json.dumps(record, indent=2)+'\n')
print(f'Staged {len(record)} guest utility/license files; no target changes')
