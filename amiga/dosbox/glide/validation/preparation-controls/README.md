# Preparation controls — physical results, 18 November 2026

These diagnostics follow the [Core1 witness comparison](../core1-witness/README.md).
The idle-worker passes left shared-memory reads and pre-submit waiting as
confounders. This probe separates those actions without launching Core1.

**Status:** after the requested Mac MCP restart, command handling recovered and
all 80 diagnostic runs completed without a timeout or guard error. The 72-run
matrix checked 300 scenes; eight additional trace runs checked 28 scenes.
A failed first read still returns error 20, even when the later read passes.
No installed player, firmware, display mode or clock setting changed.

## Results

| Case | Complete passes / runs | Scenes checked | Measured added preparation |
|---|---:|---:|---|
| Cold baseline | 0 / 12 | 38 | None |
| Unrelated texture read | 2 / 12 | 43 | 26–57 µs |
| Staged-packet read | 2 / 12 | 39 | 26–58 µs |
| OS yield | 12 / 12 | 60 | 20,480–39,913 µs |
| Busy wait | 12 / 12 | 60 | 20,012–26,424 µs |
| Earlier marker read | 12 / 12 | 60 | Existing pre-read; not separately timed |

All failed matrix scenes passed the delayed read. Seven of eight extra cold
trace runs failed; their delayed reads also passed. The captured bad values
match previous-scene colours: black instead of red, red instead of yellow,
and yellow instead of the textured red/blue result. See [trace-01](trace-01.txt),
[trace-03](trace-03.txt) and [trace-04](trace-04.txt).

This rules out an arbitrary unrelated memory read as sufficient in these tests.
Both scheduling-yield and busy-wait controls pass, so yielding to another task
is not required for the recorded wait-control passes. The waits can explain why
the earlier idle-worker preparation passed without concurrent ARM pixel reads.
They do not establish what causes the separate marker-read dependency.

Previous-scene colours are consistent with delayed visibility or incomplete
writes; they do not distinguish 68k caching, Zorro read ordering and GPU write
retirement. These waits are deliberately **not** promoted into production. The
next useful investigation is the failing-address read order and marker-specific
visibility, with independent GPU retirement evidence where available.

## Comparison procedure

The matrix used one executable, height 65, fresh marker values and disabled
marker pre-read except for the marker control. Case order rotated across twelve
rounds. Each run stopped on its first failed scene. Unknown completion, persistent
corruption or guard damage would stop the series. Tracing was off for the first
matrix; its extra bookkeeping therefore did not confound that comparison.

| Case | Added flag | Action after packet/mailbox writes, before submission |
|---|---|---|
| Cold baseline | none | None |
| Unrelated read | `--submit-read-texture` | Read one volatile texture word |
| Packet read | `--submit-read-stage` | Read one volatile staged packet byte |
| Yield | `--submit-yield` | `Delay(1)`; measured time depends on OS tick phase |
| Busy wait | `--submit-spin` | Poll timer.device for at least 20 ms |
| Marker-read control | `--marker-preread-only` | Existing earlier marker read, without its pre-clear |

`PREP` records mode 0–4, measured preparation microseconds and the sampled
read value. The marker-read control uses mode 0 and the existing `prepare=2`
field. These waits are **diagnostic only**, not proposed production fixes.
The default preparation behavior is unchanged unless a flag is supplied.

Build and example commands:

```sh
python3 scripts/build_dosbox_glide_probe.py --height 65
python3 scripts/test_dosbox_glide.py
```

On a supported idle Amiga, under the shared gateway reservation:

```text
Stack 65536
RAM:ZZGlide-Probe --no-marker-preread --marker-base 1000
RAM:ZZGlide-Probe --no-marker-preread --marker-base 1100 --submit-read-texture
RAM:ZZGlide-Probe --no-marker-preread --marker-base 1200 --submit-read-stage
RAM:ZZGlide-Probe --no-marker-preread --marker-base 1300 --submit-yield
RAM:ZZGlide-Probe --no-marker-preread --marker-base 1400 --submit-spin
RAM:ZZGlide-Probe --marker-preread-only --marker-base 1500
```

## First-read mismatch trace

`--trace-readback` retains up to sixteen failing pixel samples from the reads
that produced the first verdict. It does **not** reread GPU memory to reconstruct
the failure. Logging occurs after verification, showing pixel offset, x, row,
observed RGB565 value and expected value. For the alpha scene, expected `800f`
is the nominal value; the original quantisation tolerance still applies.

The bad-pixel total is not capped when the trace fills. Native sanitizer checks
verify the samples' order and values, trace-disabled behavior, and the bounded
trace with all 4,096 tested pixels wrong at both supported probe heights.
The existing Core1 worker continues to call the trace-free wrapper.

Raw logs, the probe build manifest and executable checksum are included here.
The underlying encoder/worker and trace tests passed locally before these runs:
ASan/UBSan, 306 upstream-validator cases plus ten fixtures, ARM soft/softfp
compilation, six compiled-worker model cases and strict 68k probe/witness builds.
No Core1 worker was launched in this standalone preparation-control series.

After the final run, ping and public ports confirmed the bridge responsive and
Core1 idle. Temporary probes, logs and failed-frame captures were removed from
RAM; the gateway reservation was released, with forwarding enabled. The existing
800 MHz setting was left unchanged. These are completion diagnostics, not a
throughput or game-FPS benchmark.
