# ZZDosBox 0.23.0 Beta installers

[Download both editions](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/releases/tag/v0.23.0-beta).

Standard provides the emulator with an empty library. With Demos also includes
Corridor 7, Doom 1.9 shareware and the non-interactive King's Quest VII Demo.
Both contain the tested 0.23.0-dev launcher, with full-page settings, compact
navigation, [i] About and scrolling status history.

The emulator runtimes, installer client/helper and demo media/profiles are unchanged
from 0.22.7 Beta. See [installation and dependencies](../../../docs/amipkg-installers.md).

## Verification

- Both outer LHA archives reproduce byte-for-byte across two builds.
- Signed catalogues verify; extracted inner archives match their recorded size/SHA-256.
- Extracted launchers match the hardware-tested launcher checksum.
- Payload comparison confirms only launcher, documentation and checksums changed.
- Native installer dependency tests pass.
- The launcher itself passed physical 640×480 and 1024×768 layout checks.
- A fresh on-machine installer upgrade test is pending; an active Core1 session
  was left undisturbed. Earlier installer upgrade results remain in the 0.22.7 records.

Public catalogues, keys and signatures are retained here; no secret keys are included.
