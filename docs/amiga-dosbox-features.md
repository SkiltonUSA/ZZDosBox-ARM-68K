# ZZDosBox features and compatibility

This describes the **0.22.2-dev launcher and current development runtimes**, including the separate Windows runtime.
It is the implemented Amiga feature set, not the complete feature list of
[upstream dosbox-libretro](https://github.com/libretro/dosbox-libretro).

## Launcher and controls

| Feature | Support |
| --- | --- |
| Native interface | MUI sidebar navigation with Load Program, DOS terminal, display/audio settings, Disks, Network, I/O, Joystick, Compatibility, Developer and About pages. Load Program contains All, Games, Applications and OS lists, with saved category tags. |
| Configuration | Developer → dosbox.conf: validated editor for supported global overrides, including core selection and exact CPU cycles. [Settings reference](amiga-dosbox-conf.md). |
| Program library | Up to 256 saved profiles, each with its own program, C: drawer and emulator settings. Rename changes the library title. Delete removes a dedicated game/app folder and saves after Yes/No confirmation; OS and shared-image installations are protected. Focus the program list and use Up/Down to select; A–Z or 0–9 jumps to the first matching title. |
| Compatibility helper | Optional LOADFIX per profile for programs needing a different conventional-memory load address. |
| DOS choice | Embedded DOS 5 or booted, separately supplied MS-DOS 6.22. |
| Startup editing | AUTOEXEC.BAT and CONFIG.SYS tabs with Default / Edited / File choices, line editing and import/export. |
| Startup recovery | Temporary MS-DOS startup-file changes preserve originals and recover an interrupted update. |
| Sessions | Timed startup tests or normal operation until closed. Keyboard press/release input is forwarded to the emulated PC. |
| Mouse | DOS INT33 and BIOS PS/2 input; three buttons, scaled window/fullscreen coordinates, release on focus loss or display switch; same-button motion coalescing and re-entry reset. |
| Live shortcuts | Alt+Return: window/fullscreen. Ctrl+minus/plus: cycle adjustment. F10: close player and return to AmigaOS. |
| DOS appearance | Foreground/background colour selection and a versioned DOS welcome screen. |

CONFIG.SYS is an editor/reference in embedded DOS; that environment does not
load its device drivers. Booted MS-DOS processes the files on its own disk.

## Graphics

| Feature | Support |
| --- | --- |
| PC adapters | CGA, EGA, VGA, S3/VESA, Tandy, PCjr and Hercules selections, saved per program. |
| S3/VESA | S3 Trio register and VBE graphics with 1 MB emulated VRAM; banked and linear framebuffer access. Modes are bounded to 1024×768 and available VRAM. |
| Legacy adapters | Tandy/PCjr 16-colour graphics and Hercules monochrome graphics/text. Expanded provides optional Tandy sound; see the audio settings. |
| Fullscreen | Dedicated 640×480 true-colour RTG screen with XACP buffer presentation. |
| Workbench window | ARM rendering into an owned ZZ9000 bitmap, followed by an AmigaOS-clipped blit into the window. |
| Window sizes | 320×240, 640×480 and 960×720 presets. Native resize handling within those limits. |
| Aspect ratio | Centred 4:3 output with black margins when the window shape differs. |
| Display switching | Change window/fullscreen while running. Both modes and repeated switches have passed physical tests. |
| Presentation | Normal / Adaptive, saved per program. Adaptive skips final frame copies without changing guest execution or its selected CPU cycles. Normal is the default. |
| Software scanlines | Off / Classic / Soft / Gradient, saved per program; **requires Fullscreen and Show FPS off. Scanlines and the FPS overlay cannot be used together**; the scanline selector is greyed out when Show FPS is enabled or the display is windowed. Independent of the Workbench FPGA scanline controls. |
| FPS overlay | Optional ARM-rendered counter in Settings. Measures completed display refreshes, not unique game frames or emulated CPU MHz. |

S3 currently exposes BIOS/register discovery, without an emulated PCI bus.
Windows S3 display-driver compatibility and game performance at higher
resolutions need separate testing. Output remains scaled to the existing
Workbench window or 640×480 fullscreen display.

Presets and changing render geometry are tested. Physical drag-resizing has
not yet been verified; it should not be presented as a completed hardware test.

## CPU and memory

| Feature | Support |
| --- | --- |
| Execution | 32-bit ARM Cortex-A9 Core1 runs an ARMv7 x86 dynamic recompiler with a 4 MiB code cache and a 386-class CPU configuration. The 68K handles AmigaOS services. |
| Real mode | DOS shell, COM/EXE programs and BIOS-based DOS boot. |
| Protected mode | The Windows build includes 386 paging, nested page-fault recovery and sparse high-address mappings. Verified with Windows 3.11 and an original page-fault test. |
| A20 and ROM | Windows build implements fast-A20 port 0x92, proper initial aliasing, and write protection for BIOS/VGA ROM. |
| Guest RAM | 1–8 MiB in a 32 MiB owned allocation; Expanded supports up to 32 MiB guest RAM in a 64 MiB allocation. |
| CPU tuning | Fixed cycles with live adjustment; Expanded adds Auto and Max. Controller bounds are not sustainable-speed or MHz claims. |
| Extended memory | The compatibility runtime provides embedded XMS and EMS 4.0 board mode. Booted DOS retains its own HIMEM driver. Expanded offers optional VCPI/UMB; EMM386 and other DOS drivers are not bundled. |
| Worker ownership | One application owns Core1. Runtime mapping and buffer checks, bounded memory, cancellation and register/cache restoration protect the application's lifecycle. |

Protected-mode support does not establish compatibility with every DOS extender
or Windows application. The DOS runtime includes JIT; the separate Windows
executable uses the interpreter with a 1 MiB disk read cache and redundant-seek
avoidance. A paging-guarded JIT candidate gave no additional improvement in the
matched Windows startup test and remains experimental. Disk writes remain
synchronous. See [the optimization report](amiga-dosbox-optimisation-round2.md) for measurements.
Experimental [x87 emulation](amiga-dosbox-x87.md) is enabled in the EMS/Sound Blaster compatibility runtime; and the combined Net-JIT runtime. 64-bit x86 guests are not supported. The owned allocation and checks are not a general security sandbox
for untrusted programs.

## Files and media

| Feature | Support |
| --- | --- |
| Shared C: drawer | Read/write access to an Amiga directory, nested directories and restricted DOS 8.3 names. Current runtimes use bounded 4 KiB read-ahead for small read-only file requests; writes stay synchronous. [Measurement and coherence rules](amiga-dosbox-file-read-optimisation.md). |
| Screenshots | Ctrl+F5 saves the completed player display as a local PPM. Per-program previews appear beside the library list; Capture launches the selected entry. |
| Game saves | Normal files written by the game persist. Emulator snapshots/save states are not implemented. |
| Floppy boot | Read-only 1.44 MB BIOS disk images. |
| Hard-disk boot | The Windows/image runtime supports writable 32–256 MiB BIOS disk images, in 256 KiB increments, with bounds checking and persistent writes. Each image profile chooses its disk and startup command. |
| New hard disk | Create a 32, 64, 128 or 256 MiB raw FAT16 `.IMG`, with startup files and an OS library entry. DOSBox DOS 5 is the default; optional MS-DOS 6.22 uses the installed system files. Existing images are never replaced. |
| Workbench access | Choose, mount, open and eject raw FAT images using diskimage.device and fat95. Mounted images are added to OS without duplicate profiles. See [IMG versus VHD](amiga-dosbox-disk-images.md). DOSBox DOS 5 image profiles mount automatically as C: and eject after the session. Booted MS-DOS/Windows requires an ejected image. |
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
untested. The [compatibility runtime](amiga-dosbox-memory-audio.md) adds Sound
Blaster Pro 2 (8-bit) or SB16 (16-bit), OPL3, independent PC speaker and 32 kHz stereo PCM through AHI unit 0. SB Pro uses A220/I7/D1; SB16 adds high DMA 5.
New program profiles select this card and EMS with 8 MB guest RAM; existing
profiles retain their settings. Booted DOS/Windows audio remains unavailable. Net-JIT can combine networking with these audio devices; Expanded adds GUS/CMS/Tandy. Munt and SoundFont synthesis are not bundled.
The output device follows AHI preferences; HDMI output is not established.

## Verified compatibility

| Workload | What has been verified |
| --- | --- |
| Embedded DOS / MS-DOS 6.22 | Boot or shell operation, file persistence, keyboard input and clean return. |
| Windows for Workgroups 3.11 | Desktop, WinBench 3.11 Graphics WinMark and normal shutdown on the real A9, using standard VGA and 8 MB guest RAM. |
| WinBench | Graphics WinMark **1,923,752 pixels/sec**. Uses the guest clock; no equivalent physical PC speed is established. |
| Commander Keen 6 | Startup and map movement; timed startup throughput recorded. |
| Oregon Trail Deluxe | Title/menu and mouse-operated Options page on the Amiga, using the 16-colour VGA option. Full-game compatibility is not established. |
| Vette / Elite | Startup with LOADFIX and return to Workbench. Full-game compatibility is not established. |

Installing a game does not make it a verified compatibility result. Windows audio remains unavailable; the Expanded gameport has separate validation below. The mouse build has
passed native and ARM tests; Oregon Trail menu interaction is verified on the
Amiga. Windows pointer movement and double-click launch of Write also passed
on hardware using the Microsoft/IBM PS/2 driver. Pointer capture/warping for games requiring continuous relative movement is
not implemented. Third-party DOS mouse-driver compatibility is not fully tested.

### Monochrome colours and software OpenGL

Hercules supports White, Amber, Green and Paperwhite in Settings. These are
our RGB palette approximations, applied when the palette changes; they do not
add a per-pixel filter. Other adapters retain their colour palettes. The
existing DOS terminal text/background controls are separate.

The optional TinyGL renderer runs a partial fixed-function OpenGL API on the
A9. Direct rendering remains the default. No GLSL shaders, guest OpenGL driver
or FPGA 3D acceleration are included. It renders a 640×480 intermediate picture
before the normal XACP handoff; higher-resolution sources are downscaled.
See [the experiment](../amiga/dosbox/opengl/README.md).

## Experimental networking — TCP/IP and IPX

The Network page provides per-program TCP/IP or DOSBox-compatible IPX-over-UDP.
TCP/IP uses NE2000/libslirp NAT over the existing Roadshow connection, with
DHCP/manual IPv4, subnet, gateway, DNS, hostname and MTU controls. Diagnostics
include ping, DNS lookup, counters, logs and cooperative disconnect. IPX uses
a compatible UDP relay with address/port, connect/status/ping/disconnect controls.

Both require `bsdsocket.library`. Launcher 0.22.1 selects `ZZDosBox-Net-JIT`
for core auto/dynamic, combining networking with EMS and SBPro2/SB16/PC speaker.
Explicit core normal retains `ZZDosBox-Net` with EMS/PCM off.
TCP/IP additionally needs staged NE2000/mTCP guest utilities. libslirp is linked
into the host; no separate Amiga installation is needed. Embedded DOS with
Direct/Normal presentation is supported; networking is off by default.
Scanline, OpenGL, Expanded and debug-runtime combinations remain separate.
Windows/MS-DOS image networking, IPv6 and port forwarding remain pending.
IPX transport passed hardware checks; multiplayer games remain untested.
Renew obtains a fresh guest lease; Release clears saved guest configuration,
not the Amiga's address or an on-wire DHCP lease.

[Network controls, validation and limitations](amiga-dosbox-network-ui.md) ·
[Runtime integration and file-service changes](amiga-dosbox-runtime-integration.md).

## Expanded hardware candidate

The optional 0.21.0 Expanded runtime adds 486/Pentium models, up to 32 MiB
guest RAM, Auto/Max cycle control, VCPI/UMB, a joystick gameport, GUS/CMS/Tandy
audio and COM1 TCP/basic modem support. It also contains an opt-in, limited
register-only paging JIT experiment. These options have their own constraints
and do not change existing profiles. See the [expanded hardware guide](amiga-dosbox-expanded-hardware.md).

Launcher 0.21.1 provides separate **I/O** and **Joystick** sidebar pages.
I/O holds the COM1 TCP/modem settings. Joystick includes a live direction,
Fire and Button 2 tester and the per-program DOS gameport switch.

Launcher 0.21.2 adds saved 1-6 button controller choices, aligned tester panels
and selectable DOS key bindings for gamepad buttons 3-6. The DOS gameport
retains its two native buttons. Extra inputs require a compatible gamepad.

0.22.0 adds **Experimental physical COM1** for AmigaOS serial-compatible
built-in, USB and expansion drivers. I/O saves device/unit, DOS-controlled or
fixed baud/framing, and optional hardware flow control per program. USB
availability is detected and its shortcut disabled when unavailable.
RTS/DTR outputs are driver-managed; see the expanded hardware guide for limits.

[I/O and joystick settings/specifications](amiga-dosbox-io.md).
