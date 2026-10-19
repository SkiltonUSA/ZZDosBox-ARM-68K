#!/usr/bin/env python3
"""Native layout checks; optionally build a read-only Amiga MUI layout probe.

Run build_dosbox_launcher.py first. The probe uses its own MUI application/port
and copies of Launcher.cfg, dosbox.conf and Compatibility.dat in a scratch
drawer. It writes layout-report.txt, visits every section, resizes the window,
checks the loading card and exits without saving or starting an ARM worker.
It constrains only the window, never the Workbench screen mode.
"""
import argparse
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-probe', choices=['640x480', '800x600', '1024x768'])
parser.add_argument('--capture-seconds', type=int, default=0, choices=range(61))
args = parser.parse_args()
out = ROOT / '.context/amiga/launcher-scalable'
out.mkdir(parents=True, exist_ok=True)
exe = out / 'test-layout'
subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                'tests/amiga/dosbox/launcher_layout.c', '-o', str(exe)], cwd=ROOT, check=True)
subprocess.run([str(exe)], check=True)
if args.build_probe:
    width, height = args.build_probe.split('x')
    image = json.loads((ROOT / 'amiga/upstream.json').read_text())['image']
    target = out / ('Layout' + width)
    subprocess.run([
        'podman', '--connection', 'nuflix-converter-root', 'run', '--rm',
        '--platform', 'linux/amd64', '-v', f'{ROOT}:/work', '-w', '/work', image,
        'm68k-amigaos-gcc', '-std=c99', '-noixemul', '-m68020', '-O2', '-g',
        '-Wall', '-Wextra', '-Werror', '-Wno-unused-function',
        '-Wno-misleading-indentation', '-fwrapv', '-DZZ_UI_LAYOUT_PROBE',
        f'-DZZ_UI_TEST_WIDTH={width}', f'-DZZ_UI_TEST_HEIGHT={height}',
        f'-DZZ_UI_CAPTURE_TICKS={args.capture_seconds * 50}',
        '-Iamiga/dosbox/developer', '-I.context/amiga/dosbox-launcher',
        'amiga/dosbox/launcher/main.c', 'amiga/dosbox/pc/cd_host.c',
        '.context/amiga/dosbox-launcher/disasm.c', '-lamiga',
        '-o', str(target.relative_to(ROOT)),
    ], cwd=ROOT, check=True)
    print(target)
