# Windows experiment — 13 October 2026

Fast A20, ROM write protection, full-core page-fault recovery and smaller sparse
TLB banks. 8 MiB guest RAM / 32 MiB owned allocation.

The full ARM model regression passed. On the physical A4000TX, Windows reached
Program Manager and completed WinBench 3.11 Graphics WinMark: 1,923,752 pixels/sec.
Windows returned to DOS; the emulator then closed with registers/cache restored
and owned memory released. This remains a development build with keyboard-only
Windows input. The tested DOS runtime remains separately available.

[Results and limitations](../../../amiga/records/2026-10-13/dosbox-wfw/findings.md).
No Windows, DOS, WinBench or guest hard-disk images are included.
