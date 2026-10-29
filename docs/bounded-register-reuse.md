# Bounded ARM register reuse — 0.23.0-dev

The eight-instruction lookback passes correctness checks but does **not establish
a useful game-speed gain**. It remains an explicit research option, disabled in
the release and installed player.

## Implementation

The translator examines up to eight already emitted A32 instructions when it is
about to load a word from the resident `cpu_regs` base in r10. If an unchanged
r0–r3 still contains that word from an earlier load or store, it emits a
non-flag-setting register move, or removes the load when source and destination
are equal. This search happens during translation, not on every execution.

The search stops at branches, calls, unknown instructions, unknown/partial memory
writes, overlapping stores, base-register changes and writes to the PC. It tracks
intervening register clobbers. Only explicitly decoded unconditional arithmetic,
MOVW/MOVT and r10-based word accesses are crossed. Block-entry, branch-target and
patch boundaries retain the existing reuse floor. ARMv7 little-endian and r10-base
contracts are compile-time assertions.

This extends the original immediately-adjacent store forwarding. It is not a full
register allocator, cross-block residency or speculative memory access. Stores
remain; helpers can observe CPU state. Guest flags remain intact. The existing
backend already patches some dead flag-helper calls; sampled CF/ZF consumers are
live and cannot simply be removed.

## Build

Use the recorded 32 MiB baseline, preserving its product version and options:

```sh
python3 scripts/build_dosbox_dos_arm.py \
  --base <recorded-32mib-build> --output .context/reuse-span8 \
  --pc-jit-reuse --pc-jit-reuse-span 8
```

Span 1 retains the original adjacent-only experiment. Span 8 requires explicit
`--pc-jit-reuse`. Both are recorded in `build.json`; neither is a release default.
The tested build retains 32 MiB executable cache, a 64 MiB allocated runtime,
8 MiB guest RAM, x87/VFP, SIMD preservation and the existing checked-read path.
Guarded writes and batched host audio remain off for this comparison.

## Validation

- 6,000 A9 instruction-model comparisons, including 3,104 accepted replacements:
  identical registers, memory and NZCV flags; explicit alias/clobber/control
  boundary rejection cases. Original 320 adjacent-forwarding cases also pass.
- Linked x86 register chains, mixed byte/word/dword writes and carry consumers at
  two runtime addresses; self-modifying-code invalidation and checked reads/writes.
- Paging aliases, fetch/RMW/nested faults, high-bank rejection and cancellation.
- x87 arithmetic/rounding/state, PIT math and complete SIMD/FPSCR restoration;
  denied SIMD access fails before executing NEON.
- 16,384 allocation alignment checks, real ARM/68k build and missing-MMU-header
  rejection. Base records reject changed versions, core pins and artifact hashes.

The first build selected an inactive Podman connection for the 68k link. Rebuilding
with the active connection completed successfully before hardware deployment.
This was a local build failure, not an Amiga failure.

## Physical Duke comparison

A4000TX / ZZ9000, Duke 1.3D opening level, 10,000 cycles, S3/VESA, fullscreen
adaptive presentation, SB16 sound/music. Same private media/configuration.
Reference / candidate / candidate / reference order; each game-render counter
sample lasts 30 wall seconds after five seconds of settling. No hot-block
instrumentation is enabled. Runtime source differences are the reuse integration;
the host additionally retains the compile-time MMU-layout check and prints the
reuse counter after measurement.

| Build | Game frames in each sample | FPS, individual runs | Mean FPS |
| --- | --- | --- | ---: |
| Reference | 147 / 147 | 4.900 / 4.900 | 4.900 |
| Eight-instruction reuse | 148 / 147 | 4.933 / 4.900 | 4.917 |

The apparent **0.34%** mean increase comes from one extra frame in one run. It is
too small and inconsistent to justify promotion. The candidate makes 5,773 / 5,772
translation-time replacements across each whole run; these counts are not dynamic
instruction savings or FPS. Guest clock progress remains around 91–92% of wall time.

All four runs complete queued PCM, restore SIMD/register/MMU/cache witnesses,
exit zero and release their allocation. Late audio links remain: reference
818 / 813, candidate 837 / 817, about 36% of requests across the whole run. The
change does not solve audio pacing, and queue completion is not audible-quality
verification. No listening or interactive gameplay acceptance is claimed.

## Decision

Keep original defaults and retain the bounded pass only as a reproducible
experiment. Further register work needs profiling that counts frequently executed
loads and helper boundaries, rather than judging it by static replacement counts.
For consoles, the [fixed-presentation study](https://github.com/SkiltonUSA/ZZNes-ARM-68K/blob/main/docs/fixed-presentation.md)
also found no repeatable earlier-audio advantage once diagnostic work was excluded.

[Raw runs, build hashes, model checks and analysis](../amiga/records/2026-10-29/bounded-register-reuse/).
