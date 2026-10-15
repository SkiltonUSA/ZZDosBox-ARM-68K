#!/usr/bin/env python3
"""Package the 0.19.12 RC1 incremental update from versioned public artifacts."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
p.add_argument('--output', type=Path)
a = p.parse_args()
root = a.root.resolve()
out = (a.output or root / '.context/releases').resolve()
out.mkdir(parents=True, exist_ok=True)
version = '0.19.12-RC1'
folder = 'ZZDosBox-Update'
latest = root / 'artifacts/development/launcher-0.19.12'
support = root / 'artifacts/development/launcher-0.19.7'
files = {name: latest / name for name in ['ZZDosBox-Launcher', 'ZZHELLO2.COM']}
files.update({f.name: f for f in sorted((latest / 'runtimes').iterdir()) if f.is_file()})
for name in ['ZZDosBox-Launcher.info', 'ZZDosDisk', 'ZZDosCreate', 'Compatibility.dat',
             'Compatibility.md', 'Compatibility-Details.md']:
    files[name] = support / name
files['Network.guide'] = root / 'artifacts/development/launcher-0.19.8/Network.guide'
files['LICENSE'] = root / 'LICENSE'
files['Changelog.md'] = root / 'CHANGELOG.md'
for f in sorted((root / 'licenses').iterdir()):
    if f.is_file():
        files['Licenses/' + f.name] = f
record = json.loads((latest / 'launcher-build.json').read_text())
assert hashlib.sha256(files['ZZDosBox-Launcher'].read_bytes()).hexdigest() == record['sha256']
for name, meta in record['assets'].items():
    if name in files:
        assert hashlib.sha256(files[name].read_bytes()).hexdigest() == meta['sha256'], name
readme = '''ZZDosBox 0.19.12 RC1 - incremental update

Requires an existing ZZDosBox installation, MUI 3.8+, AmigaOS 3.2.3
(the tested version), ZZ9000 with XX19c/XACP, and Picasso96API.library v2+.
The launcher identifies itself as 0.19.12-dev; RC1 is the package revision.

INSTALL
Close the launcher and player. Keep a copy of your existing executables.
Copy the contents of ZZDosBox-Update into the existing application drawer.
Keep existing Network/Disks support, LOADFIX.COM and ZZDOSCD.SYS.
Your Launcher.cfg, dosbox.conf, games, saves and disk images are not included
or replaced. Keep the previous player binaries until you test this candidate.

CHANGES
- Yellow ASCII welcome title, red shortcut keys, white descriptions; no URL.
- Ctrl+F5 screenshot instructions on the DOS welcome and loading screens.
- Per-program screenshot preview and Capture button in the library.
- Rename/Delete controls, protected OS/shared installations, footer help.
- Loading display handoff, mounted images in OS, keyboard selection,
  About changelog, disk-format guidance and footer revision.

CAPTURE
Capture launches the selected program. Ctrl+F5 saves its display;
F10 returns to the library. Captures are local PPM files in Snapshots.
Alt+Return switches window/fullscreen; Ctrl+minus/plus adjusts CPU cycles.

STATUS
Build and native checks passed. Physical screenshot capture, footer hover
and player exit checks remain pending after the target connection stopped
responding. This candidate is not a fully hardware-validated replacement.
ARM payloads are unchanged; no new performance result is claimed.

Documentation, requirements, validation and corresponding source:
https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/releases/tag/v0.19.12-rc1
https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/tree/v0.19.12-rc1

CREDITS
DOSBox/libretro contributors; Xanai/XACP; Stefan Stuntz/MUI;
TinyGL, libslirp and other contributors listed in Licenses/README.md.
Development: Codex assisted, Amiga-Bridge MCP.
No games, Microsoft software, personal profiles or third-party Amiga
system libraries are bundled. See LICENSE and Licenses/ for component terms.
'''
entries = {name: f.read_bytes() for name, f in files.items()}
entries['ReadMe.txt'] = readme.encode('ascii')
manifest = {n: hashlib.sha256(b).hexdigest() for n, b in sorted(entries.items())}
entries['SHA256SUMS.txt'] = ''.join(f'{v}  {n}\n' for n, v in manifest.items()).encode()
archive = out / f'ZZDosBox-{version}-update.zip'
with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
    for name, data in sorted(entries.items()):
        info = zipfile.ZipInfo(folder + '/' + name, (2026, 10, 15, 0, 0, 0))
        info.create_system = 3
        executable = name.startswith('ZZDos') and not name.endswith('.info')
        info.external_attr = (0o100755 if executable else 0o100644) << 16
        info.compress_type = zipfile.ZIP_DEFLATED
        z.writestr(info, data, compresslevel=9)
with zipfile.ZipFile(archive) as z:
    assert z.testzip() is None
    assert set(z.namelist()) == {folder + '/' + n for n in entries}
    for name, digest in manifest.items():
        assert hashlib.sha256(z.read(folder + '/' + name)).hexdigest() == digest
assert not any(n in entries for n in ['Launcher.cfg', 'dosbox.conf', 'DOS622.IMG'])
digest = hashlib.sha256(archive.read_bytes()).hexdigest()
(out / f'ZZDosBox-{version}-SHA256SUMS.txt').write_text(f'{digest}  {archive.name}\n')
print(f'{archive}: {archive.stat().st_size} bytes, {len(entries)} files')
print('SHA256 ' + digest)
