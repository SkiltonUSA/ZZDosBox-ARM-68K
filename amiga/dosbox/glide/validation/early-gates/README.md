# Early ARM acknowledgements

The completed runs pass with a short ARM acknowledgement, but a subsequent run
lost the bridge and network connection. This is an interrupted diagnostic series,
not a production fence or a stability result. No released emulator, firmware or
clock setting was changed.

## Experiment

An optional ARM-only early reply occupies its own 64-byte mailbox span inside
the existing Exec allocation. The worker writes the reply payload, performs a
barrier, then publishes the scene sequence. It continues normal verification.
The host waits with its existing one-second deadline and rejects an error reply.
No GPU memory is written by the worker.

- `GLIDEWITNESS samplegate`: publish after the four original pixel samples;
  requires `GLIDESAMPLE 1`.
- `GLIDEWITNESS markergate`: publish immediately after marker observation,
  before reading the four interior pixels.
- Existing `fast`: concurrent host/ARM readback, no early publication.
- Existing `beforehost`: wait for the complete ARM verifier.

`GLIDESAMPLE 1` was enabled in all runs. Marker-only publication does not disable
later ARM sampling or verification; it merely publishes before them. Therefore
this control does not remove concurrent ARM framebuffer traffic entirely.

## Recorded results

The first sample-gate run was inspected individually. Remaining modes were
rotated between rounds. Each run ends at its first failed scene.

| Mode | Completed runs | Full passes | Scenes | Post-submit host wait, microseconds |
| --- | ---: | ---: | ---: | --- |
| Concurrent | 4 | 0 | 8 | 0 |
| Sample acknowledgement | 4 | 4 | 20 | 72–15,056 |
| Marker acknowledgement | 3 | 3 | 15 | 72–16,287 |
| Whole-frame acknowledgement | 3 | 3 | 15 | 2,136–16,578 |

All four concurrent runs failed the red-triangle scene with 35 incorrect host
pixels. Their delayed host readbacks passed, but the runs retained failure
status. ARM whole-frame checks passed. Completed runs had zero guard errors and
verified mapping, RET1, SCTLR restoration and owned-allocation release.

The shorter waits can be tens of microseconds, but scheduling also produces
millisecond waits. This series does not compare fixed equal-duration waits,
and the marker-only worker continues reading pixels after publication. Thus:

- Waiting for the complete ARM verifier is unnecessary for the recorded passes.
- Sampling four interior pixels before publication is also unnecessary for the
  recorded marker-gate passes.
- These observations do not distinguish acknowledgement traffic, elapsed time,
  ongoing ARM traffic or host/GPU visibility. No FPS benefit is established.

## Interrupted run

[markergate-04.txt](markergate-04.txt) is the fifteenth attempted run. It contains
only the bridge's still-running response after 15 seconds, without application
output. The preceding sample-gate run completed, released memory, had no runtime
port and answered ping. Script 52360 was accepted at 2026-11-19 01:12:32 UTC;
the backend disconnected at 01:12:54 (`bridge_silent`). Network ping then failed.

There are **14 completed runs / 58 scenes, plus one interrupted run**. The
planned fourth whole-frame control was not launched. The gateway is paused and
its reservation retained; latest cleanup is unknown. No failure cause is inferred
from the unavailable log. Last pre-series clock status was held 800 MHz, Core1
idle, 58.2°C; no temperature was obtained at the interruption.

The harness redirected output to RAM and only typed it after the executable
returned. This prevented the bridge from retrieving intermediate progress.
For a future recovered-target run, invoke the probe directly within the bridge
script so its existing flushed mapping progress reaches the bridge's captured
output. This harness adjustment has not yet been tested on hardware. First
localise the repeated interruption before expanding GPU tests.

## Local verification

The worker-model test first failed because the new early publication did not
exist. After implementation, seventeen cases pass. Tests check publication
before whole-frame reads, marker-only versus sampled contents, timeout error
publication, invalid publication settings, cancellation, no pixel access before
GO and no GPU writes. Native sanitizer and unchanged firmware-validator checks
also pass. Strict 68k and ARM builds pass; [build.json](build.json) records the
exact diagnostic source and executable hashes. Instruction-model timing is not
physical hardware timing.

## Recovery check

After the next confirmed reboot, the bridge returned, no runtime port remained
and the old probe/log were absent from RAM. Clock status was held 800 MHz,
Core1 idle, 53.7°C. One marker-gate run using direct script output passed all
five scenes, returned RET1, restored SCTLR and released its allocation; see
[markergate-recovery01.txt](markergate-recovery01.txt). The temporary executable
was then removed and the bridge reservation released. This run is separate from
the interrupted matrix above and does not explain or resolve the repeated
connection loss. Direct output capture worked for this successful run; capture
of an interrupted run remains unverified. No long loop was resumed.
