# ZZDosBox — development prototype

DOSBox for the Amiga and ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

**0.10.1-dev** includes a MUI launcher, program profiles, optional LOADFIX,
selectable/editable AUTOEXEC.BAT and CONFIG.SYS, terminal colours, 1–8 MB guest RAM and VGA/EGA/CGA.
Choose a Workbench window or fullscreen, embedded DOS 5 or installed MS-DOS
6.22 on the DOS terminal tab, with an optional ARM-rendered FPS counter in Settings.
Window sizes are 320×240, 640×480 and 960×720; the Workbench resize gadget
adjusts the window between these limits, keeping the picture at 4:3.
A coloured DOS welcome panel shows the version, project link and controls.
Alt+Return switches window/fullscreen; Ctrl+minus/plus adjusts CPU cycles.
The DOS terminal also accepts ISO images or a mounted Amiga data CD as D:.
Settings includes external MT-32/General MIDI through CAMD and a CAMD/AHI/
ZZ9000AX driver check. MIDI needs CAMD and a connected external synthesizer.

Requires **MUI 3.8 or later** and XACP on the ZZ9000; tested with XX19c firmware.
AHI and ZZ9000AX are currently detected only; PCM output is not implemented.

[Features and compatibility](../../docs/features.md) ·
[Amiga requirements](../../docs/requirements.md) · [Changelog](CHANGELOG.md)

The A9 runs the x86 interpreter and draws directly into the ZZ9000 framebuffer;
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

Credits: DOSBox team, libretro contributors, MUI by Stefan Stuntz, and Xani's
[XACP](https://github.com/Xanxi-Amiga/XACP-ZZ9000) platform.

Build and test notes are in [the development record](../../docs/amiga-dosbox-feasibility.md).
