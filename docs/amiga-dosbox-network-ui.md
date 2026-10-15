# ZZDosBox Network tab

The **Network** tab configures the selected program's emulated NE2000 adapter.
Networking is experimental and off by default. It shares the Amiga's existing
Roadshow connection through IPv4 NAT. It does not reconfigure X-Surf100,
ZZ9000 Ethernet, Roadie, or the Amiga's own IP address.

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
migrate with networking disabled. `Launcher.cfg` is now version 12; preserve
the pre-upgrade profile backup when reverting to an older launcher.

## Controls

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

The network runtime is an interpreter build. It supports embedded DOS with
Direct/Normal presentation, without scanlines, OpenGL, trace/breakpoint or
memory-monitor runtimes. Explicit dynamic-core selection and unsupported
combinations are rejected, rather than silently downgraded. A default game
with networking disabled continues to use its existing runtime.

Windows/MS-DOS disk-image networking still needs guest-specific driver and
TCP/IP configuration and is not enabled by this tab. IPX, IPv6, PCAP bridging,
port forwarding, incoming LAN discovery and a persistent background guest
TCP/IP service are not provided. libslirp allocator/fatal-error recovery
remains a production-hardening item.

The launcher uses its own marked `ZZNET` subdirectory in the selected C:
drawer. It installs NE2000/mTCP utilities and regenerates `MTCP.CFG` before
each network session. It refuses to take over an unmarked existing directory.
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

## Physical acceptance — 15 October 2026

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
from RAM; the installed launcher is **0.17.0-dev**. Previous launcher and
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
