# ZZDosBox-ARM-68K

DOSBox for AmigaOS and the ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

**0.10.1-dev** provides a MUI launcher, program profiles, window/fullscreen
switching, editable startup files, DOS 5 or separately installed MS-DOS 6.22,
CD mounting and external MIDI through CAMD. Requires **MUI 3.8+**, Picasso96
and compatible [XACP firmware](https://github.com/Xanxi-Amiga/XACP-ZZ9000).
PCM sound-card audio, mouse and joystick support are pending.

The current Windows experiment fixes A20 handling and protected-mode page
faults. Windows Setup and Program Manager worked in native host tests;
WinBench reached its licence screen. The initial Amiga run stopped with an
internal allocation failure. A smaller TLB allocation is built; physical
retesting and completed WinBench results remain pending.

[0.10.1 artifacts](artifacts/0.10.1-dev) ·
[Windows experimental build](artifacts/experiments/wfw311-small-tlb) ·
[Build and restore](docs/build-and-restore.md) ·
[Windows findings](amiga/records/2026-10-13/dosbox-wfw/findings.md)

Credits: DOSBox team and libretro contributors; Xani's XACP platform;
MUI by Stefan Stuntz. Development: Codex assisted, Amiga-Bridge MCP.

Games, Microsoft software, WinBench and guest disk images are not included.
