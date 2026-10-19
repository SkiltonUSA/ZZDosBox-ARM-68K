# ZZDosBox 0.22.6 Beta — AmiPkg installers

Matching offline Standard and With Demos packages. `packages.json` records
archive sizes and SHA-256; the edition subdirectories contain signed catalogues
and the public verification key. `amipkg` and `ZZInstall` are the built 68k tools.
Game data and the signing secret are not committed here.

Build instructions and dependency policy: [installer guide](../../../docs/amipkg-installers.md).
`build.json` records the compiler image, upstream revision and source/patch hashes.

## Verification

- Clean-source cross-build: client and helper pass; helper uses warnings as errors.
- AmiPkg portable core and CLI build pass, including assign-root copy regression.
- Ed25519 verification accepts the valid catalogue and rejects tampering, missing
  keys and wrong keys; every LHA archive passes CRC checks.
- All 64 dependency-presence combinations checked for both installation modes.
- Two independent packaging runs produce byte-identical installer archives.
- Standard installation on the A4000TX passes and creates an empty library.
- Standard-to-Demos upgrade passes: 36 core and 88 demo-template files recorded;
  one custom profile plus the three supplied profiles, with the original library
  backed up byte-for-byte. Custom DOSBox settings and a save file remain unchanged.
- Repeating demo seeding leaves exactly the same four profiles and user files.
- Final client receipt-path regression on the Amiga passes for copied files and
  a drawer icon: `doctor` reports healthy files after removing the destination
  assign. A deliberately failed pre-script returns 10 before upgrade removal,
  retaining the existing receipt and files.

Physical dependency results: MUI 19.35; Workbench 47.33; ASL 47.9; keymap 47.1;
Picasso96API 2.455; ZZ9000 graphics/Fast RAM boards; AHI 4.180 with 28 modes and
unit 0 accessible; optional CAMD 37.14. No sound is played by detection.

Hardware testing uses an isolated destination. No firmware, system libraries,
startup files or existing application installation are replaced.
Full payload installation tests use the Shell path. The ASL chooser opens on
Workbench and its Cancel button closes it cleanly. Full payload tests preceded the final
receipt-path patch; that patch was separately exercised with the small physical
copy/drawer-icon/failed-upgrade regression above. No reboot or missing-driver
simulation was performed on the live system.
