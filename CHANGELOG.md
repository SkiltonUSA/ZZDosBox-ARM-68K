# ZZDosBox changelog

## 0.17.0-dev — experimental Network tab

- Add per-program DHCP/manual IPv4, private subnet/gateway, DNS, hostname,
  MTU and DHCP timeout/retry settings.
- Add a network terminal, DHCP acquisition, saved-lease release, guest ping
  and DNS lookup, live counters and cooperative Disconnect.
- Share Roadshow's existing route using NE2000 and libslirp NAT.
- Validate settings and migrate older profiles with networking disabled.
- Keep networking opt-in in a separate interpreter runtime; reject
  incompatible renderer/core/debugger combinations. Windows networking and
  IPX remain pending. Release clears the saved guest lease, not Roadshow's IP.

## 0.16.0-dev — on-demand reports and Windows disk cache

- Load selected compatibility details from `Compatibility.dat`; reduce the launcher executable by 73.3%.
- Validate data headers, indexes, record bounds and checksums; recover when a missing file is restored.
- Add configurable build-time BIOS image cache capacity. The measured 1 MiB Windows candidate cuts startup elapsed time by 14.3% versus 128 KiB.
- Limit CAMD cache maintenance to consumed MIDI spans while preserving pacing and MT-32 SysEx delays.
- Preserve SIMD/FPSCR state in scalar PC builds too: linked library math can use VFP independently of pixel settings.
- Add paging/remapping/fault regression fixtures. Keep the production paged-JIT fallback.
- Add acknowledgement counters and elapsed time to fullscreen failure diagnostics.
- Keep indexed-pixel capture unrolling experimental; a failed fullscreen run is excluded from performance results.


## 0.15.2-dev — complete-discussion topic overviews

- Index all 6,999 cached comments across 2,428 discussions, including older replies.
- Show contributor counts, date range and automatic topic counts in the right pane.
- Count unclassified comments explicitly; topic mentions are not verified fixes.
- Keep the latest excerpt separately labelled and retain original-report links.
- Expand the scrolling discussion area and bundle updated Markdown details.

## 0.15.1-dev — offline compatibility reference

- Add Compatibility to the MUI launcher: A–Z/0–9 filters, title search and report links.
- Place games on the left and per-game details on the right: aligned metadata,
  a DOSBox version/status/rating table, and a separate scrolling comment preview.
- Include 3,937 upstream DOSBox reports covering 3,932 games, with tested versions and ratings.
- Include per-game publisher/tester details, 8,709 recorded version ratings and 2,428 brief comment previews.
- Bundle summary and detailed Markdown references; keep CSV/JSON exports and reproducible scrapers.
- Clearly distinguish upstream reports from verified ZZDosBox results.

## PIC hardware floating point — 14 October 2026

- Promote the measured Direct runtime using A9 VFP for PIC event scheduling;
  keep the same queue algorithm, guest cycles and framebuffer handoff.
- Save/normalise/restore SIMD and FPSCR state using the existing firmware guard.
- Doom Demo3 averages 7.2944 → 7.6358 FPS (+4.68%), two runs per build,
  with no frame skipping. Vette startup improves slightly; Empire is unchanged.
- Physical EGA/PIT and startup/cancellation checks pass, including the installed
  path. Preserve the previous Direct runtime for rollback.
- Publish the measured executable, symbols, regression evidence and chronological
  optimisation history. OpenGL, Developer MUI and Windows runtimes are unchanged.

## 0.11.1-dev — Developer controls (hardware checked; not installed)

- Added a Developer MUI tab with a development-only performance caution.
- Added optional trace logging, CPU/video profiling, display handoff and polling controls.
- Added before/after-frame cooperative checkpoints with Continue, Next frame and Stop.
- Added Open log; developer preferences reset at launcher exit.
- Fixed MultiView lookup to include its standard `SYS:Utilities` location.
- Physical Hercules checks passed Direct/OpenGL frame stepping, Continue,
  Stop, clean teardown, option reset and the completed-log viewer.
- Separate Direct/OpenGL Trace executables; normal installed runtimes remain unchanged.
- Checkpoints run outside the ARM framebuffer mapping. The host drains display
  completion before opening nonblocking controls and continues servicing transport.

## Direct-target OpenGL experiment — 14 October 2026 (not installed)

- Render eligible 640×480 frames into the validated XACP back buffer; retain
  the existing path for resized windows and scanline masks.
