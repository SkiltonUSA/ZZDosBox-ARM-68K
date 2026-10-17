# I/O and joystick

Settings apply to the selected program. COM1 and the gameport use the
**Expanded runtime** and embedded DOS. They currently cannot be combined
with NE2000/IPX networking or booted MS-DOS/Windows.

## COM1

| Setting | Specification |
| --- | --- |
| Guest port | One 16550-style UART, COM1 at 3F8h, IRQ 4; FIFO and guest baud timing |
| Disabled | Default; no host connection |
| Raw TCP | Outbound connection to the configured numeric IPv4 address and TCP port |
| Basic modem | Outbound TCP modem command subset; uses the configured endpoint |
| Physical serial — experimental | Built-in, USB or expansion IOExtSer-compatible AmigaOS device |
| Physical device | Device name and unit 0–255; Built-in selects serial.device unit 0 |
| Baud | 0 follows DOS UART settings; a fixed value from 50–115200 overrides them, subject to driver/hardware support |
| Framing | 7/8 data bits, None/Odd/Even/Mark/Space parity, 1/2 stop bits; driver restrictions apply |
| Flow control | None or driver-managed 7-wire hardware handshake |
| Buffering | Bounded 4 KiB transmit and receive queues; asynchronous physical I/O |

Choose **I/O → Connection**. TCP modes require Roadshow and
`bsdsocket.library`; physical mode requires the selected Amiga serial driver.
Physical mode forwards bytes and modem AT commands directly to the attached
device. Driver errors appear in the runtime log.

**USB (Experimental)** is enabled when Poseidon and a supported registered
serial driver are detected. **Detect USB** refreshes that check. Detection
confirms driver availability, not a connected or working adapter. Other
IOExtSer-compatible device names can be entered manually. Drivers are not bundled.

CTS, DSR, carrier and ring status reach the guest. RTS/DTR outputs remain
controlled by the Amiga driver; arbitrary pin control and bit-banging are not
implemented. Guest break requests produce a timed 250 ms break. Built-in
serial cannot share the port with CAMD MIDI in the same session.

TCP modem mode supports AT, ATV1, ATZ, AT&F, ATE0/1, ATH/ATH0 and ATD/ATDT/ATDP
without a number. It does not implement incoming calls, telephone-number
routing, Telnet negotiation or the +++ escape sequence.

## Joystick

| Setting | Specification |
| --- | --- |
| Physical input | Amiga joystick port 2 through lowlevel.library V40 |
| Guest device | One two-axis, two-button DOS gameport at I/O 201h |
| Controller choice | 1–6 physical buttons, saved per program |
| Buttons 1–2 | Native DOS gameport buttons; one-button mode ignores button 2 |
| Buttons 3–6 | Optional DOS key bindings: Space, Enter, Ctrl, Alt, Z/X/C/V, Shift, Tab or Escape |
| Tester | Live direction/button indicators and X/Y values; active only on the Joystick page |

Digital directions map to full axis travel. Buttons 3–6 require a
lowlevel-compatible gamepad or adapter; selecting six buttons does not add
support for an incompatible electrical protocol. Extra buttons emulate keys,
not a native six-button PC gameport. Keyboard and gamepad holds are combined
so releasing one input does not release a key still held by the other.

## Validation

TCP round-trip and physical serial open/reconfigure/close tests passed on the
A4000TX. Physical settings were checked at 9600 8N1 and 19200 7E1. No external
serial device was attached: modem communication, USB/expansion transfers,
handshaking and sustained physical throughput remain untested.

Gameport emulation, button bindings and input-source merging have automated
tests. The physical library lifecycle and MUI tester were checked; movement
and all button mappings still need an interactive controller check.

[Expanded hardware and detailed test scope](amiga-dosbox-expanded-hardware.md) ·
[Network settings](amiga-dosbox-network-setup.md)
