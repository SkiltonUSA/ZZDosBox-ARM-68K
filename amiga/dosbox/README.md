# ZZDosBox — development prototype

DOSBox for the Amiga and ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

Launcher **0.19.3-dev** provides icon sidebar navigation, clearer typography and
aligned settings panels, with separate **CPU & memory**, **Display** and **Audio** pages. A brief startup splash displays the ZZDosBox logo and version.
[Navigation guide](../../docs/amiga-dosbox-launcher-navigation.md).

The experimental **Network** section provides: DHCP/manual IPv4,
DNS, MTU, renew, saved-lease release, ping, DNS lookup, live counters and
Disconnect. It shares Roadshow through a separate NE2000/NAT interpreter
runtime. IPX wrapper mode adds a DOSBox-compatible UDP client with server
IPv4/port settings. Windows networking and IPX relay hosting remain pending.
[Network settings and requirements](../../docs/amiga-dosbox-network-ui.md).

The offline **Compatibility** section provides title search,
A–Z/0–9 browsing, per-game publisher/tester details, version history, comment
previews and original report links. Games appear on the left, with a wider
details pane on the right. Game metadata uses labelled rows; version, status
and rating appear in a three-column table, with discussion overviews below.
The overviews index all 6,999 cached comments, showing contributors, dates and
automatic topic counts. Mentions do not establish successful fixes; full text
remains available through the original report link. The [Markdown reference](../../docs/amiga-dosbox-compatibility.md)
covers 3,932 games; the [per-game details](../../docs/amiga-dosbox-game-details.md)
include 8,709 version-specific ratings. These are upstream reports, not verified
ZZDosBox compatibility or speed measurements.

Keep `Compatibility.dat` beside the launcher. Reports now load on demand,
reducing the compatibility browser's contribution to the executable size.

**0.11.0-dev** includes a MUI launcher, program profiles, optional LOADFIX,
selectable/editable AUTOEXEC.BAT and CONFIG.SYS, terminal colours, 1–8 MB guest RAM and VGA/EGA/CGA/S3-VESA/Tandy/PCjr/Hercules graphics.
Choose a Workbench window or fullscreen, embedded DOS 5 or installed MS-DOS
6.22 on the DOS terminal tab, with an optional ARM-rendered FPS counter in Settings.
Window sizes are 320×240, 640×480 and 960×720; the Workbench resize gadget
adjusts the window between these limits, keeping the picture at 4:3.
A coloured DOS welcome panel shows the version, project link and controls.
Alt+Return switches window/fullscreen; Ctrl+minus/plus adjusts CPU cycles.
The DOS terminal also accepts ISO images or a mounted Amiga data CD as D:.
Settings offers **Normal / Adaptive** presentation per program, with Normal as
the default. Adaptive reduces frame-copy work without changing guest CPU speed.
S3 provides 1 MB of emulated video RAM, with modes up to 1024×768 at
256 colours or 800×600 at 16-bit colour, scaled to the existing display.
Windows S3 drivers and high-resolution game performance need further testing.
Hercules offers White, Amber, Green and Paperwhite in Settings.
Settings provides **Direct / OpenGL (experimental)**, saved per program.
Direct is the native RTG renderer and remains the default. The experimental
TinyGL renderer runs on the A9 and supports a partial fixed-function OpenGL API, without GLSL
shaders or guest OpenGL acceleration. A matched Doom test measured 7.00 FPS
Direct versus 4.72 FPS OpenGL; keep Direct for speed.
Built-in ZZDosBox ARM software scanlines offer Off, Classic, Soft and Gradient
in fullscreen with the FPS overlay off. **Scanlines and Show FPS cannot be used
together**: turn Show FPS off to enable the scanline selector. No external
scanline utility is required.
Settings includes external MT-32/General MIDI through CAMD and a CAMD/AHI/
ZZ9000AX driver check. MIDI needs CAMD and a connected external synthesizer.

The local **0.11.1-dev** launcher adds a Developer tab: trace logging, CPU/video
profiling, cooperative frame breakpoints, display handoff and host polling.
It warns that diagnostics affect performance; options reset when the launcher
closes. Trace/checkpoint controls require the matching optional Trace runtime.
Direct/OpenGL checkpoint and lifecycle checks passed on the A4000TX.
This development build is staged separately; it is not installed or released.

Requires **MUI 3.8 or later** and XACP on the ZZ9000; tested with XX19c firmware.
AHI and ZZ9000AX are currently detected only; PCM output is not implemented.

[Features and compatibility](../../docs/amiga-dosbox-features.md) ·
[Amiga requirements](../../docs/amiga-dosbox-requirements.md) · [Changelog](CHANGELOG.md)

The A9 runs the x86 dynamic recompiler and draws directly into the ZZ9000 framebuffer;
the Amiga handles files, input and display coordination. A writable Amiga
drawer serves as DOS drive C:, including subdirectories. Read-only 1.44 MB
floppy boot and a writable 32 MB BIOS hard disk are also available.
The Disks tab mounts the hard disk in Workbench for file transfers; eject it
before booting DOS. Games and MS-DOS disks are not included.

Hard-disk boot, file persistence and Workbench file access have been tested
on the A4000TX.

The 0.8 baseline averaged **37.2 FPS** across three timed Keen startup tests
on the A4000TX. Eight emulated seconds took 15 real seconds: this is not full
speed or a steady gameplay measurement. Keen map movement, Vette startup,
Elite launch and return to Workbench have been checked.

