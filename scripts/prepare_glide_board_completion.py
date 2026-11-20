#!/usr/bin/env python3
"""Apply the experimental completion backport to a verified COPY of XX19d-RIX RTL."""
import argparse
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXPERIMENT = ROOT / 'amiga/dosbox/glide/experimental'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rtl', required=True, type=Path,
                        help='Published fpga/RasterIX_G1/RasterIX/rtl directory')
    parser.add_argument('--output', required=True, type=Path,
                        help='New generated directory; must not already exist')
    args = parser.parse_args()
    if args.output.resolve().is_relative_to(args.rtl.resolve()):
        parser.error('--output must be outside the source RTL tree')
    manifest = json.loads((EXPERIMENT / 'board-source-manifest.json').read_text())
    changes = json.loads((EXPERIMENT / 'board-completion-sources.json').read_text())

    # Reject both modified and extra RTL before copying or applying the patch.
    actual = {}
    for component, source in [('RasterIX', args.rtl), ('Float', args.rtl / 'Float/rtl')]:
        actual.update({component + '/' + str(path.relative_to(source)): digest(path)
                       for path in source.rglob('*') if path.is_file()})
    if actual != manifest['source_sha256']:
        raise SystemExit('Source tree does not match the pinned XX19d-RIX manifest')

    args.output.mkdir(parents=True)
    shutil.copytree(args.rtl, args.output / 'RasterIX/rtl')
    shutil.copytree(args.rtl / 'Float/rtl', args.output / 'Float/rtl')
    for name, hashes in changes.items():
        if digest(args.output / name) != hashes['before']:
            raise SystemExit('Unexpected patch input: ' + name)
    subprocess.run(['patch', '-p1', '--batch', '--forward', '-i',
                    str(EXPERIMENT / 'board-completion.patch')],
                   cwd=args.output, check=True)
    for name, hashes in changes.items():
        if digest(args.output / name) != hashes['after']:
            raise SystemExit('Unexpected patch output: ' + name)
    print('Prepared experimental renderer; no board project or firmware changed.')


if __name__ == '__main__':
    main()
