ZZDosBox **1.0 Beta** is available in two signed offline AmiPkg editions.

- **Standard (7.2 MB):** emulator with an empty library; add your own software.
- **With Demos (23.7 MB):** complete emulator plus Corridor 7, Doom 1.9 shareware and the non-interactive King's Quest VII demo.

This release adds USB/SD shared folders as DOS C:, Workbench file transfers, and a mounted CD chooser for IDE/ATAPI, SCSI and USB data CDs as read-only DOS D:. It also fixes stale standard input in disk mount/eject helpers.

Extract the chosen `.lha`, open its drawer and run **Install**. Select the destination; existing profiles, configuration and saves are preserved. Both installers check MUI, AmigaOS libraries, Picasso96 and ZZ9000 requirements. Standard permits sound-off use; With Demos requires configured AHI v4+ audio. CAMD is optional. System libraries and drivers are not bundled.

The launcher identifies itself as **1.0 Beta**. Tested emulator runtimes and demo media remain unchanged from 0.22.7 Beta, retaining their own version identifiers.

Signed catalogues, checksums and extracted payloads pass validation; both archives reproduce byte-for-byte. The Standard installer passed a fresh A4000TX installation and AmiPkg doctor. Shared-folder read/write passed on the Amiga. Physical CD and removable USB-media checks remain pending.

[Installation guide](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/main/docs/amipkg-installers.md) · [Shared drives and CDs](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/main/docs/amiga-dosbox-shared-drives.md) · [Changelog](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/blob/main/CHANGELOG.md)
