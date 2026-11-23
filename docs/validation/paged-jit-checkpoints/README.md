# Paged-register JIT and CPU checkpoint validation

## Decision

Keep the **1.0.2 Beta paging fallback**. Neither candidate establishes a Windows
speed gain. The new code is an opt-in experiment, not a promoted release.
Save-state work has reached a tested real-mode integer CPU component only.

## Physical comparison

A4000TX/ZZ9000 at the existing 800 MHz setting; Expanded player, 4 MiB translation
cache inside a 64 MiB owned allocation, 8 MiB guest PC RAM, 6,000 configured CPU
cycles, VGA window, sound off and profiling on. Each fresh process boots the same
private Windows disk image for 6,000 guest milliseconds. The original disk and
installed players were not modified. No clock changes were made.

| Configuration | Wall time, run 1 | Wall time, run 2 | Mean | Compared with its normal-paging pair |
|---|---:|---:|---:|---:|
| Normal paging, first pair | 37,622 ms | 37,514 ms | 37,568 ms | Baseline |
| Register groups, 32-cycle fallback | 39,821 ms | 40,072 ms | 39,946.5 ms | 6.33% longer |
| Existing single-instruction paged JIT | 39,628 ms | — | 39,628 ms | One reference run |
| Normal paging, second pair | 37,446 ms | 37,823 ms | 37,634.5 ms | Baseline |
| Register groups, 256-cycle fallback | 38,140 ms | 38,127 ms | 38,133.5 ms | 1.33% longer |

The larger interpreter batch reduced paged fallback calls from 627,640 to
107,201 in these candidates. That reduces switching overhead, but the small
translated subset still does not outperform normal paging. The 1.33% difference
is small and should not be treated as a robust universal regression estimate;
it certainly does not establish an acceleration. Further work needs useful,
fault-safe instruction coverage rather than another batch-size claim.

The recorded 413–414 display submissions are **not application/game FPS**.
These are boot-path timings, not WinBench scores, audio tests or interactive
Windows performance measurements. Profiling overhead is present in every run.

All nine runs passed clean exit, ARM register/context and cache-witness
restoration, and owned-memory release. Final Core1 was idle and the unrelated
Core0 service inactive. The temporary disk copy was removed after testing.

[Build identities, per-run values and source hashes](results.json).
The individual `.txt` logs retain timing/profile/cleanup lines; full private logs
remain in the development workspace. Product version strings match the 1.0.2
base, while output hashes distinguish the isolated candidates.

## Correctness checks

- Native register-run bounds test, including an inaccessible next host page,
  passes AddressSanitizer/UndefinedBehaviorSanitizer.
- Both grouped candidates pass six paged fixtures at two ARM relocations:
  grouped-code invalidation, ordinary aliases, instruction-fetch fault,
  read/modify/write fault, nested faults and 386/486 user permissions.
- CPU checkpoint tests pass at 200 interpreter boundaries and after saving the
  diagnostic CPU/RAM data and restarting the native process. Corruption,
  truncation, trailing bytes, wrong architecture and unsupported execution modes
  are rejected. The component also compiles as Cortex-A9 ARM32 code.
- Python syntax and whitespace checks pass. UB crapper/dryer were run over the
  changed preparation/test scripts; no duplicate candidates were reported. The
  available mutator does not support C/C++, so no mutation-coverage claim is made.

[Grouped-block ARM model checks](paging-groups-model.txt) ·
[256-cycle fallback ARM model checks](paging-burst256-model.txt) ·
[CPU checkpoint native checks](cpu-checkpoint.txt).

## Save-state limits

CPU checkpoint results do not validate complete games, protected-mode state,
x87, device/event queues, sound, DOS file handles or disk-image consistency.
No save-state control is exposed in the launcher. See the
[implementation and remaining integration work](../../paged-jit-and-save-states.md).
