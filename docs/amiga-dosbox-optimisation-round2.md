# ZZDosBox: disk, launcher and runtime optimisation

This round targets disk stalls, launcher memory, MIDI servicing, indexed-pixel
capture and Windows paging/JIT behaviour. Measurements use the A4000TX's
ZZ9000 Cortex-A9 with XX19c/XACP 1.7. Each hardware experiment runs alone on
Core1; guest disk tests use a fresh copy of the installed image.

## Windows disk cache

The existing cache reads eight sectors at a time and keeps writes synchronous.
A replay of an observed sector trace suggested that its 128 KiB capacity was
causing repeated reads. The experiment increases capacity without changing the
4 KiB transfer size, cache policy, guest cycles or write behaviour.

| Cache capacity | Run 1 | Run 2 | Mean elapsed | File requests | Read bytes |
| --- | ---: | ---: | ---: | ---: | ---: |
| 128 KiB baseline | 77.579 s | 77.379 s | 77.479 s | 948 | 3,780,608 |
| 512 KiB | 68.349 s | 68.244 s | 68.297 s | 667 | 2,629,632 |
| 1 MiB | 66.377 s | 66.442 s | 66.410 s | 607 | 2,383,872 |

The 1 MiB candidate takes **14.29% less elapsed time**, equivalent to **16.67%
more throughput for this workload**. File requests fall **35.97%**, and mean
host read time falls from 19.892 to 9.644 seconds (**51.52%**). This is a timed
Windows startup workload, not a general Windows benchmark or steady gameplay
FPS claim. Every accepted run performs 12,000 guest milliseconds, presents 772
frames, performs the same 2,148,475 slices and writes 12,288 bytes.

Order: baseline, 512 KiB, 1 MiB, 1 MiB, 512 KiB, baseline. The window, 8 MiB
PC RAM, 6,000 cycles, FPS display and profiling settings are identical. A fresh
image copy precedes each run and is outside the runtime's elapsed measurement.
The first exploratory baseline was manually stopped early and is excluded.

Use **1 MiB for both Windows runtimes** (interpreter and guarded JIT). It uses 896 KiB more
cache data within the existing 32 MiB owned allocation; guest RAM remains
8 MiB. The installed DOS-game runtime is unchanged. Its older image uses the earlier
4 KiB read-ahead path; 128 KiB is the current source-build default, not that
installed binary's capacity. Larger replay candidates
show diminishing request reductions and have not established a hardware gain.
This cache applies to BIOS hard-disk images, not every host-directory file read.

Writes invalidate the affected cache block before synchronous transfer,
including failed or partial writes. Exclusive image ownership, bounded
transfers, mapping checks and teardown remain mandatory. The cache-only reproducible build matched the ARM image, relocation list and
entry point of the measured 1 MiB candidate exactly. The installation build
also includes the library-state fix below and receives a separate hardware
validation run.

