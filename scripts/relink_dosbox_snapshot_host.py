#!/usr/bin/env python3
"""Relink an archived screenshot host with its unchanged embedded ARM payload."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

from amiga import PIN, container_command

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('inputs', type=Path, help='Variant directory containing snapshot-build.json and generated headers')
parser.add_argument('--container-command')
parser.add_argument('--network-library', type=Path, default=Path('.context/amiga/dosbox-network/amiga/libzznetwork.a'))
args = parser.parse_args()
folder = args.inputs.resolve().relative_to(ROOT)
record = json.loads((ROOT / folder / 'snapshot-build.json').read_text())
flags = record['flags']
defines = []
for key, name in [('pc_network', 'DB_NETWORK'), ('pc_memory_debug', 'DB_MEMORY_DEBUG'),
                  ('pc_video_trace', 'DB_VIDEO_TRACE'), ('pc_opengl', 'DB_OPENGL'),
                  ('pc_scanlines', 'DB_SCANLINE_TEST'), ('pc_presentation', 'DB_PRESENT_TEST'),
                  ('pc_pipeline', 'DB_PIPELINE_DEFAULT=1')]:
    if flags[key]:
        defines.append('-D' + name)
includes = [str(folder), 'amiga/dosbox/pc', 'amiga/arm_debug',
            'amiga/arm_debug/zz9000', 'amiga/common/arm', 'amiga/dosbox/dos',
            'amiga/dosbox/console', 'amiga/dosbox/shell', 'amiga/dosbox/developer']
sources = [str(folder / 'launcher.c'), str(folder / 'host.c'),
           'amiga/dosbox/console/window.c', 'amiga/dosbox/shell/fs_host.c',
           'amiga/dosbox/pc/display.c', 'amiga/dosbox/pc/cd_host.c',
           'amiga/dosbox/pc/midi_host.c']
if flags['pc_network']:
    sources.append(str(args.network_library.resolve().relative_to(ROOT)))
binary = folder / 'ZZDosBox'
subprocess.run([
    *(json.loads(args.container_command) if args.container_command else container_command()), 'run', '--rm', '--platform', 'linux/amd64',
    '-v', f'{ROOT}:/work', '-w', '/work', PIN['image'], 'm68k-amigaos-gcc',
    '-std=c99', '-noixemul', '-m68020', '-O2', '-g', '-Wall', '-Wextra', '-Werror',
    *defines, '-DFF_DIRECT', '-DZZ_PC_CD', '-DZZ_HEADLESS_BENCH', '-DZZ_RELEASE',
    '-DZZ_BLOCK_SIZE=0x2000000', '-DZZ_CONTROL=0x180000', '-DMB_ALLOCATION=0x2000000u',
    '-DZZ_APP_NAME="ZZDosBox"', '-DZZ_APP_VERSION="' + record['host_version'] + '"',
    '-DZZ_MIN_STACK=65536', '-DMB_MMU', '-DMB_DCACHE', '-DZZ_HEADLESS_MMU',
    '-DZZ_HEADLESS_ICACHE', *('-I' + p for p in includes), *sources,
    '-lamiga', '-o', str(binary),
], cwd=ROOT, check=True)
print(binary, hashlib.sha256((ROOT / binary).read_bytes()).hexdigest())
