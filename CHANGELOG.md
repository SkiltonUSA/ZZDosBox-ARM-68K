# ZZDosBox changelog

Amiga-specific development history, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).
These are development milestones, not upstream DOSBox release numbers.

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