Build with:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-disk-cache-kib 1024
```

The build accepts 128, 256, 512, 1024 or 2048 KiB and isolates non-default
outputs. Native tests cover eviction, bounds, partial failures and write
invalidation. ARM instruction-model tests cover BIOS geometry, cross-track
read/write, paging, display output and cancellation. Hardware logs verify
register/cache restoration and owned-memory release separately.

## Launcher 0.16.0-dev

The executable shrinks from **1,844,592 to 493,068 bytes (73.27% smaller)**.
This is executable size, not a measured total-process RAM reduction. The
searchable title index stays compiled in; per-game details move to
`Compatibility.dat`. Selection reads one bounded record, with a 62,912-byte
index and an 8,192-byte record buffer. The entire data file is not loaded into
RAM. Existing version tables, topic summaries and report links remain.

The file uses explicit big-endian fields, a versioned header, index CRC,
per-record CRC, sorted IDs and validated lengths/offsets. Native tests compare
all 3,932 records with the source data and reject missing, stale, truncated,
corrupt or extended files. On the Amiga, the launcher remained usable with the
file missing and recovered after it was restored, without restarting. Installed
settings were compared byte for byte before and after replacement.

Keep the matching `Compatibility.dat` beside `ZZDosBox-Launcher`. Markdown
references remain available separately. The build manifest includes these
assets. Upstream compatibility reports do not certify ZZDosBox behaviour.

## MIDI servicing

ZZDosBox currently has **external CAMD MIDI, not PCM/AHI playback**. There is
therefore no existing AHI playback buffer to tune in this port.

Previously each service batch requested cache maintenance for the entire
4,096-byte shared MIDI ring. It now requests only the published span being
consumed: at most 16 bytes, split into two spans at wraparound. Actual cache
maintenance works at cache-line granularity; this is not a measured 256-fold
CPU or bus improvement. Batch rate, MIDI byte order, serial pacing and MT-32
SysEx delay are unchanged.

A test runs the production service body against mock CAMD, time and shared
memory operations. It checks byte identity, ring/counter wrap, 16-byte pacing,
SysEx delay, disabled output, disconnect and overflow. Physical internal CAMD loopback also verified **5,015 bytes / 5,005 ordered
messages**, exact note data and SysEx, with zero capture errors and clean
runtime restoration. The receiver used a private software cluster; no external
MIDI output was used. No listening test, external-device timing result or
gameplay FPS gain is claimed for this change.

## Library floating-point state

The longer Windows instruction-model test exposed a missing context guard in
scalar PC builds: the linked library's `fmod` routine contains VFP instructions,
and other linked string/conversion routines contain SIMD. PIT output queries
reach that math routine even when NEON pixel conversion is disabled. The old
model stopped on its first VFP instruction because coprocessor access was off;
this was not evidence of a guest paging failure.

All PC builds now use the existing checked SIMD-state guard. It saves all 32
D registers and FPSCR, sets IEEE round-to-nearest arithmetic without exception
traps, and restores the inherited state after normal, error and cancelled exits.
It respects firmware permission and never enables CPACR or FPEXC itself.
`--pc-neon` still selects pixel kernels; the context guard no longer depends on
that rendering option. The installed DOS PIC/VFP runtime already has its guard.

An original `PITMATH.COM` fixture programs the emulated PIT and polls its output,
reproducing the missing-guard failure in the old scalar build immediately.
This gives a small regression test without requiring a Windows image. The new
checks also exercise hostile inherited FPSCR settings, denied access, and exact
register restoration. A longer Windows interpreter instruction-model run also
completed 4,000 guest milliseconds and 279 frames, including protected-mode
execution and exact SIMD-state restoration. Model wall time is not an A9
performance measurement. Hardware validation must report `context_equal=1`.

## Windows paging and JIT

The production paging fallback remains enabled. An isolated ARM instruction-
model build with that guard removed passes five small tests: demand paging,
virtual code remapping plus physical-alias self-modification, an instruction
spanning a missing page, and carry/register preservation across a
read-modify-write page fault, and a nested fault inside a fault handler. The guarded production build is tested separately.

These are useful regression fixtures, **not evidence that Windows can safely
use paged JIT**. They do not cover arbitrary fault depth, privilege changes,
interrupt-driven task switches or all linked-block restart paths. No unguarded
payload was installed or run on the physical Amiga. The next JIT milestone is
repeatable fault/restart and address-space-switch coverage followed by an
isolated Windows boot test, before any default changes.

## Rendering experiment

The candidate compiles frame capture with `-O2` and converts four indexed
pixels per loop iteration. It is opt-in through `--pc-capture-unroll`; the
normal capture path is retained. Pixel tests cover palette indices, 15/16/32-bit
input, row alignment, widths and guard bytes. Linked ARM tests compare exact
frame output, guest text and scheduler traces, plus resize, cancellation and
SIMD-denied handling. The controlled hardware candidate relinks only the capture translation unit
against the installed DOS runtime's ARM objects. A fresh whole-source build also
includes intervening disk/paging changes and is not treated as that isolated
comparison. Instruction-model timing is not used as a speed measurement.

The first full Doom candidate run reached the complete demo frame sequence but
failed a deferred framebuffer acknowledgement during its return to the DOS
screen. Reset/teardown restored checked state and released owned memory, and
Workbench/bridge remained usable. That run is excluded from accepted timing
results. Diagnostics now include the previous/observed acknowledgement and
elapsed wait, without weakening the timeout or buffer-ownership rules.


The accepted rendering baseline runs measured **7.47990 and 7.50487 FPS**
(mean **7.49239**); the clean candidate repeat measured **7.48873 FPS**.
The observed difference is **−0.05%**, inside baseline variation. No rendering
speed improvement is established, and the candidate is not installed. The
repeat uses the identical ARM payload plus failure-only host diagnostics.
The failed first run remains recorded but is excluded from the comparison.


## Final validation and installation

Both final Windows executables include the 1 MiB cache, mandatory SIMD-state
guard and bounded MIDI maintenance. Each completed a separate physical Windows
startup lifecycle from a fresh image copy, at 6,000 cycles and 12,000 guest ms:

| Final build | Elapsed | Frames | File requests | Read bytes | Outcome |
| --- | ---: | ---: | ---: | ---: | --- |
| Interpreter | 66.755 s | 772 | 607 | 2,383,872 | Passed |
| Guarded JIT | 67.748 s | 773 | 607 | 2,383,872 | Passed |

Both reported `context_equal=1`, zero register mismatches, zero witness failures,
closed image handles and released owned memory. The JIT run reported 280,753
paged fallbacks. Its frame/slice totals differ from the interpreter; these two
single validation runs are not a controlled JIT speed comparison. The repeated
cache comparison above is the basis of the reported 14.29% improvement.

Installed launcher **0.16.0-dev**, its matching compatibility data, and both
validated Windows runtimes. The previous executables are retained with a
`.pre016` suffix. The normal DOS-game executable remains unchanged because the
rendering candidate showed no repeatable benefit. Launcher profiles and
`dosbox.conf` remain byte-identical; `core=auto` remains the default.

[Evidence and reproduction notes](../amiga/records/2026-10-14/dosbox-optimisation-round2/README.md)
include artifact hashes, accepted/rejected measurements, native tests and
instruction-model checks. This work does not enable unguarded paged JIT or PCM
audio, and no general game-FPS improvement is claimed.

## Console synchronization and review cleanup

The PC, DOS shell and console hosts now share `console/stream_host.h`. An empty
output ring only synchronizes its producer index; it no longer synchronizes
4096 payload bytes or republishes an unchanged consumer index. Nonempty input
and output synchronize exactly the occupied span, split into two at wraparound.
Payload synchronization still precedes index publication. Output is consumed
only after the console accepts it. Bounds checks remain at the ring entry
points, outside the per-byte loops.

These callbacks use `CacheClearE`, not explicit bulk Zorro reads. Their byte
ranges are cache-maintenance requests, not measured bus traffic. The change
removes unnecessary work, but establishes neither a 4 MB/s bandwidth saving nor
a gameplay FPS gain. Measurements are needed before making either claim.

The unchanged three-buffer ownership algorithm now lives in
`amiga/common/flip_plan.h`. DOSBox no longer needs the NES include directory.
The NES compatibility header and SNES staging scripts use the common source;
source manifests record the shared dependency.

Network receive now drops and counts complete packets that exceed the caller's
buffer, then looks for the next fitting packet. It never truncates a packet.
Normal MTU-sized callers are unaffected; a NULL buffer leaves the queue alone.
The scanline dimmer explicitly documents that its fourth byte is unused by the
supported P96 scanout formats.

Validation: `python3 scripts/test_dosbox_host_review.py` exercises the production
ring helpers with separate simulated CPU views (empty/full, wrapping counters,
exact payload ranges, publication order, output retry and corrupt indices),
the production network queue (oversized packets, recovery, full queue, NULL and
zero capacity), and existing DOSBox/NES presentation state tests under ASan and
UBSan. `python3 scripts/test_dosbox_network.py` covers the existing network
mailbox, configuration, control and IPX contracts. These native tests do not
establish physical cache behaviour or Amiga execution.

The console, shell and PC ARM/68k builds passed (68k hosts use `-Werror`), and
the Amiga network archive built. Linked ARM console tests passed normal output,
read-stop and write-stop at two relocated addresses. A RAM-only candidate on
the physical A4000TX reproduced all 120 lines of a 5,640-byte text fixture:
5,932 transcript bytes including shell output, hash `6d85b2f0`, 559 frames and
8,000 emulated milliseconds. Both installed baseline and candidate returned
exit=0 with no register/MMU/cache-witness mismatches and released owned memory.

Single-run elapsed values were 19,918 ms baseline and 20,534 ms candidate.
There is **no demonstrated speed gain**. This is a correctness smoke test,
not a controlled host-only performance comparison: the installed runtime and
fresh source build have different worker provenance. The installed applications
remain unchanged. Private build records, logs and test fixture are under
`.context/amiga/dosbox-review/`. No GitHub publication was performed.
