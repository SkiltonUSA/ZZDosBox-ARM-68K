# ZZDosBox requirements

ZZDosBox adapts the DOSBox engine from
[dosbox-libretro](https://github.com/libretro/dosbox-libretro), pinned at
[`a563bb5`](https://github.com/libretro/dosbox-libretro/tree/a563bb5e9753f207d74e9d8c417d793b784fa4be).
It uses an Amiga MUI launcher and XACP ARM worker. RetroArch, SDL/SDL2, a Mac
and Amiga-Bridge are not required to run the supplied Amiga executable.

## Amiga hardware and software

| Component | Requirement |
| --- | --- |
| Amiga | A Zorro III Amiga with a ZZ9000. Physically tested on an A4000TX with TF4060/68060; other CPU configurations have not been validated by this project. |
| 68K CPU | Executables target the 68020 instruction set. This is a build baseline, not a tested minimum-speed claim. No TF4060 or 68060 accelerator is required. |
| AmigaOS | Tested with AmigaOS 3.2.3. The launcher requires `workbench.library` v44+, `asl.library` v38+ and the MUI library. Compatibility with older OS installations is unverified. |
| ZZ9000 firmware | Tested **XX19c / XACP 1.7**. Later firmware has not yet been tested here. Close other applications using ARM Core1 before starting. Current PC builds require firmware-permitted SIMD access even with scalar pixels, because linked library routines can use VFP/NEON; the worker checks permission and preserves the state. |
| ZZ9000 RAM | Enable the card's **256 MB Zorro III Fast RAM**. Compat and Net-JIT need a **32 MiB contiguous free block**; Expanded needs **64 MiB** in that RAM, plus memory for AmigaOS, the launcher and display buffers. |
| Graphics | Working ZZ9000 Picasso96 driver and `Picasso96API.library` v2+. Window mode requires a supported **32-bit RTG Workbench**; fullscreen requires an existing **640×480 32-bit RTG mode**. The guest can still use CGA/EGA or 16-colour VGA. |
| Compatibility browser | Keep the matching `Compatibility.dat` beside `ZZDosBox-Launcher` (0.16.0-dev onward). Missing data disables report details, not the emulator. |
| MUI | **MUI 3.8 or later** for the launcher. No additional custom MUI classes are required. |
| Stack and storage | Use a **65536-byte stack** for the player/launcher and a writable application drawer. Allow space for your programs and any disk images. |

Guest PC RAM is selectable from **1–8 MiB**, or up to **32 MiB with Expanded**.
It is included in the runtime allocation, not additional to it. The Windows
test configuration uses 8 MiB guest RAM.

A faster 68k can improve file and disk-image access, input, networking and
Workbench window updates. CPU emulation runs on the ZZ9000 Cortex-A9.
The TF4060 identifies the test system, not a required accelerator.
A 68030 is a candidate for validation; this project has not established its
minimum practical clock speed or measured these builds on it.

## MIDI, audio and optional components

| Component | When it is needed |
| --- | --- |
| **CAMD** | External MIDI only. Requires `camd.library` **v37+**, a working CAMD driver/output, and a connected MIDI synthesizer. Select the actual output name in Settings. Leave MIDI Off when unused. |
| **Roland MT-32 / General MIDI** | External instruments: MT-32 selects intelligent MPU-401 emulation; General MIDI selects UART mode. ZZDosBox does not currently include Munt, SoundFonts or an internal MIDI synthesizer. |
| **AHI** | AHI v4+ with unit 0 configured for PCM output. Required when enabling SB Pro 2, SB16, OPL, PC speaker or Expanded audio devices. Output is 32 kHz stereo. |
| **ZZ9000AX** | Optional AHI output hardware; selected through AHI preferences. ZZDosBox does not require AX and has no direct HDMI-audio path. |
| **diskimage.device + fat95** | Needed for Workbench mounting of DOS disk images and the launcher's MS-DOS startup-file editing workflow. Tested with Thore Boeckelmann's `diskimage.device` 37.7, `DiskImageCtrl` and `L:fat95`. Direct BIOS image boot itself does not use these drivers. |
| **lowlevel.library V40** | Joystick input and the live direction/button tester on Amiga port 2. |
| **Serial driver** | Experimental physical COM1 requires an IOExtSer-compatible built-in, USB or expansion driver. Poseidon/USB-driver discovery enables the USB shortcut; no driver is bundled. |
| **CD-ROM** | Read-only ISO9660 image, or a data CD already mounted by AmigaOS. Booted MS-DOS additionally uses the supplied `ZZDOSCD.SYS` and your licensed `MSCDEX.EXE`. A USB drive needs its normal Amiga USB/storage/CD configuration. |

Settings → **Check audio drivers** checks CAMD and the selected output, AHI,
and named ZZ9000AX modes. A positive result is driver discovery, not an audible
playback test. CAMD message delivery has been captured on hardware; external
MT-32 sound has not yet been verified in this project.

SB Pro 2, SB16, OPL and PC speaker are available in Compat, Expanded and Net-JIT.
Booted DOS/Windows audio remains unavailable. GUS/CMS/Tandy require Expanded.
See [I/O and joystick specifications](amiga-dosbox-io.md).

## DOS, Windows and storage

- Embedded DOS 5 does not need Microsoft DOS disks. Programs can use a writable
  Amiga drawer as C:, with subdirectories and the port's restricted DOS 8.3 names.
- Booted MS-DOS 6.22 needs your own media. Supported BIOS hard-disk images are
  32–256 MiB; read-only 1.44 MB floppy boot is also supported.
- **Disks → New hard disk** creates 32, 64, 128 or 256 MiB images.
  **DOSBox DOS 5** is the default and needs no Microsoft files. The launcher
  mounts this image as C: for its embedded DOS, then ejects it after the session.
  This mode requires diskimage.device and fat95; CONFIG.SYS drivers are not loaded.
  **MS-DOS 6.22** is offered when the installed DOS622.IMG exists and copies
  its boot code and DOS files into a separately bootable image. No Microsoft
  files are bundled. New images receive AUTOEXEC.BAT, CONFIG.SYS and an OS entry.
- The Disks page can mount and open raw MBR FAT hard-disk `.IMG` files
  (32–256 MiB) in Workbench. Microsoft `.VHD` containers are not supported.
- Eject an image from Workbench before booting it in DOSBox. Do not mount one
  writable image in both systems at once.
- The Windows development build has completed Windows for Workgroups 3.11
  startup, WinBench 3.11 Graphics WinMark and shutdown on the A9. It uses standard
  VGA, keyboard and PS/2 mouse input. Windows and WinBench must be supplied separately.
- Embedded DOS runs AUTOEXEC.BAT but does not load CONFIG.SYS device drivers.
  Booted DOS uses its disk configuration and can load HIMEM. EMM386 and DOS=HIGH
  are not supported by the tested configuration.

## Difference from upstream

The [upstream project](https://github.com/libretro/dosbox-libretro) supplies a
libretro core with desktop audio/synthesizer options and additional devices.
Those features are not automatically included in this Amiga port.

| Area | Current ZZDosBox implementation |
| --- | --- |
| CPU | 32-bit, 386-class x86 emulation on ARM Core1; current Direct runtime uses ARMv7 dynamic recompilation. The separate Windows build uses the interpreter. No x87 FPU. |
| Display | ARM renders into ZZ9000 buffers; 68K coordinates fullscreen presentation or Workbench window blits. |
| Frontend | Native MUI profiles and DOS terminal, rather than RetroArch core options. |
| MIDI | External CAMD output; upstream Munt/FluidSynth/BASSMIDI backends are not ported. |
| Other features | Expanded provides joystick and COM1. Emulator save states, CD audio/CUE and Voodoo emulation remain unavailable. |

Upstream host build instructions and key bindings are not ZZDosBox installation
instructions. Use the Amiga controls: **Alt+Return** changes window/fullscreen,
**Ctrl+minus/plus** changes cycles, and **F10** closes the player. Exit Windows
normally to DOS before closing the emulator.

The mouse build uses the normal Amiga pointer. Windows 3.1 needs its Microsoft/IBM PS/2 mouse driver selected; it was previously installed with no mouse. No extra Amiga mouse library is required.

The optional OpenGL experiment bundles TinyGL in the ARM payload; no separate
Amiga OpenGL library is required. It provides a partial software fixed-function
renderer, not GLSL support. MUI requirements remain unchanged.

## Experimental networking — TCP/IP and IPX

The Network page provides per-program TCP/IP or DOSBox-compatible IPX-over-UDP.
TCP/IP uses NE2000/libslirp NAT over the existing Roadshow connection, with
DHCP/manual IPv4, subnet, gateway, DNS, hostname and MTU controls. Diagnostics
include ping, DNS lookup, counters, logs and cooperative disconnect. IPX uses
a compatible UDP relay with address/port, connect/status/ping/disconnect controls.

Both require `bsdsocket.library`. Auto/dynamic core selects `ZZDosBox-Net-JIT`;
normal core uses `ZZDosBox-Net` with EMS/PCM off.
TCP/IP additionally needs staged NE2000/mTCP guest utilities. libslirp is linked
into the host; no separate Amiga installation is needed. Embedded DOS with
Direct/Normal presentation is supported; networking is off by default.
Expanded, scanline, OpenGL and debug-runtime combinations remain separate.
Windows/MS-DOS image networking, IPv6 and port forwarding remain pending.
IPX transport passed hardware checks; multiplayer games remain untested.
Renew obtains a fresh guest lease; Release clears saved guest configuration,
not the Amiga's address or an on-wire DHCP lease.

[Network controls, validation and limitations](amiga-dosbox-network-ui.md).
