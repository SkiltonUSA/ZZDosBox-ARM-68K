# ZZDosBox ARM profiling and colour conversion

A4000TX / ZZ9000 XX19c, 12 October 2026. Optional --profile reports exclusive
64-bit global-timer ticks. It does not change timer/PMU configuration and is
disabled for the FPS comparisons. Explicit scopes handle cancellation without
C++ destructors across longjmp; the analyser checks exact sums.

Initial profiles identified scanline RGB565 conversion as 27.1% of Keen's
ARM time and 22.9% of Vette's. The new converter uses two 256-entry constant
tables (2 KiB), retaining every colour exactly. Format selection is per row;
32-bit rows use the existing scalar memcpy. No NEON/VFP or frame skipping.

| Startup workload | Before | After | Gain |
| --- | ---: | ---: | ---: |
| Keen 6 | 35.97 FPS | 41.85 FPS | 16.3% |
| Vette | 25.26 FPS | 28.86 FPS | 14.3% |

Both cases use 558 frames / 8,000 emulated ms, 1 ms host polling, unchanged
CPU settings and resolution, and no screenshots. All frames were ACKed,
file writes stayed zero, registers/cache witness restored and owned memory
released. These single-run startup results do not establish gameplay speed.

Tests: native ASan/UBSan and isolated A9 conversion test cover all 65,536
RGB565 colours and 1,024 widths at 8/16/32 bpp; native checks four source
alignments, ARM sources end on an unmapped page boundary. Full linked ARM
checks retain exact EGA output, both relocation bases/formats, CPU/DOS/MZ
fixtures, profile sums, nonce rejection and cancellation. EGA/PIT passes
physically. Tests of instrumentation with a synthetic clock are correctness
evidence only. No commercial game data or executable payloads are included.

- `profile-build.json`: instrumented baseline; profiler disabled for `before`.
- `capture-build.json`: new converter and final candidate provenance.
- `before-build.json`: previously installed interpreter-optimised build.
- `*-before.log` / `*-after.log`: profiling-disabled physical FPS comparison.
- `*-profile.log`: instrumented physical runs; raw ticks include measurement overhead.
- `profiles.json`, `comparison.json`: validated summaries.
- `capture-native.log`, `capture-arm.log`, `capture-full-arm.log`: correctness checks.
- `profile-model.log`: exclusive accounting, cancellation and timer wrap model.

CPU includes guest memory handlers; events includes PIC/timer/VGA generation.
Other includes BIOS/DOS/file work and startup/cleanup. Grant time is ARM waiting
for a host buffer grant, not total PAN latency. No Zorro-bandwidth or tear-free
scanout claim follows from these counters.

Executable SHA256:
`a3e8c8cf41590d5b72b53bb2896ba100824b14c4954311c7bf8f20920fb3a034`.
ARM ELF SHA256:
`36a8d323e72bf922c4a1852aa777569aa0988b893d64e3d684b6e88edafc8558`.

Follow-up profiles show 49.8%/49.4% lower conversion ticks in Keen/Vette.
Input-wait cancellation and active-game Ctrl-C also pass, with valid profiles.
Installed as the normal prototype; the previous executable is retained as
ZZDosBox-PreVGA. No GitHub publication performed.

Post-install default-path EGA/PIT passed (18 frames / 18 ACKs). Final state:
Workbench, Roadie and bridge available; Core1 ownership released.
