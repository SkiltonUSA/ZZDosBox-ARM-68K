# Network tab acceptance

Launcher 0.17.0-dev, physical A4000TX/ZZ9000, Roadshow/X-Surf100.

| Test | Result |
| --- | --- |
| DHCP acquisition | 10.0.2.15; clean exit |
| DNS through LAN resolver | example.com resolved |
| DHCP, MTU 1200, ping | 4/4 replies |
| Manual 10.0.2.42, MTU 1200 | 4/4 replies; DHCP not run |
| MUI Disconnect at DOS prompt | Cooperative cancellation and full restoration |
| Release saved lease | Current MTCP.CFG removed, profile disabled and saved |
| Normal Launch button | Automatic DHCP, program ping 1/1, normal exit |
| Existing installed profiles | 21 migrated/roundtripped; networking off |
| Configuration/IPC native sanitizer tests | Passed |
| ARM model DHCP/manual on custom subnet | Exact 4,099-byte HTTP transfers |

Every recorded physical session has zero reported drops/errors and verified
SIMD/FPSCR, MMU/register/cache-witness restoration and allocation release.
SHELL result=2 in the Disconnect run is cooperative cancellation, not a crash.
Guest timestamps are emulated DOS time, not wall-clock dates.

The experimental network runtime is separate. Existing DOS and Windows
runtimes were unchanged. See the UI guide for supported core/renderer modes
and the distinction between saved-lease release and DHCPRELEASE. No Windows
networking, IPX or sustained throughput result is claimed here.
