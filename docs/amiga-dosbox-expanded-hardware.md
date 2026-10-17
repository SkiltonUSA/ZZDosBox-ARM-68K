# Expanded PC hardware — 0.21.0 development candidate

The Expanded runtime adds optional PC devices and memory settings. Existing
profiles keep their previous settings. Selecting an expanded feature makes the
launcher choose `ZZDosBox-Expanded` automatically.

| Setting | Available choices |
|---|---|
| CPU model | 386, 486, Pentium |
| Guest RAM | 1, 2, 4, 8, 16 or 32 MiB |
| Cycles | Fixed, Auto, Max |
| Memory services | EMS/XMS, optional VCPI and UMB |
| Gameport | Two axes and two buttons, physical Amiga port 2 |
| Additional audio | Gravis Ultrasound, Game Blaster/CMS, Tandy |
| COM1 | Off, outbound raw TCP, basic outbound modem, experimental physical serial |
| Paged JIT | Experimental register-only integer subset; off by default |

CPU model selection changes the emulated instruction/identification behavior;
it does not promise the performance of a physical 486 or Pentium. x87 retains
the binary64 implementation from the Compat runtime, not full 80-bit precision.

## Memory and cycles

Expanded owns a 64 MiB ZZ9000 allocation, including up to 32 MiB of guest RAM,
its 4 MiB JIT cache, device state and working memory. The existing runtime still
uses 32 MiB. Allocation and MMU mapping are checked at launch; addresses are
never hard-coded from a previous run.

Auto starts at the selected cycle count and adjusts within 500–10,000 cycles
per guest millisecond. Max permits up to 100,000. These are bounds, not measured
sustainable speeds. The controller compares guest time against wall time every
half-second, reduces work when the guest falls behind and occasionally probes
upward when pacing keeps the guest near real time. Keyboard cycle adjustments
switch that session to manual control. Long stalls and pauses are ignored.

The configuration editor accepts `cycles=auto` or `cycles=max`, starting at
3,000, or a numeric `cycles` together with `cycle_mode=fixed|auto|max`.

VCPI requires EMS. UMB and VCPI are embedded-DOS services; booted MS-DOS supplies
its own memory drivers. Neither option installs HIMEM.SYS or EMM386.EXE.

## Audio and input

Sound Blaster and PC speaker remain available. Additional device addresses are:

| Device | Guest configuration |
|---|---|
| GUS | Base 240h, IRQ 5, DMA 3; ULTRASND=240,3,3,5,5 |
| Game Blaster/CMS | Base 220h; Sound Blaster must be off |
| Tandy | PSG and DAC; Sound Blaster must be off |

GUS requires EGA, VGA or S3 graphics in this runtime. Its instrument/patch files
are user-supplied where a game requires them; `ULTRADIR=C:\ULTRASND`. PCM output
uses the existing batched AHI path. Enabling a device does not install its DOS
drivers or configure every game automatically.

The optional gameport maps physical Amiga port 2 to one DOS two-axis,
two-button joystick through `lowlevel.library` V40. Digital directions map to
full axis travel; this is not an analog joystick adapter. The library uses
zero-based port index 1, which is physical Amiga port 2.

Launcher 0.21.2 provides aligned Joystick panels with a saved 1-6 physical
button selector. Buttons 1-2 remain DOS gameport buttons; buttons 3-6 have
selectable DOS key bindings (Off, Space, Enter, Ctrl, Alt, Z/X/C/V, Shift, Tab
or Escape). Initial extra bindings are Space, Enter, Z and X, active only when
the corresponding button count and gameport are enabled. One-button mode
ignores the second physical button. Keyboard and gamepad holds are merged so
releasing one source does not release a key still held by the other.

The tester shows the selected number of buttons, turning indicators green
while held. X/Y shows -1, 0 or +1. Testing is independent of gameport enable
and runs only while the page is open. Buttons 3-6 need a lowlevel-compatible
gamepad/adapter: Green, Yellow, Forward and Reverse respectively. Selecting
six buttons does not make an unsupported controller protocol readable and
does not emulate a native six-button PC gamepad protocol. Missing OS support
is reported in the tester. Older profiles retain two-button behavior.

## COM1

The I/O page selects a numeric IPv4 address and port. The 16550 UART, FIFO,
interrupts and baud timing run on ARM; nonblocking TCP sockets run in the Amiga
task through Roadshow's `bsdsocket.library`. Embedded DOS only. This transport
cannot be combined with the separate NE2000/IPX runtime in this candidate.

Raw TCP connects to the selected endpoint. Basic modem mode accepts `AT`,
`ATV1`, `ATZ`, `AT&F`, `ATE0`, `ATE1`, `ATH` and `ATH0`; `ATD`, `ATDT` or `ATDP`
without a number connects to the configured endpoint. Dropping DTR hangs up.
This is an initial outbound modem subset: no incoming calls, telephone-number
routing, Telnet negotiation, `+++` escape sequence or access to a physical
modem in this TCP mode. It is not a full Hayes-compatible modem.

## Physical COM1 (Experimental)

Choose **I/O → Connection → Physical serial (Experimental)**. Built-in selects
`serial.device`, unit 0. For a USB adapter or expansion card, enter its installed
AmigaOS IOExtSer-compatible device name and unit. One host port maps to DOS COM1
(3F8h, IRQ 4) per session. Multiple simultaneous COM ports are not implemented.

**Baud 0** follows the DOS UART's baud, data bits, parity and stop bits; the
initial setting is 9600 8N1. A nonzero baud fixes the physical settings to the
controls below it, useful when a DOS application expects different UART timing.
Physical framing supports 7/8 data bits, None/Odd/Even/Mark/Space parity and
1/2 stop bits, subject to the driver's limits. Built-in serial.device rejects
some combinations, including 8 data bits with two stop bits. Unsupported
settings are reported and stop the serial session rather than silently changing
its framing. Speeds above those verified with the actual hardware are untested.

