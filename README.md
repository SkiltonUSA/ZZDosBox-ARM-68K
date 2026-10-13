# ZZDosBox-ARM-68K

DOSBox for AmigaOS and the ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

**0.10.1-dev** provides a MUI launcher, program profiles, window/fullscreen
switching, editable startup files, DOS 5 or separately installed MS-DOS 6.22,
CD mounting and external MIDI through CAMD. Requires **MUI 3.8+**, Picasso96
and compatible [XACP firmware](https://github.com/Xanxi-Amiga/XACP-ZZ9000).
The new development build adds DOS and PS/2 mouse input, tested with
Oregon Trail and Windows 3.11 on the A9.
PCM sound-card audio and joystick support are pending.

The latest DOS development runtime adds ARMv7 dynamic recompilation, NEON
pixel conversion and a 4 MiB code cache. Doom Demo3 improved from **3.10 to
7.29 real FPS** on the tested ZZ9000. A separate scanline experiment measured
**7.33 FPS normally / 7.60 FPS skipping black rows** (+3.7%, single runs).

[Current DOS executable](artifacts/development/neon-dynrec) ·
[Performance results](amiga/records/2026-10-13/dosbox-tuning/method.md) ·
[Scanline experiment](artifacts/experiments/scanline-copy)

**Audio:** CAMD v37+ is optional for external MIDI. AHI and ZZ9000AX are
currently detected only; they do not enable PCM sound in this build.

[Features and compatibility](docs/features.md) ·
[Amiga requirements](docs/requirements.md) · [Changelog](CHANGELOG.md)

![ZZDosBox DOS welcome screen](docs/screenshots/dos-welcome.png)

**Windows for Workgroups 3.11 now boots on the real ZZ9000 A9.** WinBench 3.11
completed Graphics WinMark at **1,923,752 pixels/sec** (640x480, 16 colours,
8 MB guest RAM, 3000 cycles/ms). Windows exited cleanly back to DOS and Workbench.

WinBench's score and “37 MHz 80386” label use the emulated PC clock;
they do not establish equivalent real-world PC speed.

![WinBench running on the ZZ9000](docs/screenshots/windows311/winbench-results.png)

[Windows desktop and test screenshots](docs/screenshots/windows311/README.md)

[Earlier mouse build](artifacts/development/mouse) ·
[Mouse test results](amiga/records/2026-10-13/dosbox-mouse/findings.md) ·
[0.10.1 recovery artifacts](artifacts/0.10.1-dev) ·
[Windows experimental build](artifacts/experiments/wfw311-small-tlb) ·
[Build and restore](docs/build-and-restore.md) ·
[Windows findings](amiga/records/2026-10-13/dosbox-wfw/findings.md)

Credits: DOSBox team and libretro contributors; Xani's XACP platform;
MUI by Stefan Stuntz. Development: Codex assisted, Amiga-Bridge MCP.

Games, Microsoft software, WinBench and guest disk images are not included.
