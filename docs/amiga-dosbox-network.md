# ZZDosBox networking prototype

Status: experimental TCP/IP proof passed on the physical ZZ9000 after the
portability/stack fixes. The 0.17.0-dev launcher now offers opt-in networking;
see [Network tab](amiga-dosbox-network-ui.md) for current controls and limits. TCP/IP is first;
IPX multiplayer is a separate follow-up. The user selected both.

## Architecture

DOS/Windows NE2000 guest adapter → ARM packet queues → 68k libslirp NAT →
Roadshow `bsdsocket.library` → the Amiga's configured network interface.

This shares the existing X-Surf100 connection. It can also use ZZ9000 Ethernet
if that interface is already configured in Roadshow; the application does not
choose, reconfigure, or take ownership of a physical NIC. No firmware changes
or direct ARM Ethernet register access are involved.

The [ZZ9000 manual](https://mntre.com/documentation/zz9000-manual-r1.pdf)
documents its SANA-II network driver. Raw promiscuous access is deliberately
avoided: [SANA-II](https://wiki.amigaos.net/wiki/SANA-II_Revision_7) describes
exclusive-open requirements that can conflict with an existing TCP/IP stack.

The guest adapter uses port `0x300`, IRQ 3, MAC `02:5a:5a:00:00:01`.
The private IPv4 subnet is `10.0.2.0/24`, gateway `10.0.2.2`, DHCP address
`10.0.2.15`. DNS forwarding requires an explicitly supplied DNS address;
automatic Roadshow resolver discovery is not implemented. There is no
incoming port forwarding, host process execution, IPv6, TFTP service or raw
LAN bridging. An NE2000 driver and guest TCP/IP software are still required.
Windows for Workgroups network drivers/TCP-IP installation are not yet tested.

## Implementation

- `amiga/dosbox/network/` holds the host NAT adapter, small GLib compatibility
  subset, shared protocol and ARM handoff.
- libslirp **4.9.5** is hash-pinned. NE2000 is adapted from the pinned
  DOSBox-X/Bochs-derived device; original source notices are retained.
- Each shared direction has four 1,600-byte slots, with a maximum Ethernet
  frame of 1,518 bytes. Each counter has a single writer and separate cache
  line. Full queues drop/backpressure without waiting for the network.
- The 68k service processes bounded batches and uses a zero-timeout socket
  poll. All socket ownership stays on the host Amiga task.
- The existing XACP Core1 ownership/mapping/teardown model is preserved.
  Network buffers are inside the application allocation, never a fixed DDR
  allocation claimed from firmware.
- Networking and the existing memory monitor currently occupy the same
  optional control region; the builder rejects enabling both together.
- Production hardening, including graceful libslirp allocation/assertion
  failure handling while Core1 is active, remains required before promotion.

## Verification and incident

| Check | Result |
| --- | --- |
| Native NAT with address/undefined-behaviour sanitizers | ARP, 38-byte TCP and UDP echo, close/reopen passed |
| Production host queues with separate simulated host/ARM cache views | Payload identity, full queues, counter wrap, invalid length/index, disabled backend and failure propagation passed |
| Standalone physical 68k probe, first two runs | ARP and 38-byte TCP echo passed; UDP did not |
| Third standalone physical probe | TCP reached the Mac; bridge connection subsequently dropped; final result and cleanup unknown |
| Linked ARM instruction model | NE2000 driver, DHCP and exact 4,099-byte HTTP download passed; SIMD/FPSCR restored |
| Network-disabled ARM regression | EGA/PIT, shell and clean shutdown passed |
| Direct Roadshow UDP after recovery | Exact 32-byte echo, socket/library close passed |
| Corrected standalone physical NAT | ARP/TCP/UDP, 38-byte echoes and close/reopen passed |
| Initial integrated physical ARM guest | DHCP/HTTP GET, then Software Failure #80000004; no completed transfer/cleanup claimed |
| Corrected integrated physical ARM guest | DHCP, exact 4,099-byte HTTP file, normal exit and ARM state/memory restoration passed |
| IPX multiplayer | Not implemented |

The physical probe ran from `RAM:ZZNetProbe`. It did not launch an ARM worker,
replace ZZDosBox, or modify Roadshow/startup configuration. During the third
run (15 October, approximately 10:32 UTC), the last bridge heartbeat preceded
a TCP echo, then the connection went silent. We cannot yet distinguish an
Amiga fault, network failure or other interruption from that attempt. The user
subsequently reported Workbench responsive; the bridge reconnected and the old
RAM log was missing. The corrected standalone backend passed on the Amiga.

A subsequent full guest run obtained its DHCP lease and sent an HTTP request,
then the 68k host raised Software Failure **#80000004**. The requester identified
`RAM:ZZNetwork/ZZDosBox-Net`. It never produced the completion sentinel. No
byte-exact transfer, clean shutdown, SIMD/cache restoration or release has
been established for that physical run. The bridge remained responsive and
reported no saved crash context. The user subsequently restarted the Amiga. The stack defect below was fixed
and a fresh full guest run passed. No failed host task or Core1 owner remained
after that corrected run.

Do not treat successful native tests or two short TCP exchanges as proof of
Roadshow UDP, sustained transfers, Windows networking or safe physical cleanup.
Two portability defects were found: the BSD socket-address family layout, and
a missing `GLIB_SIZEOF_VOID_P` definition. The latter incorrectly made the
32-bit IP overlay 16 bytes instead of 20. The current code fixes both and
asserts packet overlay size/offsets at compile time on both host widths.
Native sanitizer TCP/UDP and the corrected standalone physical probe passed.
Neither fix establishes the cause of the earlier dropout or later host fault.

The linked ARM instruction model successfully ran the real DOS NE2000 packet
driver and mTCP DHCP client, exchanged two packets each way with the native
NAT backend and obtained `10.0.2.15`. The worker restored SIMD/FPSCR and exited
cleanly. After the layout correction, DHCP and a 4,099-byte HTTP download
passed with exact file contents (13 packets transmitted, 10 received). This
exercises real x86 emulation, card I/O, IRQs, ARM handoff and guest
file I/O, but substitutes the Mac native backend for 68k/Roadshow.

## Stack overflow found during the physical investigation

The cross-compiler stack report showed `if_encap` used **65,696 bytes**,
exceeding the entire 65,536-byte Amiga task stack before its callers. Upstream
allocates `IF_MTU_MAX + 100` bytes for jumbo-size packets. Our backend fixes IP
MTU at 1,500 and rejects Ethernet frames over 1,518 bytes, so that allocation
was unnecessary and unsafe on this host.

The portability patch now uses `ZZ_NET_MTU + 2` bytes and retains upstream's
length check. Measured function stack use falls to **1,596 bytes**. Every Amiga
NAT source build now rejects compiler-reported frames above 8 KiB. An explicit
regression compile reinstates the original array and must fail this gate.

After restart, the revised standalone physical probe passed ARP/UDP and an
exact **4,099-byte TCP echo**, followed by close/reopen (17 TX, 15 RX, zero
reported errors/drops). Native sanitizer and linked ARM-model DHCP/HTTP checks
also passed after the change. This is a concrete stack defect and a likely
cause of the requester. The corrected physical integrated run then passed:
DHCP on its first attempt, HTTP 200, exact 4,099-byte file contents, 14 transmitted
and 12 received frames, zero network drops/errors, normal guest exit, closed
files, restored SIMD/FPSCR/MMU/cache/register witnesses and released allocation.
The complete guest workload took 6.592 seconds; this includes guest startup,
file access and display work and is **not** a network throughput benchmark.
The failed run is retained as evidence and is not counted as a successful test.

## Build and local checks

```sh
python3 scripts/test_dosbox_network.py
python3 scripts/build_dosbox_network.py --native
python3 scripts/build_dosbox_network.py
python3 scripts/build_dosbox_network.py --socket-probe
python3 scripts/build_dosbox_dos_arm.py --pc --pc-network --pc-disk-cache-kib 1024 \
  --container-command '["podman","--connection","nuflix-converter-root"]'
```

The corrected runtime cross-build succeeded (1,087,780 bytes). Its ARM image
is 716,844 bytes inside the existing 32 MiB allocation. The corrected standalone
NAT probe with the larger echo fixture and stack fix is 104,928 bytes. Both
corrected binaries passed the physical checks described above.
The isolated UDP socket probe also compiles with warnings treated as errors.

Builds do not deploy or run anything on the Amiga. The network-enabled runtime
is isolated under `.context/amiga/dosbox-pc-cache1024-network/`. Its CLI accepts
`--network` and optional `--dns <IPv4>`; these were the original development options. The installed 0.16.0-dev launcher remained unchanged during that initial proof.
The subsequent 0.17.0-dev UI integrates a separate `ZZDosBox-Net` runtime and
`--network-config` input; the ordinary DOS/Windows runtimes remain unchanged.

Next work: graceful allocation/assertion failure handling, cancellation under
traffic, physical DNS checks, sustained transfers and a copy of the Windows
network setup, with the opt-in launcher controls now implemented. Preserve the single-Core1 preflight and
restoration checks. IPX requires its own UDP tunnel and interoperability tests;
IPv4 NAT does not automatically provide IPX. Direct ownership of the ZZ9000 NIC
is not part of this design.

### Guest instruction-model test

The private test inputs are DOSBox-X's embedded `NE2000.COM` and the official
[mTCP](https://www.brutman.com/mTCP/) 2026-10-10 binary archive. No driver or
mTCP binaries are committed. `model_test.py` reads the downloaded archive and
existing DOSBox-X research checkout, and runs under Unicorn 2.1.4.

```sh
python3 scripts/build_dosbox_network.py --native --model-library
.tools/dosbox-model-venv/bin/python tests/amiga/dosbox/network/model_test.py
```

Optional `--http-bind <local IPv4>` starts a temporary HTTP server bound to
that address, serving only an original 4,099-byte fixture to a local client.
The native model shared library is not sanitizer-instrumented; the separate
native probe and host-queue tests are instrumented.

Recorded evidence: `amiga/records/2026-10-15/dosbox-network/`.
