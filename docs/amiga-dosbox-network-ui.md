# ZZDosBox Network settings

For installation and everyday use, click **Network guide** at the bottom of
the Network page (launcher 0.19.8-dev). It opens the bundled `Network.guide`
in MultiView without requiring a browser or an active network connection.
The [web version](amiga-dosbox-network-setup.md) uses the same source.

The **Network** sidebar section configures the selected program's TCP/IP or IPX wrapper.
Networking is experimental and off by default. TCP/IP uses NE2000/IPv4 NAT;
IPX uses DOSBox-compatible UDP tunnelling over the existing Roadshow route. It does not reconfigure X-Surf100,
ZZ9000 Ethernet, Roadie, or the Amiga's own IP address.

## Protocol

Choose **TCP/IP** or **IPX wrapper (UDP)**. Settings are saved per program.
IPX uses the DOSBox guest IPX API and a UDP socket on the Amiga; it does not
run an NE2000 driver, mTCP or DHCP. Set the relay server’s numeric IPv4 address
and UDP port (default 213). TCP/IP fields and DHCP/DNS buttons are disabled
when IPX is selected; their saved values are retained.

Start a compatible relay on another computer with `IPXNET STARTSERVER 19900`,
then select that computer’s address and port 19900 in MUI. **Open network
terminal** connects to it. Normal Launch also connects before running the game.
The server can be any compatible host; a Mac is not specifically required.

The embedded shell offers `IPXNET CONNECT`, `STATUS`, `PING` and `DISCONNECT`.
CONNECT uses the MUI endpoint. Ping requests replies from other clients; a
relay with no other clients may produce no replies despite a working connection.
MUI **Disconnect** closes the DOS session cooperatively; shell DISCONNECT
leaves DOS running. Neither command changes Roadshow.

This is a client implementation, not a native LAN IPX/SPX stack. Relay hosting,
Windows IPX drivers, hostname resolution for relay addresses and simultaneous
TCP/IP plus IPX modes are not provided. Per-game multiplayer compatibility
requires game-specific testing.

