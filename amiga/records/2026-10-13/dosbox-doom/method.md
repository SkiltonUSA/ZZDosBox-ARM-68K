# Doom 1.9 shareware Demo3 — A4000TX / ZZ9000

Completed on physical A4000TX / ZZ9000: **approximately 3.10 game FPS**.
All 2,134 ticks completed in 689.143 measured seconds. Total player lifecycle
was 708.093 seconds, including loading and exit. No cancellation or guest error.

| System / test | Doom Demo3 FPS |
| --- | ---: |
| ZZDosBox on A4000TX / ZZ9000 (real A9 time, this run) | 3.10 |
| i386 SX25 / WDC ISA (Thandor) | 3.40 |
| i386 DX25 / OPTi (Thandor) | 4.58 |
| i486 DX33 / UM8881F PCI (Thandor) | 18.52 |

The current interpreter result is about 8.9% below the site's
386SX-25 entry. This is a comparison for this particular Doom workload and
configuration, not a general CPU-equivalence claim or official submission.
The site notes that graphics hardware affects its PC results.
Source: [Thandor Doom benchmark](https://thandor.net/benchmark/32).

Doom printed `timed 2134 gametics in 6425 realtics`: **11.62 guest-clock FPS**.
That is not the physical score. The native validation's guest-clock result
also differs, so native timings must not substitute for physical measurements.
The physical observer validated its executable signature and tick endpoints.

Teardown passed: guest exit 0, SHELL PASS, 13,064 display flips/acknowledgements,
no register mismatches or cache-witness failures, and owned memory released.
Workbench, Roadie and the bridge remained available.

## Installation

Doom 1.9 Shareware is installed and added to the launcher. All eight existing
profiles were preserved and the updated config was independently read back.
The normal game profile uses the launcher's highest preset, 6,000 cycles/ms;
the measured benchmark used a separate 10,000-cycle session. These settings
must not be confused when repeating the comparison. Private config backup:
`PC/Launcher-PreDoom`. Canonical ZZDosBox executable is unchanged.


## Method

[Thandor’s method](https://thandor.net/benchmark/32) uses Doom 1.9 shareware,
unchanged game defaults, `doom -timedemo demo3`, and 2,134 gametics.
The supplied two-part installer was joined and its ZIP CRCs checked. All 20
extracted files were transferred and independently MD5-verified on the Amiga.
No DEFAULT.CFG, detail, view-size or sound overrides were added.

The run uses 8 MiB guest memory, the normal x86 interpreter at 10,000 cycles/ms,
VGA, embedded DOS, and 640×480 fullscreen output with the FPS overlay disabled.
The application-owned XACP Core1 worker runs on the Cortex-A9. The 68k handles
OS services and display handshakes. The current port lacks PCM sound emulation.
This measures the complete emulator/display workload, not bare ARM CPU speed.

## Clocks and limitations

Doom's realtics use DOSBox's emulated timer. Applying `35 × 2134 / realtics`
therefore gives guest-clock throughput, which cannot rank the Amiga against
physical PCs when the emulator runs slower than real time.

A benchmark-only, read-only observer validates the loaded Doom executable's
code and result string before sampling its gametic and timedemo-start fields.
It timestamps the first game tick (0 or 1) and tick 2134 using the A9 global
timer (nominal 333,333,333 ticks/s). Only this elapsed time is used for the
physical comparison. It excludes executable/WAD startup and process teardown.
The observer samples once per emulated millisecond, with an endpoint just
before the last frame is rendered: approximately one frame of endpoint
uncertainty. Report the result as approximate throughput, not an exact official
Thandor submission. No screenshots or inputs were sent during the measured run.

The special executable is separate from the installed normal player. Guest
code and data are unmodified. Observer source and patch are recorded here;
private build artifacts are in `.context/amiga/dosbox-doom/`.
The generic scaler/read-ahead optimisation candidate is the build base, but
this run uses the unchanged exact 2× scaler and directory files, not a BIOS HDD.

## Validation

Two Mac native runs with the observer, including the exact uppercase physical
command, completed 2,134 gametics in 8,371 guest realtics. Signature matched;
no observer rejection; start tick 0 / end tick 2134. Mac runtime is not an A9
measurement. Linked ARM model checks passed graphics, HDD, mouse and copy-time
cancellation. A physical preliminary run was cancelled cleanly before completion;
it is not a benchmark score.

Game archives, extracted content, and diagnostic guest RAM remain private and
are not included in these records.
