# Ordered observers and reverse readback

The 68k still sometimes reads previous-scene pixels after observing the marker.
Waiting for the ARM observer to finish passed this series, but also adds time:
it does not establish a production completion fence or a cache-coherency fix.
No guest Glide integration, firmware change or release promotion occurred.
The requested 800 MHz setting was left unchanged.

## Ordered observer comparison

Twelve rotated rounds used the same five offscreen scenes and fresh markers.
A failed scene ends that run, so scene counts differ. All three modes disable
pre-submit marker preparation. The two witness modes busy-poll their mailbox;
they do not deliberately yield for one Amiga tick.

| Mode | Complete runs | Full passes | Scenes checked |
| --- | ---: | ---: | ---: |
| Standalone cold readback | 12 | 5 | 42 |
| Concurrent ARM/68k, fast mailbox polling | 12 | 5 | 39 |
| ARM verification finishes before 68k GPU readback | 12 | 12 | 60 |

`GLIDEWITNESS fast` releases the worker at submission and allows concurrent
readback. `GLIDEWITNESS beforehost` waits for the worker's `GW_DONE` publication
before the 68k reads any marker or pixel. There is no log I/O in that interval.
Both use a one-second host mailbox deadline. Existing concurrent, afterhost
and idle controls remain available.

Measured ARM preparation was 133–15,551 microseconds for fast mode and
148–10,440 microseconds for beforehost. The latter then waited another
705–29,207 microseconds after submission. This is not an instantaneous comparison
of two observers: scheduling, mailbox transactions and verification time remain
confounders. All recorded ARM scene checks passed; failed 68k checks passed a
later diagnostic read but retained failure status. All 24 Core1 runs returned
RET1, restored the recorded SCTLR and released their owned allocation.

Build hashes are in [ordered-build.json](ordered-build.json); raw logs are
`cold-*`, `fast-*` and `beforehost-*` in this directory.

## Reverse ARM traversal

The second build adds `GLIDEREVERSE 1` to inspect framebuffer offsets from 4095
down to zero. The 68k keeps its forward traversal. The worker records elapsed
verification ticks and the first incorrect original sample, if any. It does not
re-read a failed sample or write GPU memory. `GLIDEREVERSE 0` is the default.

| ARM traversal | Completed runs | Full passes | Scenes checked |
| --- | ---: | ---: | ---: |
| Forward | 5 | 3 | 23 |
| Reverse | 4 | 1 | 14 |

A fifth reverse run did not complete and is excluded from the completed-run
counts, not recorded as a pass. These are preliminary, interrupted samples.
All ARM checks in completed runs passed, including reverse reads, while some
68k checks failed. Reverse verification took 739,055–864,340 raw global-timer
ticks. A full verification pass therefore cannot prove that both processors saw
the same pixel at the same instant. No calibrated FPS or throughput is claimed.

For example, [direction1-03.txt](direction1-03.txt) records scene 1 ARM bad=0,
68k bad=35, and late 68k bad=0. Its ARM verification took 784,322 ticks; its
68k marker wait was 87 microseconds. Guards remained intact in completed runs,
and all nine completed Core1 lifecycles restored and released their allocation.
Build hashes are in [reverse-build.json](reverse-build.json).

## Interrupted run and hardware stop

[direction1-05.txt](direction1-05.txt) contains only the bridge's 15-second
still-running response, with no application output. The backend accepted script
20089 at 2026-11-19 00:40:20 UTC, then disconnected at 00:40:44 with
`bridge_silent`. Subsequent network pings and bridge commands did not respond.

The captured log cannot locate the interruption in startup, mapping, rendering
or teardown. Cleanup is unverified. The gateway was paused and its reservation
retained to prevent another hardware launch. No cause is assigned to the new
traversal code, the GPU, the bridge or the clock setting from this evidence.
The overall series contains **45 completed runs, 178 scene checks and one
interrupted run**. No further GPU probe should run until target recovery and
exclusive runtime availability have been established.

## Local checks

- Native ASan/UBSan oracle checks cover all five scenes in both traversal orders,
  corruption detection and bounded mismatch capture at both framebuffer heights.
- Nine compiled-worker instruction-model cases cover normal, corrupt, idle,
  nonce/range/order rejection, cancellation and reverse traversal. They check
  that no GPU pixels are read before release and that the worker never writes
  GPU memory. This model does not reproduce GPU/DDR timing.
- The separate 68k probe and Core1 witness build successfully. The firmware
  submit-validator check still passes; depth requests remain rejected by the ABI.

Next investigation: record a small set of matching pixel addresses immediately
at marker observation, rather than infer visibility from a whole-frame scan.
That requires a recovered target and another bounded diagnostic, not a
production delay or automatic retry after an unknown completion.
