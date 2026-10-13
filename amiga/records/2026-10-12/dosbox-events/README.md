# ZZDosBox PIC and VGA optimisation

A4000TX / ZZ9000 XX19c, 12 October 2026. Compile upstream PIC scheduling,
VGA drawing and VGA memory handlers with `-O2`. Interpreter stays `-O2`;
other ARM units stay `-Os`. No guest timing or rendering algorithm changes.

| Startup workload | Before | After | Gain |
| --- | ---: | ---: | ---: |
| Keen 6 | 41.98 FPS | 42.96 FPS | 2.3% |
| Vette | 28.88 FPS | 30.67 FPS | 6.2% |

Single profiling-disabled physical run per case: 558 frames, 8,000 emulated
ms, 1 ms polling, no screenshots. Slice counts, file reads and console hashes
match; all frames ACKed, zero writes, checked registers/cache witness restored
and owned memory released. These are startup tests, not gameplay FPS.

Separate profiles show 14.5%/18.9% fewer PIC/VGA event ticks for Keen/Vette.
Baseline instrumented logs are in the adjacent `dosbox-profile` record;
`profiles.json` includes both summaries. Bucket call counts match and
exclusive times sum exactly. Per-module contributions were not isolated.

Full linked ARM model passes both relocations/output formats, exact EGA
pixels, CPU/DOS/MZ fixtures, profile accounting, nonce rejection and
cancellation. Physical EGA/PIT and input-wait cancellation pass. The model's
synthetic timer is correctness evidence only.

- `*-before.log` / `*-after.log`: unprofiled physical measurements.
- `*-after-profile.log`: instrumented physical runs.
- `comparison.json`: equal-work checks and FPS.
- `profiles.json`: validated exclusive timing comparison.
- `*-build.json`: source hashes, compiler flags and binary provenance.
- `arm-model.log`: full linked ARM correctness results.
- `default-equivalence.log`: default build matches the tested candidate.

Executable: 728,548 bytes; ARM image: 549,576 bytes (+7,980), allocation 32 MiB.
Executable SHA256:
`12667663cfa1426196620bfa5a5396ba9f64e615098f971d373a1d72bc01a519`.
ARM ELF SHA256:
`885a85f983e344a482f14595f8c81ba7732b01923c46e796f7635c01e15a0975`.

The previous setting can be built with `--pc --pc-hardware-opt Os`.
No game data or executable payloads are included in this record.

Active Keen Ctrl-C also passes at 262 frames / 3,757 emulated ms with
`timed_done=0`, valid profile sums and clean restoration/release. Installed
as the normal prototype; preceding build retained as `ZZDosBox-PreEvents`.
Version remains 0.5.0-dev. Live PC controls/audio remain pending.
No firmware, startup or GitHub publication changes.

Post-install default-path EGA/PIT passed (18 frames / 18 ACKs). Final state:
Workbench, Roadie and bridge available; Core1 ownership released.
