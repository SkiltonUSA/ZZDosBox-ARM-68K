# ZZDosBox changelog

Version history from **0.1.0-dev** through **0.22.7-dev**, newest first.
Versions identify ZZDosBox development milestones, not upstream DOSBox releases.
Launcher and ARM runtimes can carry different version strings; experiments
without a new version are recorded separately below. Historical validation and
installation statements describe that milestone, not the current default.

Reconstructed from retained source, build metadata and development reports.
No distinct 0.10.2 milestone was established from those records; the earlier
scanline candidate is retained in the experiment notes rather than assigned one.

## 0.22.7-dev — scalable Workbench launcher

- Size the initial window to the available public screen, with a compact
  layout for 640×480 and 800×600 desktops.
- Keep navigation and Save settings outside individually scrollable pages;
  resizing the window no longer requires every settings panel to fit at once.
- Use smaller previews, tighter spacing and compact local fonts on smaller
  screens. Stack the display guide and compatibility details, and use a
  single column of network fields in compact mode.
- Fit startup/loading cards to the compact layout. No screen-mode or global
  font preference changes; emulator execution and saved profiles are unchanged.

## 0.22.6-dev — high-cycle launcher choices

- Add saved 60,000 and 100,000 CPU-cycle choices; existing selections keep
  their values. High-cycle profiles select the Expanded runtime automatically.
- Keep dosbox.conf override precedence and existing restrictions for networking,
  OpenGL, interpreter and developer modes. Add save/load and routing regressions.
- Explain timing/audio trade-offs in the CPU-cycle tooltip.

## 0.22.5-dev — guarded ARM inline reads

- Expanded runtime emits aligned linked low-bank reads directly in ARM code;
  device, unaligned and high-bank accesses keep checked helper handling.
  Writes and self-modifying-code tracking remain unchanged.
- Duke opening-level samples improved 4.53 → 4.67 game FPS (+2.9%) at the same
  10,000 cycles; repeat samples agree. This is a measured workload result,
  not a general emulator speed multiplier.
- Expose the existing late AHI-buffer-link diagnostic. Delivery timing improved,
  but audio continuity remains limited and audible quality was not verified.
- Add page-alignment/read, partial-register, carry and memory-handler regression
  fixtures, scoped hot-block profiling and optional register-ABI/forwarding
  experiments. The latter showed no meaningful FPS gain and are not defaults.
- Compare 10k/12k/15k/20k window/fullscreen runs. Higher FPS at 20k accompanies
  roughly half-speed emulated time; retain the saved 10k configuration.
- Document missing Duke keyboard assignments and their configuration fix;
  the original game executable is unchanged.

[Measurements, validation and limitations](docs/amiga-dosbox-duke-arm-optimisation.md) ·
[Development runtime](artifacts/development/expanded-0.22.5)

## Experiment — Duke Nukem 3D configuration tuning

- Measure 1.3D rendered frames against ARM wall time: VGA/6000/Normal 1.17 FPS,
  S3/6000/Normal 1.63 FPS, S3/10000/Adaptive 4.53 FPS, with sound enabled.
- Add an isolated Duke counter observer and wraparound/relocation tests. Private
  benchmark copies enable the existing FPS counter; the installed game is unchanged.
- Save the faster Duke profile using the existing launcher/player; no new runtime
  release. [Method and limits](docs/amiga-dosbox-duke-performance.md).

## 0.22.4-dev — per-program CPU cycles

- Add a saved 10000-cycle choice on the CPU page; existing 1000, 3000 and 6000
  choices keep their values. Global dosbox.conf overrides still take precedence.
- Add an isolated, read-only Descent rendered-frame benchmark with gameplay-only
  timing. The normal player does not include the benchmark or its automatic stop.

## Unreleased — protected-mode game startup

- Back up the tested Compat/Expanded executables, build metadata and licensed
  DOS/32A helper in [the development update](artifacts/development/descent-startup).

- Use DOS/32A 7.35 for the personal Descent profile; first-level rendering and
  input verified on the Amiga without DOS/4GW paging. Original game unchanged.
- Fix ARM recompiler lookahead over self-modifying invalid FE-group bytes;
  add native, relocated ARM and physical regression checks.

