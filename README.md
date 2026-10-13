# ZZDosBox-ARM-68K

DOSBox for the Amiga and ZZ9000 Cortex-A9, based on
[dosbox-libretro](https://github.com/libretro/dosbox-libretro).

**0.8.0-dev** includes a MUI launcher, program profiles, optional LOADFIX,
editable AUTOEXEC.BAT, terminal colours, 1–8 MB guest RAM and VGA/EGA/CGA.
Requires **MUI 3.8 or later** and XACP on the ZZ9000; tested with XX19c firmware.

The A9 runs the x86 interpreter and draws directly into the ZZ9000 framebuffer;
the Amiga handles files, input and display coordination. A writable Amiga
drawer serves as DOS drive C:, including subdirectories. Read-only 1.44 MB
floppy boot and a writable 32 MB BIOS hard disk are also available.
The Disks tab mounts the hard disk in Workbench for file transfers; eject it
before booting DOS. Games and MS-DOS disks are not included.

Hard-disk boot, file persistence and Workbench file access have been tested
on the A4000TX.

The current build averaged **37.2 FPS** across three timed Keen startup tests
on the A4000TX. Eight emulated seconds took 15 real seconds: this is not full
speed or a steady gameplay measurement. Keen map movement, Vette startup,
Elite launch and return to Workbench have been checked.

Current limits: restricted DOS 8.3 names, no audio, mouse or joystick. Embedded DOS has no XMS/EMS driver; CONFIG.SYS is an
editor/export reference there. Booted MS-DOS runs the configuration and
drivers on its disk and can use its own HIMEM driver. `DOS=HIGH` and EMM386
are not supported in the supplied configuration.

Credits: DOSBox team, libretro contributors, MUI by Stefan Stuntz, and Xani's
[XACP](https://github.com/Xanxi-Amiga/XACP-ZZ9000) platform.

Build and test notes are in [the development record](docs/amiga-dosbox-feasibility.md).

Current executable and launcher: [0.8.0-dev backup](artifacts/0.8.0-dev).
[Build and restore](docs/build-and-restore.md) · [Test records](amiga/records).
