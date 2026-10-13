# ZZDosBox-ARM-68K

DOSBox for AmigaOS and the ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

**0.10.1-dev** provides a MUI launcher, program profiles, window/fullscreen
switching, editable startup files, DOS 5 or separately installed MS-DOS 6.22,
CD mounting and external MIDI through CAMD. Requires **MUI 3.8+**, Picasso96
and compatible [XACP firmware](https://github.com/Xanxi-Amiga/XACP-ZZ9000).
PCM sound-card audio, mouse and joystick support are pending.

![ZZDosBox DOS welcome screen](docs/screenshots/dos-welcome.png)

**Windows for Workgroups 3.11 now boots on the real ZZ9000 A9.** WinBench 3.11
completed Graphics WinMark at **1,923,752 pixels/sec** (640x480, 16 colours,
8 MB guest RAM, 3000 cycles/ms). Windows exited cleanly back to DOS and Workbench.

WinBench's score and “37 MHz 80386” label use the emulated PC clock;
they do not establish equivalent real-world PC speed.

![WinBench running on the ZZ9000](docs/screenshots/windows311/winbench-results.png)

[Windows desktop and test screenshots](docs/screenshots/windows311/README.md)

[0.10.1 artifacts](artifacts/0.10.1-dev) ·
[Windows experimental build](artifacts/experiments/wfw311-small-tlb) ·
[Build and restore](docs/build-and-restore.md) ·
[Windows findings](amiga/records/2026-10-13/dosbox-wfw/findings.md)

Credits: DOSBox team and libretro contributors; Xani's XACP platform;
MUI by Stefan Stuntz. Development: Codex assisted, Amiga-Bridge MCP.

Games, Microsoft software, WinBench and guest disk images are not included.
