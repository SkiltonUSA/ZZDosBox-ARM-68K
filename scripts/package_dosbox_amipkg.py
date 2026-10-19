#!/usr/bin/env python3
"""Create offline, signed AmiPkg installers. Signing secrets are external inputs."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import zipfile


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def lha_archive(lha, output, files):
    with tempfile.TemporaryDirectory() as td:
        stage = Path(td)
        for name, data in files.items():
            p = stage / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_bytes(data)
        for path in sorted(stage.rglob('*'), reverse=True):
            os.utime(path, (1792368000, 1792368000))
        if output.exists():
            output.unlink()
        subprocess.run([str(lha), 'aq', str(output.resolve()), *sorted({n.split('/')[0] for n in files})], cwd=stage, check=True, stdout=subprocess.DEVNULL)
    subprocess.run([str(lha), 'tq', str(output)], check=True, stdout=subprocess.DEVNULL)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    p.add_argument('--zip-dir', type=Path, required=True)
    p.add_argument('--client-source', type=Path, required=True)
    p.add_argument('--helper', type=Path, required=True)
    p.add_argument('--signing-key', type=Path, required=True)
    p.add_argument('--lha', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    root, source, out = a.root.resolve(), a.client_source.resolve(), a.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    repo = out / 'repository'; repo.mkdir(exist_ok=True)
    helper = a.helper.read_bytes()
    drawer = bytearray((root / 'artifacts/development/launcher-0.19.7/ZZDosBox-Launcher.info').read_bytes())
    drawer[48] = 2
    drawer[58:66] = struct.pack('>II', 0x80000000, 0x80000000)
    def read_zip(edition):
        with zipfile.ZipFile(a.zip_dir / f'ZZDosBox-0.22.6-beta-{edition}.zip') as z:
            return {n.split('/', 1)[1]: z.read(n) for n in z.namelist()}
    standard, demos = read_zip('standard'), read_zip('demos')
    core = {'Core/' + n: data for n, data in standard.items()
            if n not in ('PC/Launcher.cfg', 'SHA256SUMS.json')}
    core['Core/ZZInstall'] = helper
    addon = {'Demos/DemoDefaults/' + n.removeprefix('PC/'): data
             for n, data in demos.items() if n.startswith('PC/Games/') or n == 'PC/Launcher.cfg'}
    addon.update({'Demos/DemoDefaults/' + n: data for n, data in demos.items() if n.startswith('Originals/')})
    addon['Demos/ZZInstall'] = helper
    archives = {}
    for ident, files in [('zzdosbox', core), ('zzdosbox-demos', addon)]:
        path = repo / f'{ident}-0.22.6.lha'
        lha_archive(a.lha.resolve(), path, files)
        archives[ident] = path
    entries = []
    for ident, folder, mode in [('zzdosbox', 'Core', 'standard'), ('zzdosbox-demos', 'Demos', 'demos')]:
        archive = archives[ident]
        pre = ['FailAt 10', 'Stack 65536', f'ZZPKG:cache/extract/{folder}/ZZInstall check {mode}']
        ops = [dict(op='preScript', lines=pre)]
        if mode == 'standard':
            ops += [dict(op='copyTree', src='Core', dest='ZZDOSDEST:'),
                    dict(op='drawerIcon', dest='ZZDOSDEST:PC.info', src='Core/PC/ZZDosBox-Launcher.info')]
        else:
            ops += [dict(op='copyTree', src='Demos/DemoDefaults', dest='ZZDOSDEST:DemoDefaults')]
        ops += [dict(op='postScript', lines=['FailAt 10', 'Stack 65536', f'ZZDOSDEST:ZZInstall seed {mode} ZZDOSDEST:'])]
        entries.append(dict(id=ident, name='ZZDosBox' + (' Demo Pack' if mode == 'demos' else ''),
                            version='0.22.6-beta', description='ZZ9000 DOS emulator' if mode == 'standard' else 'Corridor 7, Doom and Kings Quest VII demos',
                            category='Emulation', tier='A', archive=dict(url='https://zzdosbox.invalid/offline/' + archive.name,
                            mirrors=[], sizeBytes=archive.stat().st_size, sha256=digest(archive)),
                            requirements=dict(minCPU='68020', minKS='3.1'),
                            deps=[dict(id='zzdosbox', min='0.22.6-beta')] if mode == 'demos' else [],
                            conflicts=[], provides=[], recipe=dict(recipeSchema=2, ops=ops),
                            requiredCapabilities=['copy-tree-v1', 'pre-post-script-v1'] + (['drawer-icon-v1'] if mode == 'standard' else [])))
    signer = source / 'tools/amipkg-repo-sign'
    public = subprocess.check_output(['python3', str(signer), 'pubkey', str(a.signing_key)]).decode().strip()
    for edition in ('standard', 'demos'):
        directory = out / ('ZZDosBox-Install-' + edition)
        if directory.exists():
            shutil.rmtree(directory)
        (directory / 'Tools').mkdir(parents=True)
        (directory / 'Repository').mkdir()
        (directory / 'Licenses').mkdir()
        shutil.copyfile(source / 'amipkg', directory / 'Tools/amipkg')
        shutil.copyfile(a.helper, directory / 'Tools/ZZInstall')
        shutil.copyfile(source / 'LICENSE', directory / 'Licenses/amipkg-APACHE-2.0')
        shutil.copyfile(source / 'NOTICE', directory / 'Licenses/amipkg-NOTICE')
        shutil.copyfile(root / 'LICENSE', directory / 'Licenses/ZZDosBox-GPL')
        shutil.copyfile(root / 'docs/amipkg-installers.md', directory / 'Installer-guide.md')
        shutil.copyfile(root / 'scripts/amipkg-zzinstall.patch', directory / 'Licenses/amipkg-zzinstall.patch')
        catalog = directory / 'Repository/packages.json'
        catalog.write_text(json.dumps(dict(schema=1, indexVersion=1, packages=entries if edition == 'demos' else entries[:1]), indent=2) + '\n')
        subprocess.run(['python3', str(signer), 'sign', str(a.signing_key), str(catalog)], check=True)
        subprocess.run(['python3', str(signer), 'verify', public, str(catalog)], check=True)
        (directory / 'Repository/public.key').write_text(public + '\n')
        for ident in ('zzdosbox', 'zzdosbox-demos') if edition == 'demos' else ('zzdosbox',):
            shutil.copyfile(archives[ident], directory / 'Repository' / archives[ident].name)
        script = f'''.key DEST/K
.bra {{
.ket }}
.def DEST ""
FailAt 5
Stack 65536
If NOT EXISTS C:lha
  Echo "Missing C:lha. Install LhA before continuing."
  Quit 10
EndIf
Tools/ZZInstall check {edition}
Tools/ZZInstall verify Repository/packages.json Repository/packages.json.sig Repository/public.key
If "{{DEST}}" EQ ""
  Set target `Tools/ZZInstall choose`
Else
  Set target "{{DEST}}"
EndIf
If "$target" EQ ""
  Echo "No destination selected."
  Quit 10
EndIf
If NOT EXISTS "$target"
  MakeDir "$target"
EndIf
If NOT EXISTS "$target/PackageManager"
  MakeDir "$target/PackageManager"
EndIf
If NOT EXISTS "$target/PackageManager/db"
  MakeDir "$target/PackageManager/db"
EndIf
Copy Tools/amipkg "$target/PackageManager/amipkg"
Copy Tools/ZZInstall "$target/PackageManager/ZZInstall"
Protect "$target/PackageManager/amipkg" +e
Protect "$target/PackageManager/ZZInstall" +e
Assign ZZPKG: "$target/PackageManager"
Assign ZZDOSDEST: "$target"
FailAt 21
ZZPKG:amipkg repo remove zzdist >NIL:
FailAt 5
ZZPKG:amipkg repo add zzdist https://zzdosbox.invalid/offline {public}
ZZPKG:amipkg repo disable official
If NOT EXISTS ZZPKG:repos
  MakeDir ZZPKG:repos
EndIf
If NOT EXISTS ZZPKG:repos/zzdist
  MakeDir ZZPKG:repos/zzdist
EndIf
Copy Repository/packages.json ZZPKG:repos/zzdist/packages.json
Copy Repository/packages.json.sig ZZPKG:repos/zzdist/packages.json.sig
Copy Repository/#?.lha ZZPKG:cache/
ZZPKG:amipkg install zzdist:zzdosbox
ZZDOSDEST:ZZInstall seed standard ZZDOSDEST:
'''
        if edition == 'demos':
            script += 'ZZPKG:amipkg install zzdist:zzdosbox-demos\nZZDOSDEST:ZZInstall seed demos ZZDOSDEST:\n'
        script += 'If NOT EXISTS "$target.info"\n  Copy Drawer.info "$target.info"\nEndIf\n'
        script += 'Echo "Installation complete. Open PC/ZZDosBox-Launcher in the selected drawer."\n'
        (directory / 'Install').write_text(script)
        (directory / 'Drawer.info').write_bytes(drawer)
        subprocess.run(['python3', str(source / 'dist/mkprojicon.py'),
                        str(root / 'artifacts/development/launcher-0.19.7/ZZDosBox-Launcher.info'),
                        str(directory / 'Install.info'), 'C:IconX'], check=True)
        (directory / 'ReadMe.txt').write_text(f'''ZZDosBox 0.22.6 Beta - AmiPkg {edition} installer

Open Install from this drawer. Choose the parent drawer for ZZDosBox.
Shell alternative (from this drawer): Execute Install DEST "SD032G:Emulator/ZZDosBox"
Requires C:lha and C:IconX (Workbench launch). No network connection is needed.

MUI 3.8+, Workbench v44+, ASL v38+, keymap v37+, Picasso96API v2+ and
ZZ9000 graphics/Fast RAM boards are checked before files are installed.
AHI v4+, installed audio modes and unit 0 are required by the demo edition;
Standard reports missing audio but permits sound-off use. CAMD is optional.
Missing dependencies are reported, not downloaded or replaced. Install them
with their own official installers and rerun this installer.
Close ZZDosBox and other Core1 applications before installation.

Catalogue Ed25519 signatures and archive SHA256 are verified locally.
Existing Launcher.cfg, dosbox.conf, saves and game configuration are preserved.
Demo files are seeded only when absent; demo profiles are merged without
replacing existing profiles. Launcher.install-backup retains a pre-merge library.
PackageManager contains a private AmiPkg client, receipts and verified cache;
the system AmiPkg installation and startup scripts are untouched.
Two temporary assigns, ZZPKG: and ZZDOSDEST:, remain available until reboot.
Rerun Install to upgrade; do not use 'update' against the offline placeholder URL.

The bundled client is amipkg 0.7.11-zzinstall1, locally patched for custom
repository recipe lookup, assign-root paths, resolved receipt paths and
prerequisite checks before upgrade removal.
This is not an upstream amipkg release. The original copy/upgrade engine has
no full transactional rollback; retain backups before upgrading an installation.
Post-install seeding errors are also checked by this wrapper.
Use PackageManager/amipkg doctor to verify owned program files.
Uninstalling packages intentionally leaves user settings, seeded games and saves.

No firmware or global system libraries are installed. XACP firmware, available
contiguous RAM, display modes and audible output are checked separately at runtime.
Emulator source and client patch: https://github.com/SkiltonUSA/ZZDosBox-ARM-68K
Upstream AmiPkg: https://github.com/thomas-luebker/amipkg (Apache-2.0)
Game media retains publisher copyrights; see the demo edition documentation.
''')
        files = {str(f.relative_to(directory.parent)): f.read_bytes() for f in directory.rglob('*') if f.is_file()}
        files[directory.name + '.info'] = bytes(drawer)
        archive = out / f'ZZDosBox-0.22.6-beta-{edition}-amipkg.lha'
        lha_archive(a.lha.resolve(), archive, files)
        archive.with_suffix('.sha256').write_text(digest(archive) + '  ' + archive.name + '\n')
        print(f'{archive}: {archive.stat().st_size:,} bytes')


if __name__ == '__main__':
    main()
