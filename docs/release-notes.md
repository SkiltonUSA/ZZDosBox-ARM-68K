# ZZDosBox 1.0.2 Beta

Application/launcher version: **1.0.2 Beta**. AmiPkg version: **1.0.2-beta**.

Compatibility, Expanded, memory-debug, network JIT, software-OpenGL and both
Windows players now measure the ARM timer at launch. Presentation scheduling
and the FPS overlay use the selected 666.7/800 MHz frequency. This does not
change CPU cycles, audio rate, firmware or the board clock.

The launcher and these seven players are rebuilt at 1.0.2. Legacy silent,
Adaptive/Scanlines and network-interpreter players retain their previous build
identities and fixed-frequency accounting; use 666.7 MHz for measurements with
those players. Separate Duke/Descent diagnostic observers are not included in
this timing update. No FPS gain or audio-quality improvement is claimed;
audio gaps remain. Windows JIT retains interpreter fallback for guest paging.

Both standard and demo editions use the same emulator builds. The demo edition
also includes Corridor 7, Doom and King's Quest demos. Existing settings and
saves are preserved. Extract the installer and open Install to select a writable
destination; close emulators before updating. MUI and other dependency checks
run before installation. Missing optional clock services disable overclock
controls only; no firmware or clock change happens automatically.

The existing clock client can refuse an immediate 800 → 666.7 → 800 MHz
sequence because it reuses an ended session ID. Reset clears that session;
normal emulator launch remains available. This release retains that client.

[Release checks and build identities](runtime-clock/release-1.0.2/README.md) ·
[Changelog](../CHANGELOG.md) · [Setup](../README.md#dependencies).

## Archive documentation refresh

Installers and component LHAs include readme.txt copied from the main README,
about.txt with credits and licence.txt. Existing component notices are retained.
This documentation-only refresh leaves executable hashes and application
versions unchanged.
