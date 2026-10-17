# ZZDosBox runtime integration

The 0.22.1-dev candidate combines the existing network transport with the ARM
JIT and compatibility audio/memory services. It is based on the pinned
libretro core, not a replacement with DOSBox-X or DOSBox Pure.

## Runtime selection

| Program settings | Executable |
| --- | --- |
| Network on; core auto/dynamic | ZZDosBox-Net-JIT |
| Network on; core normal; EMS/audio off | ZZDosBox-Net |
| Network off | Existing program runtime |

The combined runtime includes the 4 MiB code cache, NEON pixel conversion,
pipelined presentation, VFP event scheduler, experimental x87, EMS/XMS,
SBPro2/SB16 and PC speaker. Networking alone leaves EMS and PCM audio off;
per-program choices enable them. TCP/IP and IPX are alternative transports.
The same choice applies to the launcher's network terminal and diagnostics.

This stage supports embedded DOS, 1–8 MiB guest RAM and fixed cycles up to
10,000. Expanded options, physical/TCP COM1, booted DOS/Windows networking,
OpenGL, Adaptive, scanlines and developer trace/memory tools remain separate.
The explicit normal core is never silently replaced with JIT.

Network and PCM queues occupy distinct ranges inside the existing owned
32 MiB allocation. A compile-time boundary check covers the combined layout.
The existing mapping probe, single-Core1 lifecycle and restoration checks are
unchanged. Socket handling stays on the 68k through Roadshow; no new firmware
interface or physical NIC ownership is introduced.

## File-service work removed

Previously every filesystem RPC rewrote 26 filename bytes and 512 directory
bytes in shared memory. The host requested cache maintenance for the 32-byte
filename span and 512-byte directory span even for an already-open file or
disk device. Ordinary handle RPCs used empty paths, so they did **not** normally
walk a nested directory tree; the unnecessary work was metadata publication,
validation and CurrentDir changes.

Only pathname operations now publish/consume this metadata. Reads, writes,
seeks, closes, disk-sector/CD operations and free-space queries use their
existing handles/device/root. Opens, creates, stats, enumeration, renames,
deletes and directory operations retain component and link validation.

Per handle/device request this removes 538 ARM byte stores and a 544-byte
host cache-maintenance range, plus path processing. These counts are not a
measured Zorro bandwidth or FPS improvement. Payload copies and synchronous
write semantics remain unchanged. The ARM and 68k sources are built together;
this change must not be applied to only one half of an embedded runtime.

## Verification

The host also reports `FILES_OP` counts, total service microseconds and maximum
service time for each operation. This distinguishes read/write stalls from
seeks and pathname requests; it excludes ARM waiting time and does not measure
physical drive throughput in isolation. Existing aggregate timings remain.

Native filesystem tests cover file/image ownership, permissions, paths/links,
read/write/seek/EOF, truncation and persistence. They also check that handle
operations neither enumerate paths nor synchronise metadata, even with stale
non-path bytes in the mailbox. Image cache tests exercise supported capacities.

Network model tests accept `--build <directory>` to test the actual candidate.
`--audio` runs EMS and SB16 fixtures, leaves an OPL tone active, then performs
DHCP/HTTP while checking nonzero PCM after incoming network traffic. IPX tests
exercise registration, guest socket/broadcast/receive and cancellation.
Native/model tests do not establish physical hardware behaviour or speed.

## Physical results

The final candidate passed these bounded checks on the A4000TX/ZZ9000:

| Check | Result |
| --- | --- |
| JIT + TCP/IP | DHCP and HTTP; downloaded 65,539 bytes matching the source exactly |
| JIT + TCP/IP + audio | EMS/XMS fixture, SB16 DMA/IRQ fixture, held OPL tone during HTTP; 47,104 AHI frames completed, no audio or network errors |
| JIT + IPX + audio | Relay registration, socket, broadcast, receive callback, ping and disconnect; 3 packets each way, 52,224 AHI frames completed, no drops/errors |
| Updated Compat and Expanded | EMS/XMS remapping, x87 and SB16 fixtures passed separately on each runtime |

Each run exited with restored context/registers, intact cache witnesses and
owned memory released. The ARM model additionally verified nonzero PCM after
incoming network traffic. These are transport and audio-delivery checks, not
subjective listening or multiplayer-game compatibility tests.

The TCP/IP workload took 6,372 ms without audio and 7,564 ms with the additional
EMS/audio fixtures. These single-run totals include startup, DOS utilities,
transfer and exit; they are not comparable game-FPS benchmarks. The existing
interpreter completed the no-audio workload in 8,190 ms, but the builds differ
in several optimisations, so this does not isolate a JIT speedup.

The no-audio workload issued 1,535 reads: 751,009 us total host service time,
7,194 us maximum. Its 32 stat requests took 63,300 us total. This identifies
small-read batching/read-ahead as a useful next experiment; it does not prove
the physical disk itself is the bottleneck. Writes remain synchronous.

Two harness mistakes were corrected before accepting results: HTGet's HTTP
success return code was initially treated as failure, and case-only filenames
collided on AmigaOS. The latter caused configuration rejection before Core1
started. Neither was an emulator crash.

Evidence is retained privately under `.context/amiga/dosbox-integration/`,
including final build hashes, native/model logs and physical run logs.

## Next work

- Bounded file read-ahead and coherence checks are now measured in the
  [0.22.2 follow-up](amiga-dosbox-file-read-optimisation.md).
- Windows paging fallback was profiled and batched; normal paging remains faster
  in the measured startup and remains the default.
- Motion coalescing and re-entry resets are implemented; continuous relative
  pointer capture still needs further work.

No game-FPS improvement or Windows-networking support is claimed by this stage.

## Installation

Launcher 0.22.1-dev, Net-JIT, Compat, Expanded and the offline Network guide
were installed and hash-checked on the development Amiga. Existing interpreter
network runtime was retained. Configuration files were byte-for-byte unchanged;
replaced files and settings have `.pre-0221` backups. The launcher reopened.
Source and documentation were synchronized to the dedicated local repository;
this development update has not been published as a GitHub release.
