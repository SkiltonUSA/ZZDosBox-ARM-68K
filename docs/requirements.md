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
| 68K CPU | Executables target the 68020 instruction set. This is a build baseline, not a tested minimum-speed claim. A TF4060 is not explicitly required by the code. |
| AmigaOS | Tested with AmigaOS 3.2.3. The launcher requires `workbench.library` v44+, `asl.library` v38+ and the MUI library. Compatibility with older OS installations is unverified. |
| ZZ9000 firmware | Tested **XX19c / XACP 1.7**. Later firmware has not yet been tested here. Close other applications using ARM Core1 before starting. |
| ZZ9000 RAM | Enable the card's **256 MB Zorro III Fast RAM**. The player needs a **32 MB contiguous free block** in that RAM, plus memory for AmigaOS, the launcher and display buffers. |
| Graphics | Working ZZ9000 Picasso96 driver and `Picasso96API.library` v2+. Window mode requires a supported **32-bit RTG Workbench**; fullscreen requires an existing **640×480 32-bit RTG mode**. The guest can still use CGA/EGA or 16-colour VGA. |
| MUI | **MUI 3.8 or later** for the launcher. No additional custom MUI classes are required. |
| Stack and storage | Use a **65536-byte stack** for the player/launcher and a writable application drawer. Allow space for your programs and any disk images. |

Guest PC RAM is selectable as **1, 2, 4 or 8 MB**. It is included within the
32 MB worker allocation; it is not an extra 8 MB allocation or a limit on the
Amiga's total RAM. The current Windows test uses 8 MB guest RAM.

## MIDI, audio and optional components

| Component | When it is needed |
| --- | --- |
| **CAMD** | External MIDI only. Requires `camd.library` **v37+**, a working CAMD driver/output, and a connected MIDI synthesizer. Select the actual output name in Settings. Leave MIDI Off when unused. |
| **Roland MT-32 / General MIDI** | External instruments: MT-32 selects intelligent MPU-401 emulation; General MIDI selects UART mode. ZZDosBox does not currently include Munt, SoundFonts or an internal MIDI synthesizer. |
| **AHI** | Optional **driver detection only** in this build. The checker supports AHI v4+. Installing AHI does not enable PCM playback. |
| **ZZ9000AX** | Optional; its AHI modes can be detected. It is not required to run DOSBox, and ZZDosBox does not yet send PCM audio to it or HDMI. |
| **diskimage.device + fat95** | Needed for Workbench mounting of DOS disk images and the launcher's MS-DOS startup-file editing workflow. Tested with Thore Boeckelmann's `diskimage.device` 37.7, `DiskImageCtrl` and `L:fat95`. Direct BIOS image boot itself does not use these drivers. |
| **CD-ROM** | Read-only ISO9660 image, or a data CD already mounted by AmigaOS. Booted MS-DOS additionally uses the supplied `ZZDOSCD.SYS` and your licensed `MSCDEX.EXE`. A USB drive needs its normal Amiga USB/storage/CD configuration. |

Settings → **Check audio drivers** checks CAMD and the selected output, AHI,
and named ZZ9000AX modes. A positive result is driver discovery, not an audible
playback test. CAMD message delivery has been captured on hardware; external
MT-32 sound has not yet been verified in this project.

Sound Blaster, AdLib/OPL and PC-speaker PCM audio remain unimplemented.

## DOS, Windows and storage

- Embedded DOS 5 does not need Microsoft DOS disks. Programs can use a writable
  Amiga drawer as C:, with subdirectories and the port's restricted DOS 8.3 names.
- Booted MS-DOS 6.22 needs your own media. The current BIOS hard-disk layout is
  32 MB; read-only 1.44 MB floppy boot is also supported.
- Eject an image from Workbench before booting it in DOSBox. Do not mount one
  writable image in both systems at once.
- The Windows development build has completed Windows for Workgroups 3.11
  startup, WinBench 3.11 Graphics WinMark and shutdown on the A9. It uses standard
  VGA and keyboard input. Windows and WinBench must be supplied separately.
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
| Other features | Joystick, emulator save states, guest networking, CD audio/CUE and Voodoo emulation remain unavailable. |

Upstream host build instructions and key bindings are not ZZDosBox installation
instructions. Use the Amiga controls: **Alt+Return** changes window/fullscreen,
**Ctrl+minus/plus** changes cycles, and **F10** closes the player. Exit Windows
normally to DOS before closing the emulator.

The mouse build uses the normal Amiga pointer. Windows 3.1 needs its Microsoft/IBM PS/2 mouse driver selected; it was previously installed with no mouse. No extra Amiga mouse library is required.
