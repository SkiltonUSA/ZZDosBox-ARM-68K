# ZZDosBox-ARM-68K

**DOSBox for AmigaOS and the ZZ9000 Cortex-A9**, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).
The ARM runs the emulated PC; the 68K handles the MUI interface, AmigaOS services
and display coordination through [Xani's XACP platform](https://github.com/Xanxi-Amiga/XACP-ZZ9000).

[Development builds](#development-builds) · [Features](#features) ·
[Dependencies](#dependencies) · [Build instructions](#build-instructions) ·
[Performance](#optimisation-history) · [Credits](#credits-and-licences)

## Development builds

This is development software. Builds and research artifacts are kept separately:

- [Current measured Direct runtime](artifacts/development/pic-vfp): ARMv7 JIT,
  4 MiB code cache, NEON pixels and VFP event scheduling.
- [Windows/mouse development build](artifacts/development/mouse): Windows for
  Workgroups 3.11, DOS mouse and PS/2 input validation.
- [Recovery artifacts](artifacts/0.10.1-dev) and
  [build/restore instructions](docs/build-and-restore.md).
- [TinyGL diagnostic builds and measurements](amiga/records/2026-10-14/tinygl-compiler):
  experiments, not replacements for the normal runtime.

## Features

| Feature | Current support |
|---|---|
| Native interface | MUI program profiles, DOS terminal, Settings, disk controls and About page. |
| DOS environments | Embedded DOS 5 or booted MS-DOS 6.22 using separately supplied media. |
| Execution | 32-bit x86 emulation on A9 Core1; ARMv7 dynamic recompilation in the current Direct runtime. |
| Display | Fullscreen or resizable Workbench window, three size presets, centred 4:3 output and optional FPS display. |
| Graphics adapters | VGA, EGA, CGA, S3/VESA, Tandy, PCjr and Hercules selections; broader high-resolution compatibility needs testing. |
| Presentation | Direct framebuffer output, optional scanline masks and Normal/Adaptive presentation. |
| Input | Keyboard, DOS mouse and PS/2 mouse; live fullscreen/window and CPU-cycle controls. |
| Storage | Amiga drawer as C:, BIOS floppy/hard-disk images, read-only ISO9660 or mounted data CDs. |
| Startup files | Editable AUTOEXEC.BAT and CONFIG.SYS with Default / Edited / File selection; optional LOADFIX helper. |
| MIDI | External Roland MT-32 or General MIDI through CAMD. |
| Windows | Windows for Workgroups 3.11 and WinBench verified in the separate Windows build. |
| Developer tools | Separate development build with tracing, frame checkpoints, guest memory viewer/scanner and x86 disassembly. |

PCM sound-card audio, joystick input, emulator save states and guest networking
are not implemented. Embedded DOS does not load CONFIG.SYS device drivers.
See [features and compatibility](docs/features.md) for per-build limitations.

![ZZDosBox DOS welcome screen](docs/screenshots/dos-welcome.png)

## Dependencies

### Amiga runtime

| Dependency | Version / configuration | Provides | Required? |
|---|---|---|---|
| Zorro III Amiga + ZZ9000 | 68020 instruction target; physically validated with a 68060 | 68K host and Cortex-A9 worker | Yes |
| AmigaOS | Tested on 3.2.3; `workbench.library` v44+, `asl.library` v38+, `graphics.library` v39+ | Workbench, requesters, graphics, Exec/DOS/Intuition/Expansion and timer.device | Yes |
| ZZ9000 firmware / XACP | Tested XX19c / XACP 1.7 | ARM execution and display handoff | Yes |
| ZZ9000 Fast RAM | Card's 256 MB Zorro III RAM enabled; 32 MB contiguous free worker block, plus OS/display memory | Worker code, guest RAM and buffers | Yes |
| Picasso96 and ZZ9000 RTG driver | `Picasso96API.library` v2+; supported 32-bit Workbench and 640×480 fullscreen mode | Window and fullscreen display | Yes |
| MUI | 3.8+; standard classes only | Native launcher | Launcher only |
| Writable application storage | Space for profiles, programs and disk images; 64 KiB application stack | Settings and persistent files | Yes |

Guest PC RAM is selectable as **1, 2, 4 or 8 MB**, inside the worker's 32 MB
allocation. Other 68K configurations and firmware versions remain unverified.

### Optional runtime components

| Dependency | Version / configuration | Provides | Required when |
|---|---|---|---|
| CAMD | `camd.library` v37+, working output driver and external MIDI instrument | MT-32 / General MIDI output | External MIDI is selected |
| AHI | v4+ supported by the detector | Driver discovery only | Using the audio-driver checker; does not enable PCM playback |
| ZZ9000AX | Installed AHI modes | Optional mode discovery | Using AX detection; not needed to run the emulator |
| diskimage.device + DiskImageCtrl + fat95 | Tested device 37.7; `L:fat95` | Workbench mounting and booted-DOS startup-file editing | Using those disk-image tools; direct BIOS image boot does not need them |
| MS-DOS 6.22 | User-supplied media | Booted Microsoft DOS | Choosing MS-DOS instead of embedded DOS |
| MSCDEX.EXE | User-supplied; paired with included `ZZDOSCD.SYS` | CD access from booted MS-DOS | Using that DOS CD driver |
| Windows for Workgroups 3.11 | User-supplied media | Windows applications | Running Windows |

SDL/SDL2, RetroArch, a development Mac and Amiga-Bridge are not runtime
requirements. The experimental TinyGL renderer is compiled into its ARM payload;
no separate Amiga OpenGL library is required. FluidSynth, Munt and desktop
DOSBox Staging audio/network libraries are not dependencies of this port.

[Detailed Amiga requirements](docs/requirements.md)

### Source and build dependencies

| Dependency | Version / source | Purpose | Required? |
|---|---|---|---|
| Git, Make, Python 3 | Tested with Python 3.14; archive scripts use extraction filters | Source preparation, builds and verification | Yes for building |
| Clang / Clang++ + LLVM + lld | Host tools, including `llvm-nm` and `ld.lld` | Native tests and original x86 diagnostic fixtures | Yes for the supplied workflow |
| Podman or Docker | Configurable container commands | Cross-compiler containers | Yes for the supplied workflow |
| ARM GCC/G++ + binutils | Tested GCC 15.3.1, `arm-none-eabi-*` | Cortex-A9 worker | Yes for ARM builds |
| Amiga m68k GCC + SDK headers | Digest-pinned `amigadev/crosstools` in [toolchain metadata](amiga/upstream.json); AmigaOS, MUI and Picasso96 APIs | 68K launcher and host | Yes for 68K builds |
| Picolibc | 1.8.12; archived PIC libc/source and retained notices | Freestanding ARM C runtime | Yes for ARM builds |
| DOSBox / libretro | Pinned [`a563bb5`](https://github.com/libretro/dosbox-libretro/tree/a563bb5e9753f207d74e9d8c417d793b784fa4be); source bundle included | Emulation engine | Yes |
| ImageMagick | `magick` command | Workbench icon conversion | Launcher/icon builds |
| jserv TinyGL | Pinned [`c2e4859`](https://github.com/jserv/tinygl/tree/c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a); source archive included | Experimental software OpenGL and native scene tests | Optional |
| ARM CMSIS-DSP | Pinned [`83a2d7b`](https://github.com/ARM-software/CMSIS-DSP/tree/83a2d7bc98c81b4bbe4a6f48b1f2ecf179868a0b) | Standalone matrix comparison | Matrix experiment only |
| Unicorn | Python package `unicorn==2.1.4` | Linked ARM instruction-model tests | Model tests only |
| Python MCP SDK + Amiga-Bridge MCP | Working bridge connection and Roadshow or compatible Amiga TCP stack | Scripted hardware transfer and measurements | Remote development only |

The x86 memory-viewer disassembler is adapted from pinned DOSBox source in
[the vendor directory](amiga/dosbox/developer/vendor); no Capstone runtime is needed.
The current host scripts were exercised on macOS and retain some Homebrew paths.

## Developer toolchain

| Area | Implementation |
|---|---|
| Languages | C99/C11, C++17, ARM and 68K assembly, Python |
| Build system | Python preparation/cross-build scripts with Make targets |
| Dependency tracking | Pinned revisions, source archives, compiler metadata and SHA256 manifests |
| Native validation | Exact-output fixtures, bounds tests, AddressSanitizer and UndefinedBehaviorSanitizer |
| ARM validation | Unicorn lifecycle and processor-state restoration tests |
| Hardware validation | Recorded A9 measurements through Amiga-Bridge MCP |

## Get the sources

```sh
git clone https://github.com/SkiltonUSA/ZZDosBox-ARM-68K.git
cd ZZDosBox-ARM-68K
make verify
```

## Build instructions

Start with [Build and restore](docs/build-and-restore.md). The repository preserves
our development toolchain layout; it is not a turnkey SDK installer.

After configuring the ARM and Amiga cross-toolchains and mounting this checkout
at `/work` in the ARM container:

```sh
python3 scripts/restore_build_inputs.py --pic-vfp-build
make test-host
make build-current
python3 scripts/build_dosbox_launcher.py
```

For instruction-model checks, use a Python environment containing Unicorn:

```sh
/path/to/python-with-unicorn tests/amiga/dosbox/check_dynrec_arm.py \
  --out .context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp
```

See [TinyGL experiment reproduction](docs/amiga-tinygl-compiler-experiment.md#reproduction)
for the separate rendering tests. Builds and host/model checks do not install
or launch anything on the Amiga.

## Experimental OpenGL / TinyGL

The A9 software renderer uses [jserv's TinyGL](https://github.com/jserv/tinygl),
pinned to [`c2e4859`](https://github.com/jserv/tinygl/commit/c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a).
Thanks to **jserv**, **Fabrice Bellard**, **C-Chads** and the TinyGL contributors.
It provides a partial fixed-function OpenGL API, with specialised depth-test/write
raster paths. Our integration adds bounded memory allocation, pixel presentation
and experimental A9 NEON routines. It is software rendering, not FPGA 3D acceleration.
Direct remains the default renderer.

The latest native TinyGL experiment measured **+28.59% visible fill**,
**+49.00% hidden fill** and **+33.37% mixed colour fill** using
scalar rejection, unguarded NEON packing and per-triangle dispatch. Small triangles
and clipping stay within 1% of scalar speed. Mixed textured scenes gain
**5.20%**; pure texture paths are unchanged. These are offscreen
native-rendering results, not DOS game FPS gains; the changes remain experimental.

[Packing, dispatch and textured results](docs/amiga-tinygl-packing-experiment.md) ·
[Hidden-pixel rejection](docs/amiga-tinygl-rejection-experiment.md)

[Raster results](docs/amiga-tinygl-raster-experiment.md) ·
[Compiler comparison and C-Chads review](docs/amiga-tinygl-compiler-experiment.md) ·
[Matrix/transform research](docs/amiga-tinygl-internal-transforms.md)

## Optimisation history

Oldest to newest. Percentages compare each change with its own measured
baseline; they are **not cumulative**. Keen/Vette figures are startup
throughput, while Doom figures are real Demo3 game FPS. Most earlier comparisons
are single runs; the latest VFP result uses two runs per build.

- **Direct — ARM-to-ZZ9000 framebuffer output:** pixels are written directly into the display card's framebuffer. **Standalone percentage gain not isolated**; this is the rendering foundation for the comparisons below. [Architecture](amiga/dosbox/pc/direct.cpp)
- **Host scheduling — 1 ms timer polling:** replace tick-based waits with a 1 ms timer — **+116.2% Keen / +66.7% Vette** startup throughput. [Results](amiga/records/2026-10-12/dosbox-scheduling/comparison.json)
- **Scalar pixel scaling — specialised 1×/2× row copies:** **+14.4% Keen / +10.0% Vette**. [Results](amiga/records/2026-10-12/dosbox-scaler/README.md)
- **Framebuffer pipelining — copy/PAN overlap prototype:** **+0.1–0.4% observed; no established gain** in the startup tests. [Results](amiga/records/2026-10-12/dosbox-pipeline/README.md)
- **Compiler optimisation — x86 interpreter `-O2`:** **+7.7% Keen / +4.2% Vette**. [Results](amiga/records/2026-10-12/dosbox-cpu-o2/README.md)
- **Lookup tables — RGB565 colour conversion:** **+16.3% Keen / +14.3% Vette**; exact colours preserved. [Results](amiga/records/2026-10-12/dosbox-profile/README.md)
- **Compiler optimisation — PIC/VGA handlers `-O2`:** **+2.3% Keen / +6.2% Vette**. [Results](amiga/records/2026-10-12/dosbox-events/README.md)
- **Division-free generic scaling and BIOS disk read-ahead:** **hardware gain not isolated**; retained as research, not a measured speed claim. [Evidence](amiga/records/2026-10-13/dosbox-optimisation/artifacts.json)
- **JIT / dynrec — x86-to-ARMv7 dynamic recompilation:** **+124% Doom**, 3.10 → 6.94 FPS, replacing repeated x86 interpretation with cached ARM code. [Results](amiga/records/2026-10-13/dosbox-dynrec/method.md)
- **Framebuffer pipelining — bounded overlap with dynrec:** **+0.24% observed; no established gain**, 6.944 → 6.961 FPS. [Results](amiga/records/2026-10-13/dosbox-tuning/results.json)
- **JIT code caching — increase from 1 to 4 MiB:** **+0.10% observed; no established gain**, 6.961 → 6.967 FPS. [Results](amiga/records/2026-10-13/dosbox-tuning/results.json)
- **SIMD / NEON — vectorised pixel processing:** **+4.6% Doom**, 6.97 → 7.29 FPS versus the same 4 MiB/overlap build. [Results](amiga/records/2026-10-13/dosbox-tuning/method.md)
- **Scanline masking — skip black-row framebuffer writes:** **+3.7% Doom**, 7.33 → 7.60 FPS; optional visual trade-off, not a general rendering gain. [Results](amiga/records/2026-10-13/dosbox-scanlines/method.md)
- **Adaptive — reduce frame presentation frequency:** **+13.5% Doom**, 7.27 → 8.25 FPS; optional, achieved by presenting fewer frames. Normal remains the default. [Results](amiga/records/2026-10-13/dosbox-presentation/results.json)
- **OpenGL — experimental TinyGL software rendering on the A9:** **−32.5% Doom versus Direct**, 7.00 → 4.72 FPS in one matched pair with profiling enabled. This adds a software rendering stage; it is not FPGA 3D acceleration. Direct remains the default. [Results](amiga/records/2026-10-13/dosbox-opengl/method.md)
- **VFP — hardware floating-point PIC event scheduling:** **+4.7% Doom**, 7.29 → 7.64 FPS averaged across two runs per build, with no skipped frames. [Report](docs/performance-pic-vfp.md)
- **TinyGL SIMD / NEON — exact integer colour spans and zero clearing:** **+22.93% visible fill / +7.75% zero clear**, but **−18.66% mostly hidden fill**. Native offscreen scenes, not DOS game FPS; experimental. [Report](docs/amiga-tinygl-raster-experiment.md)
- **Compiler selection — retain O2:** O3 and automatic vectorisation gave **about 14% lower original-fill throughput**. Native TinyGL, three runs per build. [Report](docs/amiga-tinygl-compiler-experiment.md)
- **Scalar rejection + NEON — skip hidden span prefixes:** **+47.30% mostly hidden fill / +12.79% mixed fill / +2.89% visible fill**, with about **3% lower small-triangle throughput**. Dedicated native builds, three runs each; experimental. [Report](docs/amiga-tinygl-rejection-experiment.md)
- **Scalar/NEON triangle dispatch + unguarded packing:** **+28.59% visible fill / +49.00% hidden fill / +33.37% mixed colour fill**; small geometry/clipping within 1% of scalar. Mixed textured scenes **+5.20%**, pure textures unchanged. Native research only, three runs per build. [Report](docs/amiga-tinygl-packing-experiment.md)

These techniques use the existing XACP application/host execution model;
none required a firmware change. Future firmware work should preserve the
Core1, framebuffer-handoff and processor-state restoration contracts.

CPU cycles are a fixed workload budget; see the [CPU settings review](docs/amiga-dosbox-cpu-settings-review.md) for supported controls and the next measurement plan.

## Windows measurements

Windows for Workgroups 3.11 booted on the real ZZ9000 A9. WinBench 3.11 completed
Graphics WinMark at **1,923,752 pixels/sec** (640×480, 16 colours, 8 MB guest RAM,
3000 cycles/ms), followed by a clean exit to DOS and Workbench. Its score and
“37 MHz 80386” label use the emulated PC clock and do not establish equivalent
physical PC speed.

![WinBench running on the ZZ9000](docs/screenshots/windows311/winbench-results.png)

[Windows screenshots](docs/screenshots/windows311/README.md) ·
[Windows findings](amiga/records/2026-10-13/dosbox-wfw/findings.md)

## Documentation and contributions

[Changelog](CHANGELOG.md) · [Features](docs/features.md) ·
[Requirements](docs/requirements.md) · [Build and restore](docs/build-and-restore.md) ·
[Research records](amiga/records) ·
[Report an issue](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/issues)

For bug reports, include the build, firmware, display mode and reproduction
steps. Performance changes should include matching output checks and physical
A9 timings; instruction-model timing is not hardware performance evidence.

## Credits and licences

- [DOSBox team and libretro contributors](https://github.com/libretro/dosbox-libretro): emulation engine.
- [Xani / Xanxi](https://github.com/Xanxi-Amiga/XACP-ZZ9000): XACP platform and application execution model.
- [jserv](https://github.com/jserv/tinygl), Fabrice Bellard, C-Chads and TinyGL contributors: software OpenGL renderer.
- Stefan Stuntz: MUI.
- ARM CMSIS-DSP, Picolibc and GCC contributors: retained component acknowledgements in [licence notices](licenses/README.md).

Development: **Codex assisted, Amiga-Bridge MCP**.

DOSBox upstream is GPL-2.0-or-later; the combined project includes GPL-3.0-or-later
components. See [LICENSE](LICENSE) and [component notices](licenses/README.md).
Games, Microsoft software and WinBench are supplied separately.
