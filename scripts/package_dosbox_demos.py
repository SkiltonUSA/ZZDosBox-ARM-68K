#!/usr/bin/env python3
"""Build matching ZZDosBox standard and demo editions."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile
import zipfile

from package_dosbox_shareware import sha, unpack

VERSION = '0.22.6-beta'
INPUTS = {
    'corr7.zip': 'a6e73bead887ab588e084cf53b7dc476e9b220c595d8f429199f99c848a5d3f0',
    'doom19s.zip': 'cacf0142b31ca1af00796b4a0339e07992ac5f21bc3f81e7532fe1b5e1b486e6',
}


def demo_entries(root, media, kq7):
    originals = {}
    for name, digest in INPUTS.items():
        data = (media / name).read_bytes()
        if sha(data) != digest:
            raise ValueError('Wrong demo archive: ' + name)
        originals[name] = data
    expected = json.loads((root / 'scripts/kq7-demo-files.json').read_text())
    # The manifest describes the original extracted demo, not a live game drawer.
    # Reject extra files so saves/configuration from a personal install cannot leak.
    if {p.name for p in kq7.iterdir()} != set(expected):
        raise ValueError('KQ7 input must contain exactly the pinned original files')
    king = {}
    for name, digest in expected.items():
        data = (kq7 / name).read_bytes()
        if sha(data) != digest:
            raise ValueError('Changed KQ7 demo file: ' + name)
        king[name] = data
    doom = unpack(originals['doom19s.zip'])
    games = {'CORR7': unpack(originals['corr7.zip']),
             'DOOM19': unpack(doom['DOOMS_19.1'] + doom['DOOMS_19.2']), 'KQ7': king}
    entries = {f'PC/Games/{game}/{name}': data
               for game, files in games.items() for name, data in files.items()}
    entries.update({'Originals/' + n: b for n, b in originals.items()})
    return entries


def build(root, media, kq7, output, edition='demos'):
    if edition not in ('standard', 'demos'):
        raise ValueError('Unknown edition: ' + edition)
    entries = demo_entries(root, media, kq7) if edition == 'demos' else {}
    dev = root / 'artifacts/development'
    support = dev / 'launcher-0.19.7'
    files = {'ZZDosBox-Launcher': dev / 'launcher-0.22.6/ZZDosBox-Launcher',
             'ZZDosBox-Compat': dev / 'descent-startup/ZZDosBox-Compat',
             'ZZDosBox-Expanded': dev / 'expanded-0.22.5/ZZDosBox-Expanded',
             'ZZDosBox-Net-JIT': dev / 'runtime-0.22.2/ZZDosBox-Net-JIT',
             'Network.guide': dev / 'runtime-0.22.2/Network.guide',
             'ZZHELLO2.COM': dev / 'launcher-0.19.12/ZZHELLO2.COM',
             'ZZDOSCD.SYS': root / 'artifacts/0.10.1-dev/pc/ZZDOSCD.SYS'}
    for name in ('ZZDosBox-Launcher.info', 'ZZDosDisk', 'ZZDosCreate',
                 'Compatibility.dat', 'Compatibility.md', 'Compatibility-Details.md'):
        files[name] = support / name
    for path in (dev / 'launcher-0.19.12/runtimes').iterdir():
        if path.is_file():
            files[path.name] = path
    entries.update({'PC/' + n: p.read_bytes() for n, p in files.items()})
    entries['LICENSE'] = (root / 'LICENSE').read_bytes()
    entries['Changelog.md'] = (root / 'CHANGELOG.md').read_bytes()
    for path in (root / 'licenses').iterdir():
        if path.is_file():
            entries['Licenses/' + path.name] = path.read_bytes()
    for name in ('demo-edition.md', 'requirements.md'):
        entries['docs/' + name] = (root / 'docs' / name).read_bytes()
    with tempfile.TemporaryDirectory() as directory:
        temp = Path(directory)
        subprocess.run(['cc', '-std=c99', '-I', str(root),
                        str(root / 'scripts/dosbox_demo_profiles.c'),
                        '-o', str(temp / 'profiles')], check=True)
        subprocess.run([str(temp / 'profiles'), str(temp / 'Launcher.cfg'), edition], check=True)
        entries['PC/Launcher.cfg'] = (temp / 'Launcher.cfg').read_bytes()
    entries['ReadMe.txt'] = f'''ZZDosBox {VERSION} - demo edition

Extract into a NEW writable drawer, then open PC/ZZDosBox-Launcher.
The Games library contains Corridor 7 Demo, Doom 1.9 Shareware and
King's Quest VII Demo. Select a game and Launch.
Do not overwrite an existing Launcher.cfg with this starter library.

Corridor 7: 8 MB, VGA, 6000 cycles, EMS, SB Pro2, windowed.
Doom 1.9 shareware: 8 MB, VGA, 6000 cycles, sound off, fullscreen.
King's Quest VII: 8 MB, S3/VESA, 3000 cycles, SB Pro2, windowed.
King's Quest is a non-interactive promotional demo. Allow startup time;
press Return at Include scrolling? to continue. Keep SB Pro2 enabled:
its original configuration requests AdLib/Sound Blaster drivers.

F10 exits. Alt+Return switches window/fullscreen. Ctrl+F5 captures.
Requires AmigaOS, MUI 3.8+, ZZ9000 with compatible XACP firmware and RTG.
A TF4060 is not required. See docs/requirements.md for tested requirements.
No firmware, MUI/AHI system libraries or Microsoft DOS/Windows are included.
Optional networking guest tools and LOADFIX are supplied separately;
none of these three demo profiles needs them.

Game files and included notices are preserved from the pinned demo media.
Games remain copyrighted by their publishers; the emulator GPL does not
license their data. No personal profiles or saves are included.
See docs/demo-edition.md for provenance and verification scope.
Corresponding emulator source: https://github.com/SkiltonUSA/ZZDosBox-ARM-68K
'''.encode('ascii')
    if edition == 'standard':
        entries['ReadMe.txt'] = f'''ZZDosBox {VERSION} - standard edition (no demos)

Extract into a NEW writable drawer, then open PC/ZZDosBox-Launcher.
The library starts empty. Use Load Program to add your own games and apps.
Keep your existing Launcher.cfg when updating an existing installation.

The launcher, runtimes and support files match the demo edition exactly.
No game payloads, demo installers, personal saves or Microsoft OS are included.
Requires AmigaOS, MUI 3.8+, ZZ9000/XACP and RTG; see docs/requirements.md.
System libraries, firmware, optional guest networking tools and LOADFIX
are supplied separately.

F10 exits. Alt+Return switches window/fullscreen. Ctrl+F5 captures.
See docs/demo-edition.md for the two package options.
Corresponding source: https://github.com/SkiltonUSA/ZZDosBox-ARM-68K
'''.encode('ascii')
    manifest = {n: sha(b) for n, b in sorted(entries.items())}
    entries['SHA256SUMS.json'] = (json.dumps(manifest, indent=2) + '\n').encode()
    output.mkdir(parents=True, exist_ok=True)
    archive = output / f'ZZDosBox-{VERSION}-{edition}.zip'
    folder = 'ZZDosBox-Demos' if edition == 'demos' else 'ZZDosBox'
    with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for name, data in sorted(entries.items()):
            info = zipfile.ZipInfo(folder + '/' + name, (2026, 10, 19, 0, 0, 0))
            info.create_system = 3
            executable = name.startswith('PC/ZZDos') and not name.endswith('.info')
            info.external_attr = (0o100755 if executable else 0o100644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info, data, compresslevel=9)
    with zipfile.ZipFile(archive) as z:
        assert z.testzip() is None
        assert len(z.namelist()) == len(entries)
        for name, digest in manifest.items():
            assert sha(z.read(folder + '/' + name)) == digest, name
    archive.with_suffix('.sha256').write_text(sha(archive.read_bytes()) + '  ' + archive.name + '\n')
    print(f'{archive}: {archive.stat().st_size:,} bytes; {len(entries)} files; hashes verified')
    return archive


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--edition', choices=('standard', 'demos'), default='demos')
    parser.add_argument('--media', type=Path, help='Demos only: corr7.zip and doom19s.zip')
    parser.add_argument('--kq7-dir', type=Path, help='Demos only: pinned original extracted KQ7 demo')
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if args.edition == 'demos' and (args.media is None or args.kq7_dir is None):
        parser.error('--media and --kq7-dir are required for the demo edition')
    build(args.root.resolve(), args.media, args.kq7_dir, args.output, args.edition)
