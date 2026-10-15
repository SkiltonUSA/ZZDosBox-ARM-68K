#!/usr/bin/env python3
"""Test shared console synchronization, presentation and backend receive policy."""
from pathlib import Path
import subprocess
from prepare_dosbox_network import prepare

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / '.context/amiga/dosbox-review/tests'
OUT.mkdir(parents=True, exist_ok=True)
flags = ['-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=all']
includes = ['amiga/dosbox/console', 'amiga/dosbox/dos', 'amiga/dosbox/pc',
            'amiga/common/arm', 'amiga/arm_debug', 'amiga/arm_debug/zz9000']
for name, source, extra in [
    ('stream', 'tests/amiga/dosbox/stream_host.c', []),
    ('present', 'tests/amiga/dosbox/present_plan.c', []),
    ('flip', 'tests/amiga/common/flip_plan.c', ['-Iamiga/common']),
]:
    exe = OUT / name
    subprocess.run(['clang', '-std=c99', '-Wall', '-Wextra', '-Werror', *flags,
                    *['-I'+p for p in includes], *extra, source, '-o', str(exe)], cwd=ROOT, check=True)
    subprocess.run([str(exe)], check=True)

slirp = prepare(OUT / 'slirp')
sources = sorted(p for p in slirp.glob('*.c') if p.stem not in ('state', 'stream', 'vmstate', 'version'))
exe = OUT / 'receive'
subprocess.run(['clang', '-std=gnu99', *flags, '-I'+str(slirp),
                '-Iamiga/dosbox/network/compat', '-Iamiga/dosbox/network',
                'tests/amiga/dosbox/network/receive_test.c', *map(str, sources),
                '-lresolv', '-o', str(exe)], cwd=ROOT, check=True)
subprocess.run([str(exe)], check=True)
