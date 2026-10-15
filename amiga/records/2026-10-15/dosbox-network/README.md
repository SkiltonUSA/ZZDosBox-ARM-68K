# Networking evidence — 15 October 2026

The final corrected prototype passed physical DOS/ARM NE2000, DHCP and an
exact 4,099-byte HTTP transfer through Roadshow. Exit/state restoration and
owned-memory release passed; bridge and Workbench remained responsive.

An earlier full guest run faulted in the 68k host. A compiler stack audit found
an Ethernet function exceeded the entire 64 KiB task stack. The bounded-buffer
fix reduced that frame to 1,596 bytes; a regression compile verifies the new
8 KiB frame gate rejects the original code. User restarted before the final run.

Normal installed emulator/launcher were not replaced. DNS, Windows and IPX
are not claimed by these tests. See [report](../../../../docs/amiga-dosbox-network.md).
