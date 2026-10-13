# ZZDosBox optimisation review and candidate

13 October 2026. Candidate development work; no new physical performance
result or Empire combat fix is claimed. Installed mouse build is retained.

## Current priorities

Earlier physical Keen/Vette startup profiles put 36.6–38.0% of exclusive ARM
time in CPU/memory handlers, 18.7–23.1% in events, 13.4–14.6% in conversion,
9.1–12.6% waiting for a display grant, and 8.9–11.5% in mapping/scaling/copy.
These precede the current Windows/mouse build and are not Empire profiles.
Scheduler deadline caches and CPU-only O3 experiments previously failed to
produce a consistent improvement; do not promote them without new evidence.

1. Reduce avoidable scaling arithmetic and disk exchange overhead (this candidate).
2. Measure current per-game CPU, event, file and display time. The existing
   profile aggregates file service into surrounding scopes; add explicit I/O
   attribution before drawing detailed filesystem conclusions.
3. Consider bounded file read buffering for games using mounted drawers.
   Unlike an exclusively owned disk image, files can have several open handles;
   handle position, seek, truncation and cross-handle invalidation need tests.
4. Investigate x86-to-ARM dynamic recompilation as the larger CPU project.
   Current libretro source already has an ARM backend. Linux cache-flush SWIs
   must be replaced with a tested bare-metal executable-memory/cache contract.
5. Consider display overlap and idle reduction separately, preserving timer
   IRQs, input, cancellation and OS bitmap ownership. No firmware/Core0 changes.

## Exact scaler arithmetic

The generic nearest-neighbour row loop previously divided for each output
pixel. The candidate uses a quotient/remainder accumulator with the same
floor(x * source_width / target_width) result. Existing 1x/2x fast paths remain.
No NEON/VFP state or shared-memory layout changes are required.

ASan/UBSan differential testing passed 329,728 source/target/format cases,
covering source widths 1–1024 and every valid 4:3 window width 320–960.
The A9 instruction model passed all source widths, both formats, output widths
320/508/640/960 and buffer guards. These checks establish pixel correctness.

An isolated -Os Cortex-A9 instruction-count comparison (one row, format 3):

| Source → output width | Before | Candidate |
|---|---:|---:|
| 320 → 640 | 1,934 | 1,935 |
| 640 → 640 | 2,581 | 2,579 |
| 640 → 960 | 64,786 | 10,599 |
| 320 → 960 | 60,607 | 10,599 |
| 640 → 508 | 34,551 | 5,645 |
| 800 → 640 | 42,597 | 7,093 |

These are executed instructions in Unicorn, **not hardware cycles or FPS**.
Normal 1x/2x output should gain little; generic scaling is the target.
`scale-instructions.json` records results. `count_scaler.py` is the local
benchmark harness; its baseline header is preserved alongside it. Run from
the workspace with `.context/amiga/dosbox-optimisation/scale-before.h` present.

## Disk-image read-ahead

A 4 KiB ARM-private cache batches up to eight adjacent BIOS HDD sectors using
the existing bounded mailbox protocol. The host already accepts these batches.
Writes remain immediate and invalidate the cache before attempting I/O. Open
resets the cache, including failed opens; every access checks cancellation.
A failed speculative batch retries only the requested sector. End-of-image
reads remain bounded. No guest RAM increase or delayed-write policy change.

The independent disk model passed random reads/writes, read-after-write,
failed-write invalidation, reopen, short final batch, failed read-ahead fallback,
invalid bounds and output guards under ASan/UBSan. Reading 19 consecutive
sectors needed three transfers instead of 19. Random reads can transfer more
unneeded bytes, so actual loading time still needs physical comparison.

This cache covers booted DOS/Windows disk images. Empire's current mounted
game drawer uses the separate DOS file-service path and does not benefit
directly from this cache. It is not an Empire fix claim.

## External reference review

The owner clarified that `hard drive data rate limit=0` came from a forum,
not an Amiga experiment. [DOSBox-X's documentation](https://github.com/joncampbell123/dosbox-x/wiki)
describes zero as disabling its throughput limiter. No matching limiter or
configuration key exists in the reviewed ZZDosBox sources.

Reviewed [dosbox-rewrite](https://github.com/joncampbell123/dosbox-rewrite)
at `659519f0bda38b0ec454c40ccd1371144077d8e8` (latest commit reported by
GitHub on review: 12 July 2020). Its README explains 1 ms execution budgets,
events, callbacks, CPU cores, paging and VGA memory handlers. Useful audit
leads include wide memory access paths and avoiding needless byte-by-byte
handler calls. These are leads, not measured improvements to this port.

The [CRTC rewrite design](https://github.com/joncampbell123/dosbox-rewrite/blob/659519f0bda38b0ec454c40ccd1371144077d8e8/REWRITE-TASKS/Video-standard-crtc-model.md)
focuses on hardware timing, pixel clocks and memory layout; the
[wait-state notes](https://github.com/joncampbell123/dosbox-rewrite/blob/659519f0bda38b0ec454c40ccd1371144077d8e8/REWRITE-TASKS/20200306-1327-Twitter.txt)
discuss emulating video bus contention and CGA snow. These are accuracy
features, not evidence of an A9 speedup. Retain the existing port and consult
specific sections when profiling identifies a relevant bottleneck.

## CPU-settings guide follow-up

The [DOSBox-X CPU guide](https://github.com/joncampbell123/dosbox-x/wiki/Guide%3ACPU-settings-in-DOSBox%E2%80%90X)
distinguishes normal interpretation, dynamic recompilation and the real-mode
simple core. It warns that excessive cycles can cause lag; CPU model labels
and its approximate cycle table do not establish real-PC speed equivalence.
Its dynamic_rec paging limitations also argue for retaining an interpreter
fallback when exploring recompilation for Windows workloads.

Our current player implements fixed budgets and manual live adjustments,
not the guide's adaptive max/auto scheduling. Investigate a bounded automatic
budget only after measuring real elapsed time against emulated time, separating
I/O and presentation stalls, with a fixed mode retained for timing-sensitive
games. No cycle settings were changed on the running Amiga.

The pinned libretro simple core fetches instructions through direct MemBase
pointers. Our adapter currently rejects that decoder explicitly. It cannot be
enabled as a harmless setting: instruction bounds, A20 aliases, ROM mappings,
protected-mode transitions and fallbacks need validation. This may be a smaller
CPU experiment than a JIT, but no performance improvement is established.

## Remaining validation

The ARM/68k build and full linked ARM model regression suite passed, including
HDD read/write/bounds, paging, A20, graphics, input, MIDI, CD and cancellation.
Candidate executable: 978,232 bytes; SHA-256
`64598728b83dbcb8237b656f252923d95a274005a93c65fb04992ff50805e41b`.
Artifacts are retained under `.context/amiga/dosbox-optimisation/`; original
installed-build hash was preserved and verified. Build warnings originate in
upstream VESA packed fields, libc macros and the existing RWX linker layout.

Before promoting it, compare physical loading and scaling with the preserved
mouse build. Do not interrupt the owner's active session. Empire's reported
combat slowdown remains a separate reproduction after the overall work.
