# Windows experiment — 13 October 2026

Current source build: fast A20, ROM write protection, full-core page-fault
recovery and smaller sparse TLB banks. 8 MiB guest RAM / 32 MiB owned allocation.
Physical retest is pending. Do not replace the tested runtime automatically.
Earlier model logs precede the smaller-bank change; their scope is historical.
The supplied paging fixture covers sparse high-address banks for the next run.

No Windows, DOS, WinBench or guest hard-disk images are included.
