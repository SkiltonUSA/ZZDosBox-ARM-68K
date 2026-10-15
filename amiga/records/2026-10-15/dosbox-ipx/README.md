# IPX wrapper acceptance

Launcher 0.18.0-dev; physical A4000TX/ZZ9000, XX19c, Roadshow/X-Surf100.
The relay uses the unmodified pinned dosbox-libretro IPX server with a small
host adapter and an independent registered test peer.

| Test | Result |
| --- | --- |
| Physical guest discovery, sockets, broadcast and exact ESR callback reply | Pass; 3 TX / 3 RX including registration and peer ping |
| MUI network terminal and Disconnect | Pass; cooperative cancellation, 1 TX / 1 RX |
| MUI peer ping | Pass; 2 TX / 2 RX |
| Protocol switching and save/readback | Both settings retained; byte-exact test profile |
| Existing installed profiles | 21; version-13 migration byte-exact |
| ARM model IPX API and upstream relay | Pass |
| ARM model cancellation during connection | Pass |
| ARM model MSCDEX CD regression | Pass |
| ARM model TCP/IP DHCP/custom subnet/HTTP | Pass; 4,099 exact bytes |
| Native config, packet, queue, IPC and launcher sanitizers | Pass |
| Actual multiplayer game | Not tested |

All three recorded physical runs report zero dropped packets/errors, restored
SIMD/FPSCR and MMU/register/cache witnesses, and released owned memory.
SHELL result=2 in the MUI Disconnect log means cooperative cancellation.
Installed executable identities are in results.json. Normal DOS/Windows
runtimes and dosbox.conf remain unchanged. Previous launcher, network runtime
and settings are preserved with .pre018 suffixes.

No native LAN IPX/SPX, relay hosting or Windows IPX driver support is claimed.
An external DOSBox-compatible IPXNET relay is required. IPX and TCP/IP are
alternative per-program modes in the separate interpreter network runtime.
