# ZZDosBox-ARM-68K

DOSBox for AmigaOS and the ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

The project provides a MUI launcher, program profiles, window/fullscreen
switching, editable startup files, DOS 5 or separately installed MS-DOS 6.22,
CD mounting and external MIDI through CAMD. Requires **MUI 3.8+**, Picasso96
and compatible [XACP firmware](https://github.com/Xanxi-Amiga/XACP-ZZ9000).
The new development build adds DOS and PS/2 mouse input, tested with
Oregon Trail and Windows 3.11 on the A9.
PCM sound-card audio and joystick support are pending.

The current Direct runtime adds A9 hardware floating-point event scheduling:
**7.29 → 7.64 FPS in Doom Demo3 (+4.7%)**, averaged over two runs per build.
It retains ARMv7 dynamic recompilation, NEON pixels and a 4 MiB code cache.

[Current DOS executable](artifacts/development/pic-vfp) ·
[Latest performance report](docs/performance-pic-vfp.md)

**Audio:** CAMD v37+ is optional for external MIDI. AHI and ZZ9000AX are
currently detected only; they do not enable PCM sound in this build.

[Features and compatibility](docs/features.md) ·
[Amiga requirements](docs/requirements.md) · [Changelog](CHANGELOG.md)

## Optimisation history

Oldest to newest. Percentages compare each change with its own measured
baseline; they are **not cumulative**. Keen/Vette figures are startup
throughput, while Doom figures are real Demo3 game FPS. Most earlier comparisons
are single runs; the latest VFP result uses two runs per build.

- **12 Oct — Shorter 68k host waits:** replace tick-based waits with a 1 ms timer — **+116.2% Keen / +66.7% Vette** startup throughput. [Results](amiga/records/2026-10-12/dosbox-scheduling/comparison.json)
- **12 Oct — Specialised 1×/2× scalar scaling and row copies:** **+14.4% Keen / +10.0% Vette**. [Results](amiga/records/2026-10-12/dosbox-scaler/README.md)
- **12 Oct — Buffer-copy/PAN overlap prototype:** **+0.1–0.4% observed; no established gain** in the startup tests. [Results](amiga/records/2026-10-12/dosbox-pipeline/README.md)
- **12 Oct — Compile the x86 interpreter with `-O2`:** **+7.7% Keen / +4.2% Vette**. [Results](amiga/records/2026-10-12/dosbox-cpu-o2/README.md)
- **12 Oct — Table-based RGB565 conversion:** **+16.3% Keen / +14.3% Vette**; exact colours preserved. [Results](amiga/records/2026-10-12/dosbox-profile/README.md)
- **12 Oct — Compile PIC/VGA handlers with `-O2`:** **+2.3% Keen / +6.2% Vette**. [Results](amiga/records/2026-10-12/dosbox-events/README.md)
- **13 Oct — Division-free generic scaling and BIOS disk read-ahead:** **hardware gain not isolated**; retained as research, not a measured speed claim. [Evidence](amiga/records/2026-10-13/dosbox-optimisation/artifacts.json)
- **13 Oct — ARMv7 dynamic recompilation:** **+124% Doom**, 3.10 → 6.94 FPS, replacing repeated x86 interpretation with cached ARM code. [Results](amiga/records/2026-10-13/dosbox-dynrec/method.md)
- **13 Oct — Enable bounded buffer-copy overlap with dynrec:** **+0.24% observed; no established gain**, 6.944 → 6.961 FPS. [Results](amiga/records/2026-10-13/dosbox-tuning/results.json)
- **13 Oct — Increase the translated-code cache from 1 to 4 MiB:** **+0.10% observed; no established gain**, 6.961 → 6.967 FPS. [Results](amiga/records/2026-10-13/dosbox-tuning/results.json)
- **13 Oct — NEON pixel processing:** **+4.6% Doom**, 6.97 → 7.29 FPS versus the same 4 MiB/overlap build. [Results](amiga/records/2026-10-13/dosbox-tuning/method.md)
- **13 Oct — Skip writing masked scanline rows:** **+3.7% Doom**, 7.33 → 7.60 FPS; optional visual trade-off, not a general rendering gain. [Results](amiga/records/2026-10-13/dosbox-scanlines/method.md)
- **13 Oct — Adaptive presentation:** **+13.5% Doom**, 7.27 → 8.25 FPS; optional, achieved by presenting fewer frames. Normal remains the default. [Results](amiga/records/2026-10-13/dosbox-presentation/results.json)
- **14 Oct — Hardware floating point for PIC event scheduling:** **+4.7% Doom**, 7.29 → 7.64 FPS averaged across two runs per build, with no skipped frames. [Report](docs/performance-pic-vfp.md)

These are application/host improvements on the existing XACP execution model;
none required a firmware change. Future firmware work should preserve the
Core1, framebuffer-handoff and processor-state restoration contracts.

Potential next reference: [Simd ARM32 NEON review](docs/simd-review.md). No new gain claimed.

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
