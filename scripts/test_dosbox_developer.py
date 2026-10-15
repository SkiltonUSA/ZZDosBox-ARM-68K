#!/usr/bin/env python3
"""Native sanitizer tests for Developer options, trace transport and controls."""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'.context/amiga/dosbox-video-trace'
OUT.mkdir(parents=True, exist_ok=True)
includes = ['tests/amiga/dosbox/debug_mock', 'amiga/dosbox/launcher',
            'amiga/dosbox/pc', 'amiga/dosbox/console', 'amiga/dosbox/dos',
            'amiga/common/arm', 'amiga/arm_debug', 'amiga/arm_debug/zz9000']
for name in ('developer', 'video_trace_host', 'video_debug_host', 'launcher'):
    binary = OUT/('test-'+name)
    subprocess.run(['clang', '-std=c99', '-fsanitize=address,undefined',
                    '-fno-sanitize-recover=all', *['-I'+p for p in includes],
                    'tests/amiga/dosbox/'+name+'.c', '-o', str(binary)],
                   cwd=ROOT, check=True)
    subprocess.run([str(binary)], cwd=ROOT, check=True)