[Protocol reference](https://www.dosbox.com/wiki/Connectivity).

## TCP/IP

| Setting | Function |
| --- | --- |
| Enable for this program | Select the separate network runtime for this program |
| DHCP / Manual IPv4 | Obtain a fresh guest lease at startup, or generate static mTCP settings |
| Manual IP | Static guest address, used only in Manual mode |
| Subnet mask | Private virtual network; contiguous /24 or larger network within RFC1918 space |
| NAT gateway | Address of the virtual gateway, not the physical LAN router |
| Upstream DNS | Numeric address of the LAN/public resolver; blank disables DNS forwarding |
| Hostname | Guest hostname, 1–31 letters, digits or internal hyphens |
| MTU | 576–1500 bytes; applies to NAT and the DOS client |
| DHCP retries | 1–10 attempts |
| Timeout | 5–30 seconds per DHCP attempt |

Defaults: `10.0.2.0/24`, gateway `10.0.2.2`, DHCP address `10.0.2.15`.
The guest DNS proxy uses subnet address +3; the DHCP pool starts at +15.
Addresses reserved for these services cannot be used as the gateway. Choose
a virtual subnet that does not overlap the physical LAN. The backend accepts
one upstream DNS resolver; secondary DNS and automatic Roadshow resolver
discovery are not implemented.

**Save settings** stores these options per program. Existing launcher profiles
migrate with networking disabled. `Launcher.cfg` is version 19 (earlier versions remain readable); preserve
the pre-upgrade profile backup when reverting to an older launcher.

## TCP/IP controls

- **Open network terminal** starts an embedded DOS session with the packet
  driver and mTCP configuration installed. The launcher stays responsive.
- **Renew DHCP** starts a short DOS session and obtains a fresh lease. This
  uses mTCP's acquisition process, not an in-place renewal of a running game.
- **Release saved lease** removes the managed mTCP configuration and disables
  networking for the selected program. It is available only while idle and in
  DHCP mode. The NAT server already disappears when the guest closes; this
  action does **not** transmit a DHCPRELEASE or release Roadshow's address.
- **Refresh** shows configured settings and the last saved guest lease while
  idle. During a session it requests live packet/error/drop counters.
- **Disconnect** cooperatively stops the network DOS session and lets the
  normal ARM/display/cache/memory teardown finish. It does not kill a task.
- **Ping** sends four guest ICMP probes. **DNS lookup** runs the guest DNS
  client. Both use the selected profile's network settings.
- **Open log** shows the last network diagnostic session's output. Ordinary
  program launches retain their normal run log.

Saved lease details are explicitly historical. A present socket library is
not treated as proof of a working route. The emulated DOS clock may differ
from the Amiga clock; lease timestamps are guest timestamps.

## Current scope

Launcher 0.22.1-dev selects `ZZDosBox-Net-JIT` for core auto/dynamic. It
combines networking with ARM JIT, EMS/XMS, SBPro2/SB16 and PC speaker. Explicit
core normal selects the existing `ZZDosBox-Net` interpreter and rejects EMS
or PCM audio. Networking alone does not enable either EMS or audio.

Both use embedded DOS, up to 8 MiB guest RAM, fixed cycles up to 10,000 and
Direct/Normal presentation. Scanlines, OpenGL, developer tools and Expanded
options remain separate. Unsupported combinations are rejected explicitly.
Network terminal/diagnostic sessions use the selected core and sound settings.
Programs with networking disabled retain their existing runtime.

Windows/MS-DOS disk-image networking still needs guest-specific driver and
TCP/IP configuration and is not enabled by this tab. IPv6, PCAP bridging,
port forwarding, incoming LAN discovery and a persistent background guest
TCP/IP service are not provided. libslirp allocator/fatal-error recovery
remains a production-hardening item.

The launcher uses its own marked `ZZNET` subdirectory in the selected C:
drawer. TCP/IP installs NE2000/mTCP utilities and regenerates `MTCP.CFG` before
each network session. IPX generates only its connection batch, without those utilities. It refuses to take over an unmarked existing directory.
Temporary network settings and session files remain in the launcher drawer.
Neither host startup files nor user AUTOEXEC/CONFIG.SYS files are rewritten.

## Implementation and validation

`network/config.h` is shared by the UI and NAT backend. It validates addresses,
masks, service conflicts, DNS, hostname, MTU and DHCP limits before writing DOS
commands or opening sockets. The backend accepts `--network-config <file>`.
The original `--network` / `--dns` development switches still work.

`ZZDOS_NETWORK` is a host-only Exec message port for bounded status/stop
requests. It never passes pointers to ARM, never changes the shared queue
allocation and replies to queued messages before closing. The MUI task keeps
request storage alive until it receives its reply. The usual single-Core1
ownership and restoration rules still apply.

Native sanitizer checks cover configuration, old-profile migration, generated
DHCP/static files, malformed control messages, bounded IPC polling, stop and
close/reopen. ARM instruction-model tests cover DHCP and static addressing on
`192.168.77.0/24` with an exact 4,099-byte HTTP transfer. Physical UI checks are
recorded separately; these model results do not establish Amiga execution.

Reference: [DOSBox-X networking guide](https://github.com/joncampbell123/dosbox-x/wiki/Guide:Setting-up-networking-in-DOSBox%E2%80%90X).
We adopt its distinction between guest NE2000 configuration and SLIRP NAT;
the guide's PCAP, configurable NIC resources and forwarding options are not
automatically available in ZZDosBox. The hardware adapter remains I/O `0x300`,
IRQ 3, MAC `02:5A:5A:00:00:01`.

## TCP/IP physical acceptance — 15 October 2026

- MUI DHCP acquisition: `10.0.2.15`, 2 TX / 2 RX, normal exit.
- MUI DNS lookup through the LAN resolver: successful, 5 TX / 4 RX.
- DHCP with MTU 1200: four ping replies, zero loss.
- Manual `10.0.2.42`, MTU 1200: four ping replies, zero loss, no DHCP client run.
- Normal Launch button: automatic DHCP and a successful guest ping.
- Open network terminal, then MUI Disconnect: cooperative cancellation,
  normal host exit and released ARM allocation.
- Release saved lease: generated configuration removed and profile disabled;
  saved version-12 profile verified.
- All completed hardware sessions restored SIMD/FPSCR, MMU/register/cache
  witnesses and memory, with zero reported network drops/errors.
- Installed launcher and utilities verified by CRC32. Existing DOS/Windows
  runtimes remained byte-identical. All 21 previous profiles passed a native
  migration/roundtrip check with networking disabled.

Evidence: `amiga/records/2026-10-15/dosbox-network-ui/`. Initial UI tests ran
from RAM; that test installed launcher **0.17.0-dev**. Previous launcher and
settings are preserved with `.pre017` suffixes. The network build remains
experimental despite these successful bounded checks.

## Building the optional utilities

Use the pinned official mTCP ZIP and DOSBox-X packet-driver source locally:

```sh
python3 scripts/prepare_dosbox_network_tools.py \
  --mtcp /path/to/mtcp.zip \
  --ne2000-source /path/to/dosbox-x/src/builtin/ne2000bin.cpp
python3 scripts/build_dosbox_launcher.py
python3 scripts/test_dosbox_network.py
```

The staging script verifies both input hashes, retains the mTCP license and
source links, and writes the seven files required in `Network/` beside the
launcher. The NE2000/mTCP binaries are optional guest utilities; they are not
system-wide Amiga libraries. Roadshow must already be running. MUI 3.8 or
later and the normal ZZDosBox/ZZ9000 requirements still apply.

## IPX implementation and test scope

The pinned dosbox-libretro IPX driver runs inside the ARM guest emulator. A
small source-level UDP adapter replaces SDL_net; there is no SDL runtime
requirement. The 68K owns one connected, nonblocking Roadshow UDP socket.
The existing bounded queues carry packets of 30–1,424 bytes, with validated
IPX lengths. Packets from another UDP endpoint are filtered by the socket.
The INT 2F multiplex chain accommodates both MSCDEX and IPX.

Native tests cover malformed/truncated/oversized datagrams, exact byte transfer,
peer filtering and repeated socket close/reopen. The ARM test uses an original
DOS program and the **unmodified pinned upstream DOSBox relay**, with a second
independent test peer. Model tests do not establish physical Amiga execution or
full game compatibility. The linked ARM test passed discovery, registration,
socket open/close, a broadcast with an exact reply delivered through a guest ESR callback, peer
ping and disconnect (3 packets each direction). Cancellation during connection
and the MSCDEX CD regression also passed. The TCP/IP regression acquired a
custom-subnet DHCP lease and transferred 4,099 exact bytes through HTTP.

```sh
python3 scripts/test_dosbox_network.py
python3 scripts/test_dosbox_developer.py
python3 scripts/build_dosbox_network.py --native --model-library
python3 scripts/build_dosbox_ipx_relay_test.py
# Use your Mac's local IPv4 address, and a Python environment with Unicorn:
python3 tests/amiga/dosbox/network/ipx_backend_test.py --bind <local-ip>
python3 tests/amiga/dosbox/network/ipx_model_test.py --bind <local-ip> --upstream
```

## IPX physical acceptance — 15 October 2026

On the A4000TX, the original DOS test program passed IPX discovery, socket
creation, broadcast, an exact reply delivered through the guest ESR callback,
peer ping and disconnect through the unmodified upstream DOSBox relay.
Three packets were sent and received, with zero network drops or errors.

The MUI network terminal connected automatically; MUI Disconnect returned
cleanly. The Diagnostics Ping action received a peer reply (2 TX / 2 RX).
Switching TCP/IP → IPX retained both protocols' settings; saving and reading
back the isolated profile produced identical bytes. Completed physical runs
restored SIMD/FPSCR, MMU/register/cache witnesses and released owned memory.

These tests establish the IPX transport and guest API on physical hardware.
A multiplayer game session against another DOSBox player remains untested.
Evidence: `amiga/records/2026-10-15/dosbox-ipx/`.
