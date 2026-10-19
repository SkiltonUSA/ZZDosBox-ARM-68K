ZZDosBox **0.22.6 Beta** provides two complete, offline AmiPkg installers.

## Choose an installer

| Installer | Size | Included programs | AHI requirement |
| --- | --- | --- | --- |
| **Standard** | **7.2 MB** | Emulator, launcher, utilities and compatibility database; empty game library | Optional — sound-off use allowed |
| **With Demos** | **23.7 MB** | Everything in Standard, plus **Corridor 7**, **Doom 1.9 shareware** and **King's Quest VII Demo**, with configured profiles | AHI v4+, installed output drivers and accessible unit 0 |

Both contain identical emulator binaries. With Demos includes the complete application; you do not need to install Standard first. King's Quest VII is a **non-interactive promotional demo**.

## Install or update

1. Extract the chosen `.lha` archive on the Amiga. `C:lha` and, for Workbench launch, `C:IconX` are required.
2. Close ZZDosBox and any active Core1 application, then double-click **Install**.
3. Choose a destination. Open `PC/ZZDosBox-Launcher` after installation.

Both check **MUI 3.8+, AmigaOS libraries, Picasso96, ZZ9000 graphics/Fast RAM and AHI**. Missing requirements are reported; system libraries and drivers are not installed automatically. CAMD is optional for MIDI. Microsoft DOS/Windows and ZZ9000 firmware are not included.

Run Install again against the same destination to update. Existing profiles, settings and saves are preserved. Adding With Demos merges its profiles and copies missing game files; installing Standard afterward does not remove demos.

## Verification

Signed catalogues, SHA-256 archive checks and checksum files are included. Standard installation, the demo upgrade and preservation of user data were tested on the A4000TX. The final receipt-path fix was tested separately on the Amiga. This is a Beta; full validation details are linked below.

The launcher identifies as `0.22.6-dev`; the Beta name identifies this distribution. Runtime version strings vary by component. Demo data retains its publishers' copyrights and original notices.

[Installer guide](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/v0.22.6-beta/docs/amipkg-installers.md) · [Demo profiles](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/v0.22.6-beta/docs/demo-edition.md) · [Validation](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/v0.22.6-beta/artifacts/development/amipkg-0.22.6/README.md) · [Changelog](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/v0.22.6-beta/CHANGELOG.md)