- Measured 5.06 FPS OpenGL versus 6.94 FPS Direct in Doom Demo3. Previous GL
  measured 4.72 FPS; the historical comparison suggests about 7% improvement.
- A subsequent loop refinement passed software tests but the Amiga froze
  during its first hardware test. Withdrawn; exact cause remains unresolved.
- Source restored to the first tested variant. Installed runtimes unchanged.
- Keep Settings choices Direct / OpenGL (experimental), saved per program.

[Test and failure record](amiga/records/2026-10-14/dosbox-gl-direct/method.md).

## 0.11.0-dev — graphics adapters and rendering

- Added per-program Hercules monochrome colours: White, Amber, Green, Paperwhite.
- Added an optional A9 TinyGL software renderer; Direct remains the default.
  No GLSL or guest OpenGL driver.
- Added S3/VESA, Tandy, PCjr and Hercules to the per-program video selector.
- Added bounded S3 banked/linear framebuffer mapping and RGB555 capture.
- Limited S3 to 1 MB VRAM and capture dimensions up to 1024×768.
- Retained Normal/Adaptive, scanlines, NEON conversion and the 4 MiB JIT cache.
- Corrected Hercules welcome/boot text addressing. A separate welcome helper preserves existing copies.
- Tandy/PCjr sound and Windows S3 driver compatibility are not included in the verified feature set.

## 0.10.3-dev launcher — 13 October 2026

- Added Normal / Adaptive presentation to Settings, saved per program; Normal default.
- Installed the optional adaptive runtime and Off/Classic/Soft/Gradient scanline choices.
- Preserved existing profiles; verified migration, save/reopen and physical launch/exit.
- Verified the low-stack guard and 64 KiB Workbench icon stack.

## Adaptive presentation experiment (now optional in MUI)

- Optional fixed or adaptive frame presentation; guest CPU/timing stays unchanged.
- Counts generated, displayed and skipped frames separately; FPS overlay follows presentation count.
- Separate experimental executable; default remains Off. Physical Doom Demo3: 7.27 to 8.25 real FPS (+13.5%) in a matched pair with profiling disabled. Five complete timedemos passed cleanup; visual smoothness remains unassessed.


Amiga-specific development history, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).
These are development milestones, not upstream DOSBox release numbers.

## Earlier scanline-style launcher candidate (superseded above)

- Added per-program Off, Classic, Soft and Gradient choices in MUI Settings.
  Fullscreen with Show FPS off; existing profiles default to Off.
- Classic preserves black rows; Soft/Gradient fuse dimming with ARM scaling.
  The separate optional runtime leaves normal launches unchanged.
- Added a 64 KiB launcher stack guard after a staged CLI launch faulted with
  a 16 KiB stack. Subsequent physical verification and installation are recorded above.

## ARMv7 dynrec, NEON and scanline experiments — 13 October 2026

- Integrated the existing ARMv7 dynamic recompiler with owned executable memory
  and A9 cache publication. Doom Demo3 improved from 3.10 to 6.94 real FPS.
- Installed a tested 4 MiB JIT-cache / NEON-pixel / display-overlap build:
  7.29 real FPS, with interpreter and initial-dynrec fallbacks retained.
- Separate Classic scanline experiment: normal 7.33 FPS, rewritten black rows
  7.44, preserved black rows 7.60. Single runs; preliminary +3.7% vs normal.
  This saves final framebuffer writes, not guest rendering; it is independent
  of FPGA scanline controls and has not replaced the normal executable.
- Physical lifecycle, mode-toggle and pixel checks passed; detailed evidence
  is retained with each experiment. No firmware change.

## Earlier optimisation candidate — historical

- Replaced per-pixel division in generic nearest-neighbour scaling with an
  exact quotient/remainder accumulator; preserved 1x/2x paths and output pixels.
- Added bounded 4 KiB read-ahead for exclusively owned BIOS hard-disk images.
  Writes remain immediate, invalidate cached reads, and preserve error handling.
- Local pixel, disk-model and ARM scaler checks pass. Physical comparisons
  were pending at this stage; later results and
  installed-build status are recorded above.

## Mouse development build — 13 October 2026

- Added the upstream DOSBox INT33 mouse driver and BIOS PS/2 callback support.
- Added a bounded 68K-to-ARM input queue with independent producer/consumer
  cache lines, explicit initialisation and checked event fields.