Current limits: restricted DOS 8.3 names; no PCM sound-card audio or joystick. The new mouse build supports DOS INT33 and BIOS PS/2 input.
The Cortex-A9 and this port are 32-bit; 64-bit x86 software is not supported. Embedded DOS has no XMS/EMS driver; CONFIG.SYS is an
editor/export reference there. Booted MS-DOS runs the configuration and
drivers on its disk and can use its own HIMEM driver. `DOS=HIGH` and EMM386
are not supported in the supplied configuration.

Credits: DOSBox team, libretro contributors, MUI by Stefan Stuntz, and Xanxi's
[XACP](https://github.com/Xanxi-Amiga/XACP-ZZ9000) platform.

Build and test notes are in [the development record](../../docs/amiga-dosbox-feasibility.md).

## Optimisation history

Oldest to newest. Percentages compare each change with its own measured
baseline; they are **not cumulative**. Keen/Vette figures are startup
throughput, while Doom figures are real Demo3 game FPS. Most earlier comparisons
are single runs; the latest VFP result uses two runs per build.

- **Direct — ARM-to-ZZ9000 framebuffer output:** pixels are written directly into the display card's framebuffer. **Standalone percentage gain not isolated**; this is the rendering foundation for the comparisons below. [Architecture](pc/direct.cpp)
- **Host scheduling — 1 ms timer polling:** replace tick-based waits with a 1 ms timer — **+116.2% Keen / +66.7% Vette** startup throughput. [Results](../records/2026-10-12/dosbox-scheduling/comparison.json)
- **Scalar pixel scaling — specialised 1×/2× row copies:** **+14.4% Keen / +10.0% Vette**. [Results](../records/2026-10-12/dosbox-scaler/README.md)
- **Framebuffer pipelining — copy/PAN overlap prototype:** **+0.1–0.4% observed; no established gain** in the startup tests. [Results](../records/2026-10-12/dosbox-pipeline/README.md)
- **Compiler optimisation — x86 interpreter `-O2`:** **+7.7% Keen / +4.2% Vette**. [Results](../records/2026-10-12/dosbox-cpu-o2/README.md)
- **Lookup tables — RGB565 colour conversion:** **+16.3% Keen / +14.3% Vette**; exact colours preserved. [Results](../records/2026-10-12/dosbox-profile/README.md)
- **Compiler optimisation — PIC/VGA handlers `-O2`:** **+2.3% Keen / +6.2% Vette**. [Results](../records/2026-10-12/dosbox-events/README.md)
- **Division-free generic scaling and BIOS disk read-ahead:** **hardware gain not isolated**; retained as research, not a measured speed claim. [Evidence](../records/2026-10-13/dosbox-optimisation/artifacts.json)
- **JIT / dynrec — x86-to-ARMv7 dynamic recompilation:** **+124% Doom**, 3.10 → 6.94 FPS, replacing repeated x86 interpretation with cached ARM code. [Results](../records/2026-10-13/dosbox-dynrec/method.md)
- **Framebuffer pipelining — bounded overlap with dynrec:** **+0.24% observed; no established gain**, 6.944 → 6.961 FPS. [Results](../records/2026-10-13/dosbox-tuning/results.json)
- **JIT code caching — increase from 1 to 4 MiB:** **+0.10% observed; no established gain**, 6.961 → 6.967 FPS. [Results](../records/2026-10-13/dosbox-tuning/results.json)
- **SIMD / NEON — vectorised pixel processing:** **+4.6% Doom**, 6.97 → 7.29 FPS versus the same 4 MiB/overlap build. [Results](../records/2026-10-13/dosbox-tuning/method.md)
- **Scanline masking — skip black-row framebuffer writes:** **+3.7% Doom**, 7.33 → 7.60 FPS; optional visual trade-off, not a general rendering gain. [Results](../records/2026-10-13/dosbox-scanlines/method.md)
- **Adaptive — reduce frame presentation frequency:** **+13.5% Doom**, 7.27 → 8.25 FPS; optional, achieved by presenting fewer frames. Normal remains the default. [Results](../records/2026-10-13/dosbox-presentation/results.json)
- **OpenGL — experimental TinyGL software rendering on the A9:** **−32.5% Doom versus Direct**, 7.00 → 4.72 FPS in one matched pair with profiling enabled. This adds a software rendering stage; it is not FPGA 3D acceleration. Direct remains the default. [Results](../records/2026-10-13/dosbox-opengl/method.md)
- **VFP — hardware floating-point PIC event scheduling:** **+4.7% Doom**, 7.29 → 7.64 FPS averaged across two runs per build, with no skipped frames. [Report](../../docs/amiga-dosbox-pic-vfp.md)

- **Disk caching — 1 MiB Windows image cache:** **14.3% less startup elapsed time** (77.48 → 66.41 s), equivalent to **+16.7% workload throughput**, with synchronous writes. Two runs per cache size; not a general Windows FPS claim. [Report](../../docs/amiga-dosbox-optimisation-round2.md)
- **On-demand data — indexed compatibility reports:** **73.3% smaller launcher executable** (1.84 MB → 493 KB); one selected report loaded at a time. This is a size reduction, not an emulation speed measurement. [Report](../../docs/amiga-dosbox-optimisation-round2.md)

- **SIMD/VFP context isolation — library math in scalar PC builds:** save and restore all 32 D registers and FPSCR even with scalar pixels. **Correctness fix; no speedup claimed.** [Regression and evidence](../../docs/amiga-dosbox-optimisation-round2.md)

These techniques use the existing XACP application/host execution model;
none required a firmware change. Future firmware work should preserve the
Core1, framebuffer-handoff and processor-state restoration contracts.
