# AmiPkg installers

[Download 0.22.7 Beta](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/releases/tag/v0.22.7-beta). Both editions contain the same
launcher, emulator runtimes, utilities and compatibility database.

| Edition | Size | Library | Audio requirement |
| --- | --- | --- | --- |
| Standard | 7.2 MB | Empty; add your own games and applications | AHI checked, but sound-off use is allowed |
| With Demos | 23.7 MB | Corridor 7, Doom 1.9 shareware and King's Quest VII Demo, with tested profiles | AHI v4+, output drivers and accessible unit 0 |

With Demos is a complete installer; Standard is not needed first. King's Quest
VII is a non-interactive promotional demo. Neither edition includes Microsoft
DOS/Windows, system libraries or ZZ9000 firmware.

Extract the chosen `.lha` archive, open its drawer and double-click **Install**.
Choose a parent drawer; the application is installed in its `ZZDosBox` drawer.
For an exact destination, open a Shell in the extracted installer drawer:

```text
Execute Install DEST "SD032G:Emulator/ZZDosBox"
```

Open `PC/ZZDosBox-Launcher` in the destination after installation.
`C:lha` is required; Workbench launch also uses `C:IconX`.
No network connection, Microsoft operating system or Mac is required.

## Launcher update in 0.22.7

Both editions include the scalable 0.22.7-dev launcher: compact layout for
smaller Workbench screens, independently scrollable pages and resizable content.
The 640×480 layout was verified with a constrained window on the Amiga; no
Workbench screen mode was changed during testing. Emulator runtimes and demo
profiles are unchanged. Run Install against the existing destination to upgrade.

## Dependency checks

| Check | Standard | With Demos |
| --- | --- | --- |
| MUI 3.8+ (`muimaster.library` v19+) | Required | Required |
| Workbench v44+, ASL v38+, keymap v37+ | Required | Required |
| `Picasso96API.library` v2+ | Required | Required |
| ZZ9000 graphics and Fast RAM board configuration | Required | Required |
| AHI v4+, registered audio modes and accessible unit 0 | Reported; sound-off use allowed | Required for the supplied audio-enabled profiles |
| CAMD v37+ | Optional MIDI information | Optional MIDI information |
| Closed ZZDosBox launcher and no active Core1 task | Required | Required |

Missing dependencies produce an explanation and stop installation where required.
The installer does **not** replace system libraries or install drivers silently.
Use their official installers, configure AHI preferences if necessary, and retry.
No sound is played during detection. Finding modes and opening unit 0 does not
prove audible output or require a ZZ9000AX. Firmware compatibility, display modes
and sufficient contiguous ZZ9000 RAM remain player startup checks.

## Updates and preserved data

Run the installer again with the same destination to update. Switching from
Standard to With Demos adds the games; installing Standard later does not erase
them. Existing profiles, `dosbox.conf`, saves and game configuration remain.

AmiPkg owns the shipped executable/support files. The demo package owns immutable
templates in `DemoDefaults`; missing playable files are seeded into `PC/Games`
without overwriting existing files. Demo profiles are merged into the library,
without changing matching profiles. A pre-merge library is retained as
`PC/Launcher.install-backup`. A malformed library is reported and preserved.
Removal intentionally leaves user data and seeded games behind; the launcher's
Delete action remains available for removing individual games.

The destination's `PackageManager` drawer holds a private AmiPkg client, cache and
receipts. Existing system AmiPkg installations and startup scripts are untouched.
`ZZPKG:` and `ZZDOSDEST:` are temporary assigns, not startup entries.
After installing, `PackageManager/amipkg doctor` checks receipt-owned files.

## Offline trust and client changes

Each installer carries a signed catalogue and its pinned LHA archives. `ZZInstall`
verifies the Ed25519 signature before staging the catalogue; AmiPkg checks archive
SHA-256 before extraction. Standard includes only the core package. With Demos
includes both core and the `zzdosbox-demos` package, which declares the core as a
dependency. No MUI/AHI package dependency is fabricated: installed system
components are checked through AmigaOS APIs instead of requiring AmiPkg receipts.

The private client is **amipkg 0.7.11-zzinstall1**, based on upstream commit
`66cfb9888f7d6b28d1411ff254817ee14ed4e043`. It includes four local fixes:

- Read custom package recipes from their selected repository's catalogue.
- Run prerequisite scripts **before removing an old package during upgrade**.
- Preserve assign-root paths when copying trees (`ZZDOSDEST:PC`, without a parent-directory slash).
- Store resolved volume paths in receipts, so checks and removal survive reboot
  and later installations using a different temporary assign.

This is not an upstream AmiPkg release. Upstream's copy/upgrade engine does not
provide complete transactional rollback. Keep a backup when upgrading. The wrapper
checks seeding again because AmiPkg normally reports post-script failures only as
warnings. Use **Install** for this offline repository; its `.invalid` URL is a
deliberate placeholder, not an online update service.

## Build

```sh
python3 scripts/build_dosbox_installer.py
# Generate a private signing key once; keep the .secret outside Git and releases.
python3 .context/amipkg-installer/client-src/tools/amipkg-repo-sign keygen /private/path/zzdosbox
python3 scripts/package_dosbox_amipkg.py \
  --zip-dir /path/to/standard-and-demo-zips \
  --client-source .context/amipkg-installer/client-src \
  --helper .context/amipkg-installer/ZZInstall \
  --signing-key /private/path/zzdosbox.secret \
  --lha /path/to/lha --output .context/installers
```

Source archive, client patch and Apache licence/notice are retained. Public-key
files and signatures can ship; secret signing keys never do. Demo provenance and publisher notices are described in
[release editions](demo-edition.md).

[Upstream AmiPkg](https://github.com/thomas-luebker/amipkg) ·
[Recipe documentation](https://github.com/thomas-luebker/amiga-pkg/blob/main/PACKAGING.md)
