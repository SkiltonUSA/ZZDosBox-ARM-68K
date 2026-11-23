# Standard and memory-debug runtime-clock candidates

Historical candidate record. For packaged builds and subsequent checks, see
[1.0.2 release validation](../release-1.0.2/README.md).

Both configurations were rebuilt from current source using the matching
[toolchain](../../build-and-restore.md#matching-arm-compiler-and-c-headers).
The standard configuration follows `make build-current`, with opt-in runtime
timer calibration added; the memory build adds `--pc-memory-debug`. Both use
4 MiB JIT and a 32 MiB Exec allocation. They are separate from the Expanded
runtime and its 64 MiB allocation.

[Standard build](standard-build.json) · [Memory build](memory-build.json).
Installed players and package versions are unchanged. These tests do not cover
network, Windows or OpenGL variants, and do not establish a game-FPS gain.

The native memory-debug tests pass with ASan/UBSan, covering guest-RAM bounds,
paused compare-before-write edits, unaligned scans, refine/overflow and reads.
The physical helper in `tests/amiga/dosbox/memory_live.c` is deliberately narrower:
it checks status on an 8 MiB guest, pauses, reads the same guest BIOS-data range
twice, rejects an overflowing address and resumes. It does not write guest RAM.
Run it only against an isolated memory-debug player; it keeps its Exec message
alive until the reply, rather than freeing an in-flight request.

## Physical results

All four runs used 8 MiB guest RAM, 10,000 cycles and Doom `-playdemo demo3`
for 20,000 guest milliseconds at the standing 800 MHz ARM setting. Each
completed 640,000 stereo audio frames and passed the SIMD context witness,
register/MMU/cache restoration and owned-memory release checks.

| Player | Display | Presentation | Audio gap events | Result |
| --- | --- | --- | ---: | --- |
| standard | window | Normal | 623 | Pass |
| standard | window | Adaptive | 452 | Pass |
| standard | fullscreen | Adaptive | 459 | Pass |
| memory | window | Adaptive | 508 | Pass |

[Measurements and logs](hardware.json) retain each timer estimate and emulation
interval. Source-frame counts and the overlay are not Doom's internal game FPS;
these checks establish runtime integration, not a speed improvement. Audio gaps
remain, and this single run per configuration is not a comparative audio benchmark.

The [live debugger exchange](memory-ipc.log) passed status, pause, repeated reads,
bounds rejection and resume while the memory player was running. The helper's
[source and executable identity](memory-check-build.json) are recorded separately.
The [Windows, network and OpenGL follow-up](../remaining-variants/README.md)
records their separate builds and hardware checks.
Stock-clock physical testing and release integration remain pending.
