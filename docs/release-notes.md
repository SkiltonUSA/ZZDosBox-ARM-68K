# ZZDosBox 1.0.1 Beta

Application/launcher version: **1.0.1 Beta**. AmiPkg version: **1.0.1-beta**.

## Changes

- Rebuild the launcher with the next numbered release; remove the dated package suffix.
- Include Settings → ZZOverclock, with 666.7 MHz and experimental 800 MHz controls.
- Retain the recorded emulator players and their build identifiers. This release does not rebuild or relabel those cores.
- Update the main README and linked feature documentation.

Standard and demo editions include the updated launcher and clock client. Existing DOS runtimes are retained. Their older ARM-tick FPS/presentation accounting is not validated at 800 MHz; use 666.7 MHz for comparable measurements. No Glide experiment is included.

## Installation and compatibility

Extract the LHA, open its drawer and double-click Install. Choose a writable
destination and close emulators before updating. Dependency checks and existing
settings/saves are preserved. Clock control requires the optional ABI 3 firmware
service; its absence disables clock controls only. The clock affects the whole
board. No firmware is installed and no clock change happens automatically.
Use suitable cooling for 800 MHz; temperature preflight is not a continuous
thermal watchdog. No new performance gain is claimed here.

## Verification

All launchers were cross-compiled into separate release directories. Recorded
player hashes are retained. Installer signatures, payload hashes, extraction,
component references and application version strings are checked before publication.
Prior hardware evidence remains separate from these package checks.
