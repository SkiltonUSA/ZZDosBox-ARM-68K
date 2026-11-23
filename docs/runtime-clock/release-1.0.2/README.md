# ZZDosBox 1.0.2 release checks

The Compatibility, Expanded, memory-debug, network JIT, software-OpenGL and
Windows interpreter/JIT players are rebuilt at 1.0.2 with runtime timer
calibration. Their exact outputs, source hashes, options and compiler are in
the seven build records. [Packaged executable identities](payload-identities.json).

All seven installed players pass on the A4000TX at both 666.7 and 800 MHz:
Doom runs complete 10,000 guest milliseconds; Windows runs complete 18,000.
Each selects the expected 333.333/400 MHz timer, passes SIMD/FPSCR context,
register/MMU/cache restoration, file closure and owned-memory release.
The OpenGL renderer and network backend close cleanly. Windows uses a private
copy of the installed image; the original image is untouched. Windows tests
stop through the bounded harness, not its menu. [Measurements](hardware.json).
A separate F10 exit check stops the Compatibility player before the harness
limit and passes cleanup. The final standard installer also updates the normal
application directory; executable hashes match and Launcher.cfg is unchanged.
The authorised stock-clock test ends with 800 MHz KEEP restored.

The clock-restoration harness initially reused an ended session nonce and was
rejected by the firmware replay guard (error 6). A fresh session restored the
clock, then the normal launcher session was restored. No firmware or thermal
checks were changed. The launcher's existing fixed session ID has the same reapply limitation:
after ending an 800 MHz session, immediately reapplying it can be refused until
reset. This release does not change that clock client or firmware. Normal
emulation remains available. The issue did not invalidate the seven completed
stock-clock tests.

The signed standard AmiPkg installer was transferred, extracted and executed
on the Amiga into a separate destination. Dependency checks, catalogue/payload
verification, installed executable hashes and launcher open/exit pass.
[Installer log](dos-install.log). The demo edition is checked locally for
signatures, checksums, full LHA round-trip and matching runtime payloads;
its demo content is unchanged from 1.0.1. No fresh physical demo-edition install
is claimed. Neither package contains settings or save files.

Native sanitizer checks cover both timer frequencies and adaptive cadence,
all 64 required/optional installer dependency combinations, and clock-service
absence, busy/fault/temperature/foreign-session refusal. These policy tests do
not cover a complete clock-session reapply sequence (the limitation above).
Missing-service cases
are native policy checks, not a firmware replacement on the physical machine.

Legacy silent, Adaptive/Scanlines and network-interpreter executables retain
their recorded earlier builds and fixed timer assumptions. Use 666.7 MHz for
their timing measurements. Duke/Descent diagnostic observers remain outside
this release change. Windows paged execution still falls back safely to the
interpreter. Audio gaps remain. This release corrects timing accounting and
presentation budgets; it does not claim a game-FPS or audible-quality gain.
