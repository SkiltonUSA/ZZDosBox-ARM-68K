# Game startup and Windows image I/O

These checks separate the shared-folder file cache from booted Windows disk
images. They use bounded startup workloads; they do not measure steady gameplay
FPS, time to a playable title screen, or Windows application benchmarks.

## Controlled game comparison

Both Compat builds use the same source hashes, compiler overrides, 4 MiB ARM
JIT, NEON pixels, pipelined presentation and VFP scheduler. The only feature
switch changed is `--pc-file-readahead` (0 versus 4 KiB per read-only handle).
Each game uses a freshly copied SD-card folder, 6,000 CPU cycles, a 640×480
Workbench window and a 6,000 ms guest-time limit. The run order is baseline,
read-ahead, read-ahead, baseline. Originals and user saves are not modified.

| Game | Guest RAM | Startup command | EMS / sound |
| --- | ---: | --- | --- |
| Vette | 2 MiB | `LOADFIX VETTE.EXE` | Off / off |
| Gods | 4 MiB | `GODS.COM` | Off / off |
| Pinball Fantasies | 8 MiB | `PINBALL.EXE` | On / SBPro2 |

The installed launcher still routes plain profiles without EMS/audio to the
older standard player. These measurements explicitly select Compat in both
arms of the comparison; they do not establish that all installed profiles
already use the new file cache.

## Game results

Two runs per build, all at 6,000 ms guest time:

| Game | Unbuffered wall ms | Read-ahead wall ms | Change in mean wall time | Read RPCs per run |
| --- | ---: | ---: | ---: | ---: |
| Vette | 14,955 / 14,971 | 15,003 / 14,997 | +0.25%; no meaningful gain | 82 → 83 |
| Gods | 14,598 / 14,534 | 14,564 / 14,582 | +0.05%; no meaningful gain | 154 → 135 |
| Pinball Fantasies | 11,926 / 12,198 | 11,628 / 11,431 | **−4.41%** | **333 → 183** |

Pinball Fantasies completed this startup workload in less wall time in both
read-ahead runs. Read calls fell 45.0%, while bytes fetched rose 2.6%. Each run
presented 356 frames and completed all 191,488 queued audio samples with no
audio error. This modest whole-workload improvement is separate from the much
larger synthetic one-byte-read result; it is not a measured gameplay-FPS gain.

Vette and Gods presented 418 frames per run. Vette's large-block read pattern
needs almost no buffering; Gods spends much more time resolving filenames than
reading bytes. No launcher runtime-routing change was made from these results.
All twelve runs completed their bounded lifecycle and restored registers,
SIMD context, cache witnesses and owned memory without reported errors.

## Windows copy experiment

The Windows interpreter already has a 1 MiB write-through sector cache. This
experiment keeps that ARM payload identical and changes only the 68k payload
copy: bounded byte loops versus Amiga Exec `CopyMem`. Cache maintenance,
mailbox ordering, synchronous disk writes and `Flush` remain unchanged. This
is a host-memory transfer experiment, not a larger-cache experiment.

A fresh copy of the same Windows disk image is placed on SD storage for each
run. Both hosts use 8 MiB guest RAM, 6,000 CPU cycles, the same windowed mode
and a 12,000 ms guest-time limit, in byte/bulk/bulk/byte order. The actual file
service with the candidate copies passed native AddressSanitizer and UBSan
checks for bounds, read/write/seek/truncation, path validation and exclusive
image ownership before physical testing.

### Windows results

| 68k payload copy | Wall ms | Mean wall ms | Mean disk-read service ms |
| --- | ---: | ---: | ---: |
| Byte loops | 64,541 / 64,594 | 64,567.5 | 9,102.5 |
| Exec CopyMem | 64,458 / 64,485 | 64,471.5 | 9,020.5 |

The measured mean reduction is only **0.15%** overall (96 ms), insufficient to
establish a useful optimisation. The candidate remains research and was **not
installed or applied to production source**. Its four physical runs completed
without reported errors, each presenting 772 frames for 12,000 ms guest time,
issuing 607 file-service calls and 209 host seeks, reading 2,383,872 bytes and
writing 12,288 bytes. Registers, cache witnesses and owned memory were restored.

The baseline profile assigns 54.4% of runtime to CPU execution, 11.8% to event
handling and 5.3% to display copying. Disk reads plus writes account for about
14.8% of wall time. These are profiling categories, not independently removable
costs. The results point away from bulk-copy replacement as the next Windows
optimisation; CPU/paging execution and backing-store latency are better targets
for further isolated measurements.

An initial image-copy command exceeded the bridge's synchronous wait limit;
no emulator was launched from that attempt. The copy completed normally, and
the harness was changed to wait for a completion marker before each measured
run. All reported runs used confirmed complete clones.

## Profiling interpretation

In the first baseline Vette run, the ARM profiler attributed 43.6% of runtime
to CPU execution and 23.0% to event handling. Only 0.39 s was spent servicing
reads. Buffering small reads cannot remove those CPU/event costs.

For Gods, the first baseline run spent 2.56 s opening files and 2.54 s on file
metadata requests, versus 0.44 s reading payloads. The shared-folder host walks
and enumerates directories to validate names and reject links. Reducing repeat
lookup work is a more relevant next experiment for this workload, provided
path validation, mutation semantics and external-file changes remain correct.
No directory-cache change is included in this update.

## Evidence

[Run logs, build hashes and research patch](../amiga/records/2026-10-17/dosbox-load-profile/)
include all twelve game runs and four Windows runs. The main installation keeps
the existing runtimes. Only the requested launcher layout/About changes were
deployed in this pass.