The USB shortcut is disabled unless `poseidon.library` and a registered
`serialpl2303.device`, `usbmodem.device` or `serialft232r.device` are detected.
**Detect USB** refreshes this after connection. Registration establishes driver
availability, not a working adapter/unit. Other serial-compatible USB drivers
can be entered manually. No USB or expansion driver is bundled or installed.

Physical mode passes bytes, including AT commands, directly to your hardware;
it does not use the TCP modem command parser. Reads/writes are asynchronous,
buffered and bounded. Transmit bytes are consumed only after device completion;
full guest receive buffers stop new reads. Driver errors are reported in the
runtime log. CTS, DSR, carrier and ring input status reach the guest UART.
A guest break request sends a 250 ms break, rather than an indefinite level.

RTS/DTR outputs remain controlled by the Amiga driver. Optional 7-wire flow
control uses its hardware handshake; arbitrary guest RTS/DTR transitions,
bit-banging, precise edge timing and DTR-triggered hangup are not implemented.
Use the modem's command interface where available. This remains experimental
for serial-device development, not a replacement for a protocol analyser.

Ports are opened exclusively, then pending I/O is aborted/drained on exit.
Close other applications using the same port. Built-in serial mode rejects
simultaneous CAMD MIDI in the same DOS session. USB/expansion use with MIDI is
possible only when they use separate physical ports. Embedded DOS is required;
the separate NE2000/IPX runtime and booted disk-image DOS are not supported by
this COM1 implementation yet. Default profiles keep serial disabled.

Reference: [AmigaOS serial-device API](https://wiki.amigaos.net/wiki/Serial_Device).

## Paging experiment

The default remains interpreter execution while guest paging is enabled.
In 0.22.2, unsupported instructions use interpreter bursts of up to 32 cycles,
bounded by the next scheduled event. This removes excessive one-instruction
fallback overhead, but a matched Windows startup was still 12.4% slower than
normal paging. Keep this option off for normal use; see the
[measurements](amiga-dosbox-file-read-optimisation.md).
The opt-in experiment compiles individual register-only integer instructions;
memory operands, stack operations, branches, mode changes and x87 fall back to
the interpreter. Fetches are checked, page-boundary instructions fall back,
paging transitions flush compiled code and paged blocks are not linked.
This is a limited foundation, not a completed general-purpose paged recompiler
or a demonstrated Quake speed improvement.

## Validation

Native sanitizer tests cover CPU identification, 20 MiB XMS allocation and
access above 16 MiB, VCPI page allocation, UMB allocation, gameport behavior,
additional PCM output and UART loopback/transport. A separate real TCP test
covers ring wrap, modem commands and disconnect. Configuration tests cover
profile migration, option validation and runtime selection.

The linked ARM image passes CPU, memory and extra-audio fixtures, relocation
checks, SIMD/FPSCR restoration and the denied-SIMD path. With paged JIT enabled,
two-base tests cover code aliases/remapping, self-modifying code, cross-page
instruction fetch, read/modify/write faults and nested faults. CPL3 instruction
fetch protection is checked with the pinned 386 and 486 U/S rules. The default
interpreter paging path also passes its fault regressions.

On the A4000TX, the candidate passed all three CPU identities, the 32 MiB RAM /
VCPI / UMB fixture, GUS/CMS/Tandy delivery through AHI, COM1 UART/TCP round-trip,
Auto/Max adjustment, x87, EMS and SB16. Every completed run reported restored
registers, clean cache witnesses and release of the owned allocation. The
physical paged-register JIT alias/remapping/SMC fixture also passed, returned
to real mode and exited cleanly. Physical
joystick library setup/teardown passed; moving the stick/buttons still needs
an interactive check. The modem command parser is native-tested; the physical
transport check used raw TCP. VCPI protected-mode client programs need further
compatibility testing beyond the allocation/service fixture.

Auto and Max both reduced the EGA fixture from 6,000 to 2,008 cycles when guest
time fell behind wall time. This verifies adjustment, not increased game speed
or a sustainable 100,000-cycle rate. Display and I/O bottlenecks can still limit
throughput. No new game FPS gain is claimed for this capability expansion.

The previous Compat runtime is retained. Expanded settings are opt-in and the
paged recompiler remains an experimental subset.

The launcher and Expanded runtime are installed on the development A4000TX.
Saving through MUI migrated the library to format 17 with all 137 existing
profiles preserved exactly. CPU, Audio and COM1 pages were checked on
Workbench. Build identifiers and physical test receipts are in the
[validation record](../amiga/records/2026-10-16/dosbox-expanded/README.md).

### Physical serial validation (0.22.0)

Native sanitizer tests passed for asynchronous request cleanup, open/parameter
failures, partial writes, receive backpressure and wrap, configuration changes,
modem status and timed break requests. Existing TCP/modem tests also passed.

On the A4000TX, serial.device unit 0 passed repeated open/reconfigure/close and
reopen checks at 9600 8N1 and 19200 7E1. A DOS UART configuration fixture then
changed the physical port through the ARM/68K bridge and exited cleanly. The
COM1 TCP round-trip regression passed on the same runtime. Both Core1 runs
restored register/cache witnesses and released their owned allocation.
These checks sent no bytes through the physical serial connector. No modem,
USB serial adapter or expansion serial device was connected; their end-to-end
communication, handshaking, sustained throughput and electrical signals remain
unverified. The launcher detected Poseidon and serialpl2303.device; this is
driver detection, not evidence of a connected adapter.