- Add reproducible Doom/Duke shareware starter packaging and fresh launcher
  profiles. Retain Descent in personal builds only due to its bundling terms.

- Fix stale DOS error state during temporary-file creation, which prevented
  Descent's DOS/4GW 1.97 loader from starting (errors 2504/1011).
- Add a temporary-file regression and native per-game CPU selection for tests.
- Document Duke Nukem 3D's 486 setting and Quake's CD-audio startup pause.
- [Startup review and validation](docs/amiga-dosbox-shooter-startup-review.md).

## 0.22.3-dev — launcher temperature

- Show the ZZ9000 Zynq temperature beside the footer version, refreshed every
  two seconds while the launcher is active. Unavailable readings show n/a.
- Read-only sensor access; no ARM job or firmware changes.

## 0.22.2-dev — file reads and input scheduling

- Removed the GitHub button from About; aligned the Display page panels.

- Optional 4 KiB read-ahead per read-only DOS file; buffers drain before writes,
  truncation and other mutations, preserving AmigaDOS handle positions.
- Consecutive mouse motion is coalesced without merging button/focus transitions;
  leaving the painted area resets relative-position tracking.
- Opt-in paged JIT interprets unsupported instructions in bursts of up to 32
  cycles, bounded by the next scheduled event. Default paging remains interpreted.
- Refreshed I/O/joystick specifications and launcher screenshots.
- Measurements and installation scope: see the file-read optimisation report.

## 0.22.1-dev — network integration candidate

- Added `ZZDosBox-Net-JIT`: TCP/IP or IPX with ARM dynrec, 4 MiB code cache,
  NEON conversion, pipelined display, EMS and SBPro2/SB16/PC speaker.
- Launcher honours explicit normal core with the existing interpreter runtime;
  network terminal uses the selected runtime and audio settings too.
- Handle/device file operations no longer rewrite or synchronise unused path
  metadata. Writes stay synchronous; pathname operations retain validation.
- Expanded hardware/serial and booted Windows networking remain separate.
- Validation and hardware promotion are recorded in the integration report.

