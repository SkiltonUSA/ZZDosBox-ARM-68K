# Descent startup update — 0.22.2-dev

Tested development update for an existing ZZDosBox installation.

| File | Change |
| --- | --- |
| ZZDosBox-Compat | Fix stale DOS errors during temporary-file creation |
| ZZDosBox-Expanded | Same filesystem fix, plus speculative invalid-opcode handling in the ARM recompiler |
| DOS32A.EXE | DOS/32A 7.35 external loader; avoids Descent's DOS/4GW paging path |

Close the launcher and player, retain backup copies, and replace the two runtime
executables in the ZZDosBox application drawer. Keep the existing launcher and
configuration. Other runtime variants are unchanged.

For Descent, copy `DOS32A.EXE` and `D32-LIC.TXT` into the game's directory.
Set the launch command to `DOS32A DESCENT.EXE`, or `DOS32A DCNTSHR.EXE`
for the shareware executable. Select 16 MiB, EMS, 386 and 6,000 cycles;
leave LOADFIX, VCPI, UMB and experimental paged JIT off. For sound, match
SB Pro 2 in the launcher and game setup: port 220h, IRQ 7, DMA 1.
The original game executable does not need modification.

Physical checks reached the first mine, responded to movement and exited
cleanly. Sound-enabled delivery completed without reported errors; it was not
a listening test. No new game-FPS benchmark or complete-game claim is made.

[Fix explanation and tests](../../../docs/amiga-dosbox-shooter-startup-review.md) ·
[Recorded hardware results](../../../amiga/records/2026-10-18/dosbox-descent/)

`builds/` retains the ARM ELF, metadata and generated integration patches.
`sha256.json` covers this backup. This is a development update, not a new
versioned release. No game data, personal profiles or guest OS media are included.

This product uses DOS/32 Advanced DOS Extender technology.
DOS/32A 7.35 is Copyright (C) 1996–2005 Supernar Systems, Ltd.; its complete
licence is in `D32-LIC.TXT`. [Original binary distribution](https://downloads.sourceforge.net/project/dos32a/DOS_32A%20v7.35/DOS_32A%20DOS%20Extender/dos32a-735-bin.zip).
ZZDosBox component licences remain in the repository's `licenses/` directory.
