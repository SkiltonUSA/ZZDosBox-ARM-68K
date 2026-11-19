# Core1 readback witness — 18 November 2026

A read-only Cortex-A9 Core1 worker now checks the same offscreen GPU scenes as
our 68k probe. One concurrent run produced **correct ARM readback and 35 wrong
68k pixels in the same additive scene**. A later 68k read passed. This is useful
isolation evidence, **not proof of a cache defect or a reliable completion fence**:
the two observers do not read every pixel at exactly the same instant.

The released player is unchanged. No guest Glide calls, game performance,
firmware modification or display takeover are involved. The existing 800 MHz
setting was left unchanged.

## Results

All runs used a 64 × 65 RGB565 target, five scenes in order (clear, solid,
additive, texture, alpha), CRC-checked packets and fresh per-run marker values.
The 68k pre-submit marker read was disabled. A failed scene stops that run.

| Configuration | Runs | Complete passes | Scenes checked |
|---|---:|---:|---:|
| Standalone 68k, no Core1 worker | 12 | 4 | 42 |
| Initial concurrent Core1 witness | 13 | 13 | 65 |
| Core1 armed, but no GPU-memory reads (`idle`) | 8 | 8 | 40 |
| Core1 reads only after the 68k pixel check (`afterhost`) | 8 | 8 | 40 |
| Concurrent witness with control-mode build | 8 | 7 | 38 |

The initial series contains one smoke run plus twelve interleaved pairs.
The later series alternates idle, afterhost and concurrent modes eight times.
This is 49 runs and 225 scene checks, not 49 complete five-scene passes.

- All active ARM witness checks passed. Idle mode reports zero checked pixels
  and is **not** counted as ARM pixel validation.
- Eight standalone runs failed: 35–946 incorrect pixels. All delayed reads passed.
- [concurrent-07.txt](concurrent-07.txt) records the paired discrepancy:
  scene 2, ARM `checked=3502 bad=0`; 68k `checked=3502 bad=35`; delayed 68k `bad=0`.
- Guards and texture checks remained intact. No GPU or witness timeout occurred.
- All 37 Core1 launches passed the runtime mapping probe, returned through the
  epilogue, restored the recorded SCTLR and released their Exec allocation.
- Failed first reads still return error 20. A later good read does not convert
  failure into success.

## What the controls establish

The idle and afterhost passes mean that concurrent ARM pixel reads are **not
necessary** for these recorded passes. The witness launch adds setup, shared
mailbox reads and a potentially one-tick wait before submission. Those changes
are confounders, not production fixes. The remaining concurrent failure also
rules out treating the witness itself as a cure.

The raw ARM wait values are global-timer ticks, not converted milliseconds or
an FPS measurement. Host timings use timer.device. The ARM watchdog is bounded
at 500,000,000 ticks and checks the cooperative stop flag on each marker poll.

Next useful controls: reproduce the mailbox-read/prepare delay in the standalone
probe without a pixel-reading worker; then record failing addresses and the
order of both observers' reads. Do not add a production delay or infer that
Core0/FPGA command retirement is broken from this evidence alone.

## Implementation and safeguards

- `probe_readback.h` holds one endian-aware pixel oracle used by both processors.
  Boundary pixels and ambiguous texture-edge samples remain excluded, as in
  the original probe; alpha tolerates one RGB565 quantisation step.
- `probe.c` retains the standalone entry point. A separate build supplies the
  existing launcher's `ff_window` callback, reusing that launcher's exclusive
  runtime port instead of trying to claim it twice.
- `witness_entry.S` comes from the existing tested ARM entry framework. The
  worker uses local instruction cache only; MMU/data cache stay off. It never
  changes global L2 state, GPU registers, textures or framebuffer memory.
- The launcher reserves a real 128 KiB Exec allocation, verifies its ARM mapping
  with eight nonce-derived words, relocates the worker, and synchronously resets
  Core1 before freeing memory. GPU targets stay in the published scratch pool.
- Host command, release flag and ARM result occupy separate cache lines, with
  one writer per region. Sequence/nonce, scene and pixel bounds are checked.
- In concurrent mode the host releases the worker immediately after writing
  the Core0 submit register. The worker does not read pixels before that release.
  In afterhost mode release follows the first 68k verification; idle skips all
  GPU-memory reads. There is no log I/O before the first 68k pixel check.
- Core1 errors are separate from the 68k bad-pixel count.

## Reproduce

Build locally; the script does not deploy or run anything:

```sh
python3 scripts/build_dosbox_glide_witness.py
python3 scripts/test_dosbox_glide.py
python3 tests/amiga/dosbox/glide/witness_model.py  # requires Unicorn and ARM binutils
```

Only run on a supported, idle physical target under the shared gateway lease.
From an Amiga CLI, after copying the diagnostic to RAM:

```text
Stack 65536
Set GLIDEWITNESS concurrent
RAM:ZZGlide-Witness
```

Use `idle` or `afterhost` in place of `concurrent` for the controls. These are
local shell variables, not saved launcher preferences. The original standalone
probe remains available with its original default pre-submit preparation.

Native ASan/UBSan checks cover all five scene expectations and deliberate pixel
corruption at heights 65 and 72. The actual upstream firmware validator accepts
306 encoder cases and ten probe fixtures. Six compiled-worker instruction-model
cases check valid/corrupt readback, idle mode, invalid nonce/range and cancellation
before release; they assert no pixel reads before release and no GPU-memory
writes. Unicorn supplies a synthetic Core1 MPIDR because its A9 model exposes
Core0. These model tests do not simulate Zorro, cache coherence or GPU timing.

Raw logs and build manifests are included here. `initial-build.json` identifies
the first concurrent-only binary; `control-build.json` identifies the three-mode
binary. Both are historical manifests, not a promise that later documentation
or comments retain the same source hash. All binaries used were temporary RAM
probes. After testing the Amiga responded, Core1 was idle, probes were removed,
and the shared gateway lease was released.
