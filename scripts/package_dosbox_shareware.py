#!/usr/bin/env python3
"""Build a ZZDosBox starter drawer with complete, pinned shareware payloads.

Descent is restricted to an explicitly personal build; never publish that ZIP.
Inputs are supplied separately, not harvested from a running installation.
"""
import argparse
import hashlib
import io
import json
from pathlib import Path
import subprocess
import tempfile
import zipfile

INPUTS = {
    'doom19s.zip': 'cacf0142b31ca1af00796b4a0339e07992ac5f21bc3f81e7532fe1b5e1b486e6',
    'DN3DSW13.SHR': 'a6c010e735a8f8d910807d41985f16379583cd9d81f14a1e4634f30d04b881e4',
    'DOSBOX_DESCENT.ZIP': 'ec3f0f4d3a420a38a638ab37ee6defb044db0d264a9f5551c921d1256afed070',
    'DESCENT-LICENCE.TXT': '963088fea6cafbcb913a44b7ecd5bedcd3fed097d1ccce52c6d21ed5210a7395',
}


def sha(data):
    return hashlib.sha256(data).hexdigest()


def unpack(data):
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        result = {}
        for item in archive.infolist():
            if item.is_dir():
                continue
            name = item.filename.upper()
            if '/' in name or '\\' in name or ':' in name or name in result:
                raise ValueError('Unexpected archive member: ' + name)
            result[name] = archive.read(item)
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--media', type=Path, required=True)
    parser.add_argument('--runtime-dir', type=Path, required=True,
                        help='Directory containing the tested Compat and Expanded binaries')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--personal-descent', action='store_true')
    args = parser.parse_args()
    root = args.root.resolve()
    media = {}
    for name, digest in INPUTS.items():
        if not args.personal_descent and name in ('DOSBOX_DESCENT.ZIP', 'DESCENT-LICENCE.TXT'):
            continue
        data = (args.media / name).read_bytes()
        if sha(data) != digest:
            raise ValueError('Unrecognised shareware input: ' + name)
        media[name] = data
    doom = unpack(media['doom19s.zip'])
    games = {
        'DOOM19': unpack(doom['DOOMS_19.1'] + doom['DOOMS_19.2']),
        'DUKE3DS': unpack(media['DN3DSW13.SHR']),
    }
    if args.personal_descent:
        games['DCNTSHR'] = unpack(media['DOSBOX_DESCENT.ZIP'])
    entries = {f'PC/Games/{game}/{name}': data
               for game, files in games.items() for name, data in files.items()}
    # Preserve the supplied installers as well as every decompressed member.
    entries.update({'Originals/' + name: data for name, data in media.items()})
    latest = root / 'artifacts/development/runtime-0.22.2'
    support = root / 'artifacts/development/launcher-0.19.7'
    for name in ('ZZDosBox-Launcher', 'Network.guide'):
        entries['PC/' + name] = (latest / name).read_bytes()
    for name in ('ZZDosBox-Compat', 'ZZDosBox-Expanded'):
        entries['PC/' + name] = (args.runtime_dir / name).read_bytes()
    # Doom's existing sound-off profile uses the normal runtime.
    entries['PC/ZZDosBox'] = (root / 'artifacts/development/launcher-0.19.12/runtimes/ZZDosBox').read_bytes()
    for name in ('ZZDosBox-Launcher.info', 'ZZHELLO2.COM'):
        source = root / 'artifacts/development/launcher-0.19.12' / name
        if not source.exists():
            source = support / name
        entries['PC/' + name] = source.read_bytes()
    entries['LICENSE'] = (root / 'LICENSE').read_bytes()
    for name in ('shareware-build.md', 'amiga-dosbox-shooter-startup-review.md', 'requirements.md'):
        entries['docs/' + name] = (root / 'docs' / name).read_bytes()
    for source in (root / 'licenses').iterdir():
        if source.is_file():
            entries['Licenses/' + source.name] = source.read_bytes()
    # Use the launcher's serializer, rather than maintaining another CFG format.
    with tempfile.TemporaryDirectory() as directory:
        temp = Path(directory)
        subprocess.run(['cc', '-std=c99', '-I', str(root),
                        str(root / 'scripts/dosbox_shareware_profiles.c'),
                        '-o', str(temp / 'profiles')], check=True)
        subprocess.run([str(temp / 'profiles'), str(temp / 'Launcher.cfg'),
                        'personal' if args.personal_descent else 'public'], check=True)
        entries['PC/Launcher.cfg'] = (temp / 'Launcher.cfg').read_bytes()
    scope = 'PERSONAL BUILD - DO NOT PUBLISH (includes Descent)' if args.personal_descent else 'Shareware starter build'
    entries['ReadMe.txt'] = (scope + '''

Extract into a NEW writable drawer. Open PC/ZZDosBox-Launcher.
Do not copy this Launcher.cfg over your existing library.
Requires the usual ZZDosBox AmigaOS/MUI, ZZ9000/XACP and RTG setup.
This is a game starter drawer; advanced disk/network/developer support files
remain part of the full installation. No Microsoft DOS media is included.

Doom 1.9: 8 MB, normal core, sound off (verified attract demo).
Duke Nukem 3D 1.3D: 8 MB, 486 guest, EMS, SB16. Run SETUP.EXE first
to choose 320x200 and Sound Blaster 16, port 220, IRQ 7, DMA 1/5.
Descent, when included: 16 MB, EMS, SB Pro 2; LOADFIX/VCPI/paging JIT off.
Its startup reaches pilot selection; later physical gameplay remains unverified.

Game files are preserved byte-for-byte from the pinned input archives, including
their notices. No personal saves, pilots, test helpers or custom game CFGs were
added. Configure controls/audio through each game's SETUP.EXE if needed.
Games retain their publishers' terms; ZZDosBox's GPL does not cover game data.
Descent's original installer licence restricts bundling despite its README's
broader sharing language. Personal builds must not be uploaded as releases.
Originals/ retains the source installers and additional Descent licence.

F10 exits. Alt+Return switches window/fullscreen. Ctrl+F5 captures the display.
See the repository docs/shareware-build.md for packaging and provenance.
''').encode('ascii')
    manifest = {name: sha(data) for name, data in sorted(entries.items())}
    entries['SHA256SUMS.json'] = (json.dumps(manifest, indent=2) + '\n').encode()
    prefix = 'ZZDosBox-Personal' if args.personal_descent else 'ZZDosBox-Shareware'
    args.output.mkdir(parents=True, exist_ok=True)
    archive = args.output / (prefix + '-0.22.2-dev.zip')
    with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for name, data in sorted(entries.items()):
            info = zipfile.ZipInfo(prefix + '/' + name, (2026, 10, 18, 0, 0, 0))
            info.create_system = 3
            executable = name.startswith('PC/ZZDosBox') and not name.endswith('.info')
            info.external_attr = (0o100755 if executable else 0o100644) << 16
            info.compress_type = zipfile.ZIP_DEFLATED
            z.writestr(info, data, compresslevel=9)
    with zipfile.ZipFile(archive) as z:
        assert z.testzip() is None
        assert len(z.namelist()) == len(entries)
        for name, digest in manifest.items():
            assert sha(z.read(prefix + '/' + name)) == digest, name
    archive.with_suffix('.sha256').write_text(sha(archive.read_bytes()) + '  ' + archive.name + '\n')
    print(f'{archive}: {archive.stat().st_size:,} bytes; {len(entries)} files; all hashes verified')


if __name__ == '__main__':
    main()
