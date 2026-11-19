# Early ARM and 68k pixel samples

Following the reboot, the bridge reconnected and confirmed that the old probe
and its RAM log were gone. No Core1 application held the runtime port. The clock
service reported its held 800 MHz session, Core1 idle and 53.5°C before testing.
No clock, firmware, installed emulator or release was changed.

The new optional `GLIDESAMPLE 1` diagnostic captures four original pixel reads
immediately after each observer finds the marker. It stores them privately,
before whole-frame verification or log output. The offsets are 1616, 2704, 3344
and 3368: interior pixels, including positions implicated by earlier failures.
ARM publishes its captured values with the normal completed reply. The host
prints both sets after that reply; it does not re-read the framebuffer for them.
With sampling enabled, `wait_ticks` and host `fence_us` also include the small
sampling operation; they are not identical to the previous marker-only measures.
Sampling defaults off, and the original whole-frame verdict remains authoritative.

| Mode | Complete runs | Full passes | Scenes checked |
| --- | ---: | ---: | ---: |
| Concurrent ARM and 68k, fast mailbox polling | 4 | 2 | 16 |
| ARM verification finishes before host GPU readback | 4 | 4 | 20 |

All eight runs completed mapping, RET1 return, SCTLR restoration and allocation
release. All recorded ARM whole-frame checks passed. Both failing host runs
passed delayed readback but retained failure status. No guard corruption or
completion timeout occurred.

## Captured disagreements

| Run / scene | Pixel offsets | ARM original samples | 68k original samples | Whole-frame host failures |
| --- | --- | --- | --- | ---: |
| fast-01 / texture | 3344, 3368 | `f800`, `001f` (red, blue) | `ffe0`, `ffe0` (previous yellow scene) | 28 |
| fast-04 / red triangle | 3344, 3368 | `f800`, `f800` (red) | `0000`, `0000` (previous black scene) | 35 |

See [fast-01.txt](fast-01.txt) and [fast-04.txt](fast-04.txt). The other two
sampled offsets matched in these scenes. All four ordered runs matched all four
samples in every scene.

This removes whole-frame traversal from the *captured-value* comparison. It does
not make the two processors read simultaneously: their marker polling, request
acknowledgement and read paths still differ. ARM samples follow a barrier; the
68k early samples precede its full-frame CacheClearE. The ordinary host verifier
then performs that cache maintenance and still fails in the two recorded cases.
These results therefore do not justify identifying one cache, bridge or GPU
pipeline as the cause, or treating an ARM read as a production fence.

Next useful test is a publication immediately after the ARM's small sample set,
so the host can wait for that event without waiting for a whole-frame verifier.
A timing-matched control would still be needed before claiming a fix.

## Validation and final state

The native test first failed on the missing sampling function. The implementation
then passed ASan/UBSan checks of offsets and endian conversion at heights 65 and
72. Twelve compiled-worker model cases pass, including sampling order, captured
corruption, invalid sample mode, cancellation, no reads before GO and no GPU
writes. The real upstream submit-validator checks pass. Both standalone and
witness hardware binaries build with strict warnings; [build.json](build.json)
records source and executable hashes. The model does not reproduce hardware
visibility or timing.

After testing, the runtime port was absent, the clock service reported Core1 idle
and 56.4°C, and bridge ping responded. Temporary probe/log/captures were removed
from RAM and the reservation was released. The prior interrupted run remains an
unexplained failure in the earlier report; this successful series does not erase it.
