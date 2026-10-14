# ZZDosBox features and compatibility

This describes the current **Direct development runtime** and the separate
**Windows/mouse development build**; individual options depend on the chosen build.
It is the implemented Amiga feature set, not the complete feature list of
[upstream dosbox-libretro](https://github.com/libretro/dosbox-libretro).

## Launcher and controls

| Feature | Support |
| --- | --- |
| Native interface | MUI launcher with Programs, DOS terminal, Settings, Disks and About pages. |
| Program library | Up to 32 saved profiles, each with its own program, C: drawer and emulator settings. |
| Compatibility helper | Optional LOADFIX per profile for programs needing a different conventional-memory load address. |
| DOS choice | Embedded DOS 5 or booted, separately supplied MS-DOS 6.22. |
| Startup editing | AUTOEXEC.BAT and CONFIG.SYS tabs with Default / Edited / File choices, line editing and import/export. |
| Startup recovery | Temporary MS-DOS startup-file changes preserve originals and recover an interrupted update. |
| Sessions | Timed startup tests or normal operation until closed. Keyboard press/release input is forwarded to the emulated PC. |
| Mouse | DOS INT33 and BIOS PS/2 input; three buttons, scaled window/fullscreen coordinates, release on focus loss or display switch. |
| Live shortcuts | Alt+Return: window/fullscreen. Ctrl+minus/plus: cycle adjustment. F10: close player and return to AmigaOS. |
| DOS appearance | Foreground/background colour selection and a versioned DOS welcome screen. |

CONFIG.SYS is an editor/reference in embedded DOS; that environment does not
load its device drivers. Booted MS-DOS processes the files on its own disk.

## Graphics

| Feature | Support |
| --- | --- |
| PC adapters | VGA, EGA, CGA, S3/VESA, Tandy, PCjr and Hercules selections. Broader high-resolution compatibility needs testing. |
| Fullscreen | Dedicated 640×480 true-colour RTG screen with XACP buffer presentation. |
| Workbench window | ARM rendering into an owned ZZ9000 bitmap, followed by an AmigaOS-clipped blit into the window. |
| Window sizes | 320×240, 640×480 and 960×720 presets. Native resize handling within those limits. |
| Aspect ratio | Centred 4:3 output with black margins when the window shape differs. |
| Display switching | Change window/fullscreen while running. Both modes and repeated switches have passed physical tests. |
| FPS overlay | Optional ARM-rendered counter in Settings. Measures completed display refreshes, not unique game frames or emulated CPU MHz. |

Presets and changing render geometry are tested. Physical drag-resizing has
not yet been verified; it should not be presented as a completed hardware test.

## CPU and memory

| Feature | Support |
| --- | --- |
| Execution | 32-bit ARM Cortex-A9 Core1 runs 386-class x86 emulation. Current Direct builds use ARMv7 dynamic recompilation; the separate Windows build retains the interpreter. The 68K handles AmigaOS services. |
| Real mode | DOS shell, COM/EXE programs and BIOS-based DOS boot. |
| Protected mode | The Windows build includes 386 paging, nested page-fault recovery and sparse high-address mappings. Verified with Windows 3.11 and an original page-fault test. |
| A20 and ROM | Windows build implements fast-A20 port 0x92, proper initial aliasing, and write protection for BIOS/VGA ROM. |
| Guest RAM | 1, 2, 4 or 8 MB, within the fixed 32 MB owned worker allocation. |
| CPU tuning | 1000/3000/6000 cycles/ms presets; live adjustment from 500 to 10000 in 500-cycle steps. These values are not MHz ratings. |
| Extended memory | Booted DOS can use its HIMEM driver. Embedded XMS/EMS, EMM386 and DOS=HIGH are not supported by the tested setup. |
| Worker ownership | One application owns Core1. Runtime mapping and buffer checks, bounded memory, cancellation and register/cache restoration protect the application's lifecycle. |

Protected-mode support does not establish compatibility with every DOS extender
or Windows application. The current Direct runtime enables ARMv7 dynarec/JIT. No x87 FPU or
64-bit x86 guest support is enabled. The owned allocation and checks are not a general security sandbox
for untrusted programs.

## Files and media

| Feature | Support |
| --- | --- |
| Shared C: drawer | Read/write access to an Amiga directory, including nested directories and the port's restricted DOS 8.3 names. |
| Game saves | Normal files written by the game persist. Emulator snapshots/save states are not implemented. |
| Floppy boot | Read-only 1.44 MB BIOS disk images. |
| Hard-disk boot | Writable 32 MB BIOS disk image, with bounds checking and persistent writes. |
| Workbench access | Mount/eject controls using diskimage.device and fat95. Eject before the emulator takes writable ownership. |
| Data CDs | Read-only ISO9660 images or an already-mounted Amiga data CD, exposed as D:. |
| MS-DOS CD driver | Original ZZDOSCD.SYS gateway with the user's licensed MSCDEX.EXE. |

CUE/BIN mounting, audio-CD tracks, IDE/AHCI controller emulation and hot CD
changes are not implemented. BIN media can be prepared into supported data
files/ISO externally; that conversion is not a launcher feature.

## MIDI and audio

- External MT-32 through intelligent MPU-401 emulation and CAMD.
- External General MIDI through MPU-401 UART mode and CAMD.
- Selectable CAMD output, bounded message transport and SysEx handling.
- Driver discovery for CAMD, AHI and ZZ9000AX in Settings.

CAMD capture passed on hardware. Audible external-instrument playback remains
untested. Sound Blaster, AdLib/OPL, PC speaker PCM, AHI playback and ZZ9000AX/HDMI
PCM output are not implemented. Munt and SoundFont synthesizers are not bundled.

## Verified compatibility

| Workload | What has been verified |
| --- | --- |
| Embedded DOS / MS-DOS 6.22 | Boot or shell operation, file persistence, keyboard input and clean return. |
| Windows for Workgroups 3.11 | Desktop, WinBench 3.11 Graphics WinMark and normal shutdown on the real A9, using standard VGA and 8 MB guest RAM. |
| WinBench | Graphics WinMark **1,923,752 pixels/sec**. Uses the guest clock; no equivalent physical PC speed is established. |
| Commander Keen 6 | Startup and map movement; timed startup throughput recorded. |
| Oregon Trail Deluxe | Title/menu and mouse-operated Options page on the Amiga, using the 16-colour VGA option. Full-game compatibility is not established. |
| Vette / Elite | Startup with LOADFIX and return to Workbench. Full-game compatibility is not established. |

Installing a game does not make it a verified compatibility result. Joystick,
guest networking and Windows audio remain unavailable. The mouse build has
passed native and ARM tests; Oregon Trail menu interaction is verified on the
Amiga. Windows pointer movement and double-click launch of Write also passed
on hardware using the Microsoft/IBM PS/2 driver. Pointer capture/warping for games requiring continuous relative movement is
not implemented. Third-party DOS mouse-driver compatibility is not fully tested.
