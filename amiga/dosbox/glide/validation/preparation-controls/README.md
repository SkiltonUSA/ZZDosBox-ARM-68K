# Preparation controls — built, hardware results pending

These diagnostics follow the [Core1 witness comparison](../core1-witness/README.md).
The idle-worker passes left shared-memory reads and pre-submit waiting as
confounders. This probe separates those actions without launching Core1.

**Status:** native checks, the upstream packet validator, both A9 compiler modes,
the six worker instruction-model cases, and the 68k diagnostic build pass.
No new physical test has run: the initial read-only bridge ping returned an
uncertain response. Network ping and passive bridge heartbeats remain available;
that is not proof that command handling or the target is ready for GPU work.

## Planned comparison

Use the same executable, height 65, fresh marker values and disabled marker
pre-read for each case. Rotate case order across rounds. Compare twelve runs
per case; stop a run on its first failed scene and stop the series on unknown
completion, persistent corruption or guard damage. Keep trace recording off
for the first matrix so its extra bookkeeping does not confound that comparison.

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

Follow-up once command handling is restored: run the matrix, repeat failing
cases with tracing, and compare the addresses/values against the previous
scene. A later good read must still leave the original test failed.