| Version | Main change |
|---|---|
| 0.22.2-dev | File read-ahead, mouse motion coalescing and batched paging fallback |
| 0.22.1-dev | Combined network/JIT/EMS/Sound Blaster candidate; less filesystem mailbox work |
| 0.22.0-dev | Experimental physical COM1, USB driver detection and per-program serial settings |
| 0.21.2-dev | Aligned joystick panels, 1-6 buttons and extra-button DOS key bindings |
| 0.21.1-dev | Dedicated I/O and live joystick/button tester pages |
| 0.21.0-dev | Optional expanded PC hardware and memory settings |
| 0.20.1-dev-compat-x87 | Experimental x87 guest FPU on A9 VFP |
| 0.20.1-dev | Sound Blaster choices, PC speaker and file dates |
| [0.20.0-dev](#v0-20-0) | EMS/XMS and Sound Blaster Pro 2 compatibility runtime |
| [0.19.12-dev](#v0-19-12) | Per-program screenshots and library previews |
| [0.19.11-dev](#v0-19-11) | Loading handoff, Rename, OS images and footer revision |
| [0.19.10-dev](#v0-19-10) | Program-list keyboard navigation |
| [0.19.9-dev](#v0-19-9) | Explicit scanline / FPS help |
| [0.19.8-dev](#v0-19-8) | Offline Network guide |
| [0.19.7-dev](#v0-19-7) | create hard disks |
| [0.19.6-dev](#v0-19-6) | disk browsing and settings help |
| [0.19.5-dev](#v0-19-5) | larger libraries |
| [0.19.4-dev](#v0-19-4) | library capacity |
| [0.19.3-dev](#v0-19-3) | ZZDosBox identity |
| [0.19.2-dev](#v0-19-2) | startup and loading splashes |
| [0.19.1-dev](#v0-19-1) | launcher presentation |
| [0.19.0-dev](#v0-19-0) | sidebar navigation |
| [0.18.0-dev](#v0-18-0) | IPX wrapper client |
| [0.17.0-dev](#v0-17-0) | experimental Network tab |
| [0.16.0-dev](#v0-16-0) | on-demand reports and Windows disk cache |
| [0.15.2-dev](#v0-15-2) | complete-discussion topic overviews |
| [0.15.1-dev](#v0-15-1) | offline compatibility reference |
| [0.15.0-dev](#v0-15-0) | initial compatibility browser |
| [0.14.0-dev](#v0-14-0) | dosbox.conf editor |
| [0.13.0-dev](#v0-13-0) | program categories |
| [0.12.0-dev](#v0-12-0) | memory development tools |
| [0.11.1-dev](#v0-11-1) | Developer controls (hardware checked; not installed) |
| [0.11.0-dev](#v0-11-0) | graphics adapters and rendering |
| [0.10.3-dev](#v0-10-3) | Adaptive presentation and scanline options |
| [0.10.1-dev](#v0-10-1) | Welcome screen, live controls, startup/CD/MIDI |
| [0.10.0-dev](#v0-10-0) | startup files, CD and MIDI |
| [0.9.1-dev](#v0-9-1) | Window sizes and resize handling |
| [0.9.0-dev](#v0-9-0) | Workbench windows and DOS selection |
| [0.8.0-dev](#v0-8-0) | Writable hard disks |
| [0.7.0-dev](#v0-7-0) | Directories, floppy boot and guest RAM |
| [0.6.0-dev](#v0-6-0) | MUI launcher and live input |
| [0.5.0-dev](#v0-5-0) | PC devices and direct graphics |
| [0.4.0-dev](#v0-4-0) | DOS shell and live files |
| [0.3.0-dev](#v0-3-0) | interactive console |
| [0.2.0-dev](#v0-2-0) | DOS kernel |
| [0.1.0-dev](#v0-1-0) | first ARM execution prototype |

## 0.22.0-dev — Physical serial (Experimental)

- Connect DOS COM1 to an AmigaOS serial-compatible driver: built-in serial,
  supported USB adapters or serial expansion cards, with device/unit selection.
- Follow DOS UART baud/framing or override physical baud, 7/8 data bits,
  parity and stop bits. Optional RTS/CTS driver handshaking.
- Asynchronous buffered read/write, modem input status and timed break output.
  RTS/DTR outputs remain driver-managed, not arbitrary guest pin control.
- Grey out the USB shortcut without Poseidon and a registered supported serial
  driver. Detect USB refreshes the check; no port is opened by detection.
- Save serial choices per program in library format 19; preserve older profiles.
- Retain the light-grey sidebar, original-grey joystick test and oval directions.

## 0.21.2-dev — Joystick button choices

- Align the gameport, tester and key-binding panels to the same width.
- Use smaller round indicators with labels below in the live joystick tester.
- Save a 1-6 physical-button choice per program and display that many buttons
  in the live tester. Buttons 3-6 use compatible gamepad colour/shoulder inputs.
- Keep buttons 1-2 on the DOS gameport; map buttons 3-6 to selectable DOS keys.
  Duplicate bindings and simultaneous keyboard holds retain independent releases.
- Profile format 18 migrates earlier libraries with the previous two-button
  behavior. Expanded runtime 0.21.2 adds host input handling; ARM core unchanged.

## 0.21.1-dev — I/O and Joystick pages

- Move COM1 TCP/modem settings from Network to a dedicated I/O sidebar page.
- Add a Joystick page with per-program gameport enable, live direction and
  Fire/Button 2 indicators, and X/Y values.
- Poll physical Amiga port 2 (lowlevel index 1) only while the tester is open;
  release the timer and library when leaving or closing the launcher.
- Preserve profile format 17 and the existing runtime/device behavior.

## 0.21.0-dev — Expanded hardware candidate

- Optional 386/486/Pentium CPU models and up to 32 MiB guest RAM.
- Auto/Max cycle control, VCPI/UMB and a two-axis joystick gameport.
- GUS, Game Blaster/CMS and Tandy sound devices through AHI.
- COM1 UART with outbound TCP and basic modem commands.
- Experimental register-only paging JIT, disabled by default.
- CPU/Audio/Network settings and dosbox.conf controls; profile format 17
  preserves existing profiles. Expanded runtime uses a 64 MiB owned allocation.
- Development candidate: compatibility and performance claims require separate
  hardware results. See `docs/amiga-dosbox-expanded-hardware.md` for limitations.

## 0.20.1-dev-compat-x87 — Experimental guest FPU

- Optional portable x87 in the compatibility runtime, including interpreter and ARM JIT helpers.
- Uses A9 VFPv3 double precision with preserved firmware floating-point state; does not require a 68k FPU.
- Native, relocated ARM-model and physical x87 fixtures pass. Full 80-bit precision is not provided.
- See the [x87 report](../../docs/amiga-dosbox-x87.md) for scope and game results.

## 0.20.1-dev — Audio choices and file dates

- Per-program Sound Blaster 8-bit (Pro 2), Sound Blaster 16-bit and independent PC speaker/beep controls in Audio.
- SB16 DMA 5 and IRQ 7, matching BLASTER environment; PIT speaker output through AHI.
- dosbox.conf supports `sbtype=none|sbpro2|sb16` and `[speaker] pcspeaker=false|true`.
- Profile format v16 preserves old libraries; sound and speaker settings persist separately.
- Remote DOS files report their host timestamps consistently on open and stat, fixing the stale-resource check seen in Eye of the Beholder III.
- Native game checks use fresh recursive copies, preserving nested game assets and preventing cross-game test contamination.

## Unreleased — runtime review cleanup

- Document ZZDosBox DHCP/manual IPv4 and IPX setup, with the supported subset
  of DOSBox-X networking and the absence of PCAP/raw Ethernet bridging.

- Share the three-buffer ownership plan through `amiga/common/flip_plan.h`.
- Synchronize only occupied console-ring spans across PC, shell and console hosts.
  Preserve consumption only after successful output; skip empty payload ranges.
- Drop/count oversized receive packets and continue to the next fitting packet.
- Document the unused fourth byte in scanline pixel formats.
- Add sanitizer tests for ring publication, wrapping and receive recovery.
- RAM-only hardware correctness check passed; no speed gain is established.

<a id="v0-20-0"></a>
## 0.20.0-dev — memory and audio compatibility

- Embedded EMS 4.0 and XMS share the guest-memory pool.
- Sound Blaster Pro 2, DMA/IRQ and DBOPL on ARM; batched 32 kHz stereo through AHI.
- Per-program EMS and sound-card controls, with versioned library migration.
- Dedicated Compat runtime preserves existing Windows, networking and Developer builds.
- Native guest-level memory, DSP/DMA/IRQ and FM tests pass; hardware results are
  tracked in the memory/audio guide. This is not a released compatibility claim.


<a id="v0-19-12"></a>
## 0.19.12-dev — Library screenshots

- Main window title bar has no text.
- Screenshot previews sit beside All, Games, Applications and OS lists.
- Capture launches the selected program. Ctrl+F5 saves a completed display frame;
  F10 returns to the library. A new capture replaces that entry's previous picture.
- The loading screen lists Ctrl+F5 alongside fullscreen, exit and CPU controls.
- DOS welcome uses a yellow ASCII title, current launcher version and Ctrl+F5
  instructions, without a GitHub URL. Known older welcome helpers upgrade on launch.
- Pictures stay local in Snapshots, retain their association after Rename, and
  are refreshed when the player exits. Capturing briefly pauses presentation.
- Capture uses the existing owned bitmap after rendering and flip completion;
  the display lock is released before writing the PPM file.

<a id="v0-19-11"></a>
## 0.19.11-dev — Loading and library controls

- Loading stays visible while the player starts, until its display window opens;
  it also closes if startup fails. The launcher sleeps during normal gameplay.
- Rename changes library titles. Four compact category buttons include Delete.
- Delete removes dedicated game/application folders and their saves after a
  Yes/No confirmation (No by default). OS entries/images and shared installations
  are protected; shared-image apps require their own uninstaller.
- Successfully mounted raw hard disk images appear in OS, reusing existing
  profiles. Bootable images must be ejected from Workbench before launching.
- The launcher revision is always visible at the bottom right. Delete help
  appears on hover at the bottom left, restoring the status when the pointer leaves.
- Hard-disk actions sit inside their frame; Disks explains IMG versus VHD.

<a id="v0-19-10"></a>
## 0.19.10-dev

- Up/Down selects entries in the focused All, Games, Applications or OS list.
- Letter and number keys jump to the first matching title, ignoring case.
- Program lists join the Tab focus order; changing category focuses its list.
- Text fields and sidebar retain their own keyboard controls.
- About includes a scrollable version history generated from this changelog.
- Replace the sidebar text block with the embedded ZZDosBox logo.

<a id="v0-19-9"></a>
## 0.19.9-dev

- Keep an explicit Display-page note visible: scanlines and Show FPS cannot be used together.
- Clarify the fullscreen requirement and disabled selector in the feature documentation.
- Help text only; rendering behaviour and saved profile format are unchanged.

<a id="v0-19-8"></a>

## 0.19.8-dev — offline Network guide

- Add a Network guide button below the Network page's subtabs.
- Open the bundled AmigaGuide in MultiView, including during a network session.
- Generate linked help sections from the same Markdown master as the web guide.
- Keep network settings and program profiles unchanged.

<a id="v0-19-7"></a>

## 0.19.7-dev — create hard disks

- Create 32/64/128/256 MiB FAT16 raw images with startup files and OS profiles.
- Default to embedded DOSBox DOS 5; optionally use installed MS-DOS 6.22 files.
- Mount DOS 5 images as C: for a session and eject on return.

<a id="v0-19-6"></a>

## 0.19.6-dev — disk browsing and settings help

- Select, mount, open and eject supported raw FAT images from the Disks page.
- Explain scanline availability and related display options beside settings.

<a id="v0-19-5"></a>

## 0.19.5-dev — larger libraries

- Increase saved-program capacity from 128 to 256 profiles.
- Preserve existing categories, launch settings and selected entry; no ARM runtime change.

<a id="v0-19-4"></a>

## 0.19.4-dev — library capacity

- Increase saved-program capacity from 32 to 128 profiles.
- Validate full-capacity save/load, category indexes and excess-entry rejection.

<a id="v0-19-3"></a>

## 0.19.3-dev — ZZDosBox identity

- Add the ZZDosBox box logo, with prominent ZZ lettering alongside DOS/BOX.
- Use the same artwork for startup/loading splashes and Workbench icons.

<a id="v0-19-2"></a>

## 0.19.2-dev — startup and loading splashes

- Show the DOSBox logo and launcher version at startup.
- Continue automatically after two seconds, or dismiss the panel with a click.
- Embed the artwork and share the version string with About.
- Show a loading card before games/apps with fullscreen, exit and CPU-cycle shortcuts.

<a id="v0-19-1"></a>

## 0.19.1-dev — launcher presentation

- Add readable proportional text, aligned icon/text navigation columns and framed page headings.
- Standardize button spacing and settings groups while retaining the sidebar layout.
- Align Screen and Rendering panel edges, label columns and controls.
- Keep native MUI colour preferences and font fallbacks; no extra MUI classes.

<a id="v0-19-0"></a>

## 0.19.0-dev — sidebar navigation

- Replace main tabs with a native MUI sidebar and selected-program heading.
- Split Settings into CPU & memory, Display and Audio; consolidate window/fullscreen controls under Display.
- Keep Save settings visible and preserve edits while navigating.
- Synchronize category Edit and live Network/Developer sessions with the sidebar.
- Retain MUI 3.8 support and existing profile/session formats.

<a id="v0-18-0"></a>

## 0.18.0-dev — IPX wrapper client

- Add TCP/IP / IPX wrapper (UDP) selection and per-program server IPv4/port.
- Port the pinned DOSBox IPX driver: discovery, sockets, ECB send/receive, ESR callbacks and IPXNET commands.
- Carry IPX packets over bounded queues to a nonblocking Roadshow UDP socket.
- Validate packet lengths and peer source; retain TCP/IP settings when switching protocols.
- Upgrade profiles to version 13 while reading previous network records.
- Verify IPX sockets, broadcast/reply callbacks, peer ping and clean disconnect on the A4000TX; retain passing TCP/IP and CD regression checks.
- Client mode only; an external DOSBox-compatible relay is required. Windows IPX/SPX, Novell drivers and relay hosting are not implemented.

<a id="v0-17-0"></a>

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

<a id="v0-16-0"></a>

## 0.16.0-dev — on-demand reports and Windows disk cache

- Load selected compatibility details from `Compatibility.dat`; reduce the launcher executable by 73.3%.
- Validate data headers, indexes, record bounds and checksums; recover when a missing file is restored.
- Add configurable build-time BIOS image cache capacity. The measured 1 MiB Windows candidate cuts startup elapsed time by 14.3% versus 128 KiB.
- Limit CAMD cache maintenance to consumed MIDI spans while preserving pacing and MT-32 SysEx delays.
- Preserve SIMD/FPSCR state in scalar PC builds too: linked library math can use VFP independently of pixel settings.
- Add paging/remapping/fault regression fixtures. Keep the production paged-JIT fallback.
- Add acknowledgement counters and elapsed time to fullscreen failure diagnostics.
- Keep indexed-pixel capture unrolling experimental; a failed fullscreen run is excluded from performance results.

<a id="v0-15-2"></a>

## 0.15.2-dev — complete-discussion topic overviews

- Index all 6,999 cached comments across 2,428 discussions, including older replies.
- Show contributor counts, date range and automatic topic counts in the right pane.
- Count unclassified comments explicitly; topic mentions are not verified fixes.
- Keep the latest excerpt separately labelled and retain original-report links.
- Expand the scrolling discussion area and bundle updated Markdown details.

<a id="v0-15-1"></a>

## 0.15.1-dev — offline compatibility reference

- Add Compatibility to the MUI launcher: A–Z/0–9 filters, title search and report links.
- Place games on the left and per-game details on the right: aligned metadata,
  a DOSBox version/status/rating table, and a separate scrolling comment preview.
- Include 3,937 upstream DOSBox reports covering 3,932 games, with tested versions and ratings.
- Include per-game publisher/tester details, 8,709 recorded version ratings and 2,428 brief comment previews.
- Bundle summary and detailed Markdown references; keep CSV/JSON exports and reproducible scrapers.
- Clearly distinguish upstream reports from verified ZZDosBox results.

<a id="v0-15-0"></a>

## 0.15.0-dev — initial compatibility browser

- Add the offline DOSBox compatibility index, title search and alphabet filters.
- Retain links to upstream reports; these are community reports, not ZZDosBox test results.
- Superseded by the right-pane details and tables in 0.15.1.

<a id="v0-14-0"></a>

## 0.14.0-dev — dosbox.conf editor

- Add Developer → dosbox.conf with 29 validated global overrides, defaults, import/export and previous-file backup.
- Select interpreter or dynamic core and exact CPU cycles without rewriting program profiles.
- Route explicit dynamic disk-image launches to a separate guarded runtime; paging still falls back to interpretation.

<a id="v0-13-0"></a>

## 0.13.0-dev — program categories

- Group All, Games, Applications and OS under Load Program.
- Add a saved category selector with migration of existing profiles and category filtering.

<a id="v0-12-0"></a>

## 0.12.0-dev — memory development tools

- Add paused guest-RAM hex/ASCII viewing, 8/16/32-bit edits and undo.
- Add value scanning/refinement and 16/32-bit x86 disassembly.
- Use separate diagnostic runtimes with cooperative pause/resume; these tools are not host-physical-memory access.

<a id="v0-11-1"></a>

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

<a id="v0-11-0"></a>

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

<a id="v0-10-3"></a>

## 0.10.3-dev launcher — 13 October 2026

- Added Normal / Adaptive presentation to Settings, saved per program; Normal default.
- Installed the optional adaptive runtime and Off/Classic/Soft/Gradient scanline choices.
- Preserved existing profiles; verified migration, save/reopen and physical launch/exit.
- Verified the low-stack guard and 64 KiB Workbench icon stack.

<a id="v0-10-1"></a>

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

<a id="v0-10-0"></a>

## 0.10.0-dev — startup files, CD and MIDI

- Group AUTOEXEC.BAT, CONFIG.SYS and CD controls in DOS terminal.
- Add startup-file selection/editing and restoration of temporary booted-DOS edits.
- Add read-only ISO/mounted-CD access and the booted-DOS CD gateway.
- Add external intelligent/UART MPU-401 transport through CAMD and audio-driver discovery; PCM remains unavailable.
- Initial integration runtime; 0.10.1 adds the welcome screen and live display/cycle controls.

<a id="v0-9-1"></a>

## 0.9.1-dev — 13 October 2026

- Added 320×240, 640×480 and 960×720 window presets on the DOS terminal tab.
- Added Workbench resize handling with centred 4:3 output and black margins.
- Moved Show FPS to Settings; preserved prior profile formats.

<a id="v0-9-0"></a>

## 0.9.0-dev — 13 October 2026

- Added Workbench window output alongside fullscreen operation.
- Added embedded DOS 5 / installed MS-DOS 6.22 selection.
- Added the optional ARM-rendered display-refresh counter.

<a id="v0-8-0"></a>

## 0.8.0-dev — 13 October 2026

- Added writable 32 MB BIOS hard-disk images and Workbench mount/eject controls.
- Verified MS-DOS boot, persistent files and program-profile launches.
- Recorded Keen startup throughput averaging 37.2 refreshes/sec across three
  runs; this was a timed startup test, not a full-speed gameplay claim.

<a id="v0-7-0"></a>

## 0.7.0-dev — 12 October 2026

- Added nested DOS directories and bounded host-file access.
- Added read-only 1.44 MB BIOS floppy boot and guest RAM choices up to 8 MB.
- Introduced the banked TLB to fit the interpreter within its owned allocation.
- Compared selected behaviour with DOSBox-X; retained dosbox-libretro as the
  source base.

<a id="v0-6-0"></a>

## 0.6.0-dev — 12 October 2026

- Added the MUI program launcher, profiles, LOADFIX option, startup-file editors,
  terminal colours, video choices and CPU-cycle presets.
- Added live keyboard input and sessions that run until closed.

<a id="v0-5-0"></a>

## 0.5.0-dev — PC devices and direct graphics

- Add PC BIOS, PIT and keyboard devices with CGA/EGA/VGA display integration.
- Render on ARM into owned ZZ9000 buffers and coordinate presentation through the 68K.
- Optimize host polling, scaling and pixel conversion through measured candidates; initially timed execution without live gameplay input.

<a id="v0-4-0"></a>

## 0.4.0-dev — DOS shell and live files

- Integrate the upstream DOS shell and live writable C: drawer.
- Support shell commands, COM/EXE execution and bounded host file requests.
- Use LOADFIX for programs requiring a different conventional-memory load address.

<a id="v0-3-0"></a>

## 0.3.0-dev — interactive console

- Add streaming text input/output between ARM and an Amiga console.
- Use bounded shared rings and cooperative stop while waiting for input or output.

<a id="v0-2-0"></a>

## 0.2.0-dev — DOS kernel

- Integrate upstream DOS services, COM/MZ loading and process/memory management.
- Stage a private flat C: drive, execute programs and return file changes to the host.
- This precedes the live host-backed filesystem and interactive shell.

<a id="v0-1-0"></a>

## 0.1.0-dev — first ARM execution prototype

- Run the upstream x86 interpreter on application-owned Cortex-A9 Core1.
- Verify an original arithmetic/memory diagnostic and minimal text/exit services.
- Establish relocation, cancellation, processor-state restoration and owned-memory release; no full DOS shell or PC graphics yet.

# Unversioned runtime and research milestones

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

## Direct-target OpenGL experiment — 14 October 2026 (not installed)

- Render eligible 640×480 frames into the validated XACP back buffer; retain
  the existing path for resized windows and scanline masks.
- Measured 5.06 FPS OpenGL versus 6.94 FPS Direct in Doom Demo3. Previous GL
  measured 4.72 FPS; the historical comparison suggests about 7% improvement.
- A subsequent loop refinement passed software tests but the Amiga froze
  during its first hardware test. Withdrawn; exact cause remains unresolved.
- Source restored to the first tested variant. Installed runtimes unchanged.
- Keep Settings choices Direct / OpenGL (experimental), saved per program.

[Test and failure record](../records/2026-10-14/dosbox-gl-direct/method.md).

## Adaptive presentation experiment (now optional in MUI)

- Optional fixed or adaptive frame presentation; guest CPU/timing stays unchanged.
- Counts generated, displayed and skipped frames separately; FPS overlay follows presentation count.
- Separate experimental executable; default remains Off. Physical Doom Demo3: 7.27 to 8.25 real FPS (+13.5%) in a matched pair with profiling disabled. Five complete timedemos passed cleanup; visual smoothness remains unassessed.




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
