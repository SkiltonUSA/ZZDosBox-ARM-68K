# File reads, mouse input and paging fallback

This development pass targets filesystem round trips and input/CPU scheduling.
It does not change the Zorro bus, firmware or CPU clock.

## DOS file read-ahead

Small reads from a read-only DOS handle can use a 4 KiB ARM-side buffer. Larger
requests continue through the existing 4 KiB mailbox transfers. Each of the
16 supported host handles has bounded storage: at most 64 KiB of file buffers.
Cache hits still service cancellation and input.

Before writes, zero-length truncation, creation, deletion or renaming, all
open-file buffers discard unread data and restore their host positions. This
is necessary because AmigaDOS can adjust handle positions during a mutation.
An initial lazy-invalidation candidate passed the model but failed the physical
alias/truncation test and was rejected. Seek restores the logical position;
EOF is not cached, so later reads can observe appends. Writes stay synchronous.

Read-ahead applies to the shared Amiga drawer used as DOS C:. It is separate
from the existing BIOS disk-image sector cache and does not accelerate booted
Windows image access by itself. Concurrent edits from another Amiga application
are not cache-coherent; close/reopen the DOS file after external modification.

Build with `--pc-file-readahead`. The switch leaves historical build recipes
unchanged and permits comparisons with the unbuffered path.

## Mouse event handling

Consecutive motion events with unchanged buttons are combined at their latest
position. A press/release or invalid-position event ends a batch, preserving
click order and coordinates. Leaving the painted area without a held button
resets relative-position tracking, avoiding a jump on re-entry. The guest
still receives at most one notification per tick.

This is not continuous pointer capture or host-pointer warping. Games that need
unlimited relative motion still require further work and interactive testing.

## Opt-in paging JIT

The existing register-only paged JIT remains experimental and off by default.
Its previous one-instruction interpreter fallback caused excessive dispatch
and event-loop work during Windows startup. A physical run reached its
10-million-slice test limit after 4,437 emulated ms / 40,353 wall ms; the normal
paging path completed 6,000 emulated ms in 26,975 wall ms. The stopped run
returned cleanly to Workbench with owned memory released.

The candidate interprets unsupported instructions in bursts of up to 32 cycles,
never exceeding the current scheduler allocation. It preserves the allocation
when splitting a quantum. Memory operands, branches, faults and x87 remain in
the paging-safe interpreter; the translated subset is unchanged. This targets
fallback overhead without expanding the set of JIT-supported instructions.

## Verification

- Sanitizer checks cover small/large reads, EOF growth, seeks, mutation/truncation,
  failure/retry and disabling the cache.
- The original `RDTEST.COM` fixture checks 8,192 one-byte reads, a second handle
  modifying buffered data, truncation, append and current-position seeking.
- Mouse tests cover ring wrap, buttons, focus release and BIOS PS/2 callbacks.
- Paged-JIT tests cover aliases, self-modifying code, instruction-fetch and
  read/modify/write faults, nested faults and privilege checks.

## Physical measurements

All times below are diagnostic runtime wall times on the A4000TX, not game FPS
or physical drive-throughput ratings. The read fixture makes 8,192 one-byte
reads and then checks alias writes, truncation, append and current position.

| Storage | Unbuffered | Read-ahead | Host reads | Result |
| --- | ---: | ---: | ---: | --- |
| RAM: | 16,964 / 17,358 ms | 293 / 294 ms | 8,198 → 8 | All data/coherence checks passed |
| SD032G: | 18,671 ms | 336 ms | 8,198 → 8 | All data/coherence checks passed |

The SD case used **98.2% less runtime wall time** and **99.90% fewer read RPCs**.
Total file-service requests fell from 8,250 to 64. Read-ahead transferred
17,759 bytes versus 8,909, trading bounded over-read for fewer round trips.
This is a small-read latency result; large reads and CPU-bound gameplay should
not be expected to improve by the same factor. Both versions wrote exactly
8,196 bytes and removed their scratch file successfully.

### Windows paging comparison

Each run used a fresh RAM: copy of the same 32 MiB Windows disk image, 8 MiB
guest RAM, 6,000 cycles, windowed display and a 6,000 ms guest-time target.
The user's disk was read only as the copy source.

| Paging mode | Guest time | Runtime wall time | Outcome |
| --- | ---: | ---: | --- |
| Previous one-instruction fallback | 4,437 ms | 40,353 ms | Hit the diagnostic 10-million-slice limit; clean error exit |
| Current normal paging | 6,000 ms | 26,516 ms | Completed; clean exit |
| Batched register-JIT fallback | 6,000 ms | 29,806 ms | Completed; clean exit |

The new batched mode avoids that test-budget failure but remained **12.4% slower**
than normal paging in this single matched startup comparison. It is not promoted
to the default. This does not establish sustained Windows application or WinBench
performance. Further work needs a more useful translated subset and measured
translation/dispatch costs, rather than enabling paged JIT for every program.

All completed physical runs restored registers and cache witnesses and released
the owned ARM allocation. The failed old paging test also released its allocation.
No firmware, user program data or default profile settings were changed.

## Integration and installation

The final Compat and Expanded builds passed physical EMS/XMS, x87 and SB16
diagnostics. Expanded also passed COM1 TCP round-trip. Net-JIT passed a
65,539-byte exact HTTP transfer, including with EMS and Sound Blaster active,
and an IPX socket/broadcast/receive/ESR exchange with audio. AHI queues drained
without errors. These are protocol and lifecycle checks, not listening tests.

Launcher, Compat, Expanded and Net-JIT 0.22.2-dev were installed and hash-checked
on the development Amiga. Configuration files stayed byte-for-byte unchanged;
previous versions were retained for rollback. The launcher reopened successfully.

[Hardware result counters](../amiga/records/2026-10-17/dosbox-file-read/physical-results.json) ·
[Exact development binaries](../artifacts/development/runtime-0.22.2).
