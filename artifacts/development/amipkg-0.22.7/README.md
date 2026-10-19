# ZZDosBox 0.22.7 Beta — AmiPkg installers

Both editions include the scalable 0.22.7-dev launcher. Standard has an empty
library; With Demos includes Corridor 7, Doom shareware and King's Quest VII Demo.
`packages.json` records final archive sizes and SHA-256 checksums. Edition
subdirectories contain signed catalogues and the public verification key.

The installer client/helper are unchanged from [0.22.6](../amipkg-0.22.6/build.json).
The launcher build and physical layout reports are in [launcher-0.22.7](../launcher-0.22.7).
Game data and the signing secret are not stored in this repository.

## Package verification

- Two independent builds produce byte-identical outer LHA archives.
- Catalogue Ed25519 signatures and all LHA CRC checks pass.
- Nested package extraction yields the verified 709,288-byte launcher,
  SHA-256 `28c530a44c6f651152ac07c6e79b8aa264cb39a5beb6ada4e9fa28c9260aa7a0`.
- Compared with 0.22.6, only the launcher, release text, documentation and
  checksum manifest changed. Emulator runtimes, game profiles and media match.
- Native launcher layout checks pass; source hashes match the production build.

## Physical upgrade

The Standard and With Demos installers upgraded an isolated 0.22.6 installation
on the A4000TX to 0.22.7 Beta successfully. The installed launcher checksum matches the build;
the existing library, custom DOSBox configuration and test save file retain
identical before/after checksums. Both package receipts report 0.22.7-beta.
The library still contains one custom profile plus the three supplied demos,
without duplicates. The running production launcher was closed cooperatively
for the check and reopened afterward. Workbench screen preferences were not changed.

[Installation and dependencies](../../../docs/amipkg-installers.md).