- Added three-button Intuition input in Workbench windows and fullscreen,
  coordinate scaling to the painted image, and button release on focus loss
  or window/fullscreen switching.
- Kept mouse callbacks in reserved BIOS storage so a booted DOS kernel owns
  its conventional-memory area.
- Verified queue wrap, positions, buttons, PS/2 IRQ12, invalid-input rejection
  and cancellation in native/ARM tests; Oregon Trail menu interaction works
  on hardware. Windows native and physical testing verified movement and
  double-click application launch.

## Windows development build — 13 October 2026

Separate experimental executable, still identifying as 0.10.1-dev.

- Implemented fast-A20 port 0x92 and correct initial A20 mapping.
- Protected BIOS/VGA ROM from guest writes and restored upstream full-core
  handling of nested page faults.
- Reduced sparse TLB bank allocations from 1.25 MB to 80 KB, resolving the
  initial Windows worker-heap failure while retaining 4 GB linear coverage.
- Kept 8 MB guest RAM within the existing 32 MB worker allocation.
- Verified Windows for Workgroups 3.11 desktop, WinBench and normal Windows exit
  on the physical ZZ9000 A9; emulator teardown restored registers/cache and
  released its memory.
- Completed WinBench 3.11 Graphics WinMark: **1,923,752 pixels/sec**, standard
  VGA 640×480/16 colours at 3000 cycles/ms. This and the displayed “37 MHz 80386”
  estimate use the emulated clock, not a proven real-PC speed equivalence.
- Passed ARM regression tests for A20, ROM protection, page faults and sparse
  high-address mappings. Added physical screenshots and benchmark records.

## 0.10.1-dev — 13 October 2026

- Added the DOS welcome screen with version, project link and keyboard controls.
- Added Alt+Return window/fullscreen switching and live Ctrl+minus/plus cycle
  adjustment, including delayed window activation after display changes.
- Added external MT-32/General MIDI through CAMD, plus CAMD, AHI and ZZ9000AX
  driver detection. PCM sound remains pending.
- Grouped AUTOEXEC.BAT, CONFIG.SYS and CD-ROM controls under DOS terminal.
  Startup files support Default, Edited and File choices, with recovery of
  temporary changes to the booted DOS disk.
- Added read-only ISO9660 and mounted Amiga data-CD access, including the
  original ZZDOSCD.SYS gateway for booted MS-DOS.
- Verified display switching, MIDI capture, CD reads, startup-file restoration
  and clean shutdown on hardware. External synthesizer audio remains untested.

## 0.9.1-dev — 13 October 2026

- Added 320×240, 640×480 and 960×720 window presets on the DOS terminal tab.
- Added Workbench resize handling with centred 4:3 output and black margins.
- Moved Show FPS to Settings; preserved prior profile formats.

## 0.9.0-dev — 13 October 2026

- Added Workbench window output alongside fullscreen operation.
- Added embedded DOS 5 / installed MS-DOS 6.22 selection.
- Added the optional ARM-rendered display-refresh counter.

## 0.8.0-dev — 13 October 2026

- Added writable 32 MB BIOS hard-disk images and Workbench mount/eject controls.
- Verified MS-DOS boot, persistent files and program-profile launches.
- Recorded Keen startup throughput averaging 37.2 refreshes/sec across three
  runs; this was a timed startup test, not a full-speed gameplay claim.

## 0.7.0-dev — 12 October 2026

- Added nested DOS directories and bounded host-file access.
- Added read-only 1.44 MB BIOS floppy boot and guest RAM choices up to 8 MB.
- Introduced the banked TLB to fit the interpreter within its owned allocation.
- Compared selected behaviour with DOSBox-X; retained dosbox-libretro as the
  source base.

## 0.6.0-dev — 12 October 2026

- Added the MUI program launcher, profiles, LOADFIX option, startup-file editors,
  terminal colours, video choices and CPU-cycle presets.
- Added live keyboard input and sessions that run until closed.

## Earlier prototypes — 12 October 2026

- Established x86 interpreter execution on the application-owned A9 Core1.
- Added the DOS kernel, COM/EXE loading, console, shell and writable C: drawer.
- Added PC BIOS/PIT/keyboard devices and CGA/EGA/VGA display integration.
- Tuned host polling, ARM scaling, conversion and compiler settings using
  repeatable physical tests; retained rejected experiments separately.
