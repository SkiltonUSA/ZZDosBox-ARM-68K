# ZZDosBox configuration editor

Open **Developer → dosbox.conf** in the launcher. Select a line, edit it,
and press Return or **Replace**. **Save settings** writes `dosbox.conf` beside
the launcher, retaining the previous file as `dosbox.conf.bak`. Open and Save as
import/export plain text; Defaults restores the commented reference template.

This file contains global overrides for launches through the MUI interface.
Commented settings keep each program's preferences; overrides do not rewrite
those profiles. Changes apply on the next launch. The standalone runtime does
not read this file itself. Invalid keys, sections, values and duplicates report
an error with a line number. Unsupported combinations stop before launching.

For example:

```ini
[cpu]
core=dynamic
cycles=5000

[dosbox]
memsize=8
machine=vga

[sdl]
fullscreen=false
windowresolution=640x480
```

CPU core selection:

- `auto`: existing DOS runtime selection; disk-image profiles use the interpreter.
- `normal`: interpreter (`ZZDosBox-Windows`). Direct/Normal presentation only,
  without scanlines, trace breakpoints or memory tools.
- `dynamic`: existing JIT DOS runtimes; booted DOS/image sessions select
  `ZZDosBox-Windows-Dynamic`. That guarded build uses the interpreter while
  guest paging is enabled. It does not provide paged Windows JIT acceleration.

The selected executable must be present beside the launcher. Every launch log
records the requested core, selected executable, cycles and guest RAM.

| Section | Settings and supported values |
| --- | --- |
| `cpu` | `core=auto/normal/dynamic`; `cycles=500..100000`, `fixed N`, `auto` or `max`; see Expanded settings below |
| `dosbox` | `memsize=1/2/4/8/16/32` MiB; `machine=vga/ega/cga/svga_s3/tandy/pcjr/hercules` |
| `sdl` | `fullscreen=false/true`; `windowresolution=320x240/640x480/960x720` |
| `render` | `show_fps=false/true`; `renderer=direct/opengl`; `presentation=normal/adaptive`; `scanlines=off/classic/soft/gradient`; `monochrome=white/amber/green/paperwhite` |
| `midi` | `mpu401=none/intelligent/uart`; `midiconfig=<CAMD output port>` |
| `cdrom` | `source=none/iso/device`; `path=<Amiga ISO path or device volume>` |
| `dos` | `loadfix=false/true`; `ems=false/true` |
| `sblaster` | `sbtype=none/sbpro2/sb16` (220h, IRQ7, DMA1; SB16 high DMA5) |
| `speaker` | `pcspeaker=false/true` (independent PC beep through AHI) |
| `terminal` | `foreground` and `background`: black/red/green/yellow/blue/magenta/cyan/white; `live=false/true` |
| `startup` | `autoexec=default/edited/file`; `autoexec_file=<Amiga path>`; `configsys=default/edited/file`; `configsys_file=<Amiga path>` |
| `developer` | `trace=false/true`; `profile=false/true`; `breakpoint=off/before/after`; `frame=1..10000000`; `handoff=default/serial/pipelined`; `poll_ms=default/1/4/20` |

OpenGL remains experimental. Software scanlines require fullscreen with FPS off.
Monochrome colour applies to Hercules. Booted DOS controls its own text colours
and runs until closed. Custom disk images and floppy boots use their on-disk
startup files. Image sessions support profiling and polling choices but not
trace breakpoints, memory tools, OpenGL, Adaptive or scanlines. External MIDI
requires CAMD and the selected output connection.

EMS, Sound Blaster or PC speaker selects `ZZDosBox-Compat` (0.20.1-dev). It supports embedded
DOS, core auto/dynamic and Direct rendering with networking and Developer tools
off. Sound Blaster and PC speaker require AHI v4+ unit 0. Existing profiles default to these
options off; newly added programs enable both. See [memory and audio setup](amiga-dosbox-memory-audio.md).

This is a supported subset for this port, not a general DOSBox-X/Staging parser.
256 MiB guest RAM, IDE emulation, `int33=false`, alternative Sound Blaster
resources, arbitrary mixer settings and an XMS enable/disable switch are not
accepted. The Expanded candidate adds the settings documented below.
The [DOSBox-X Windows 3.1x guide](https://github.com/joncampbell123/dosbox-x/wiki/Guide:Installing-Windows-3.1x)
is useful reference, but its sample configuration cannot be used unchanged.

## Expanded runtime settings

These optional settings select the Expanded candidate. Existing profiles retain
their settings. See [hardware requirements and limitations](amiga-dosbox-expanded-hardware.md).

```ini
[cpu]
cputype=386
# Use either cycles=auto/max or a numeric cycles with cycle_mode.
cycle_mode=fixed
paged_register_jit=false

[dosbox]
memsize=8

[dos]
vcpi=false
umb=false

[joystick]
joysticktype=none

[sound]
extra_device=none

[serial]
serial1=disabled
host=127.0.0.1
port=23
```

Choices: `cputype=386|486|pentium`, `cycle_mode=fixed|auto|max`,
`memsize=1|2|4|8|16|32`, `joysticktype=none|2axis`,
`extra_device=none|gus|cms|tandy`, `serial1=disabled|tcp|modem`.
VCPI requires EMS enabled. COM1 requires
embedded DOS and a numeric IPv4 endpoint. CMS/Tandy require Sound Blaster off.
