# ZZDosBox configuration editor

Open **Developer → dosbox.conf** in launcher 0.14.0-dev. Select a line, edit it,
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
| `cpu` | `core=auto/normal/dynamic`; `cycles=500..10000` (also `fixed N`) |
| `dosbox` | `memsize=1/2/4/8` MiB; `machine=vga/ega/cga/svga_s3/tandy/pcjr/hercules` |
| `sdl` | `fullscreen=false/true`; `windowresolution=320x240/640x480/960x720` |
| `render` | `show_fps=false/true`; `renderer=direct/opengl`; `presentation=normal/adaptive`; `scanlines=off/classic/soft/gradient`; `monochrome=white/amber/green/paperwhite` |
| `midi` | `mpu401=none/intelligent/uart`; `midiconfig=<CAMD output port>` |
| `cdrom` | `source=none/iso/device`; `path=<Amiga ISO path or device volume>` |
| `dos` | `loadfix=false/true` |
| `terminal` | `foreground` and `background`: black/red/green/yellow/blue/magenta/cyan/white; `live=false/true` |
| `startup` | `autoexec=default/edited/file`; `autoexec_file=<Amiga path>`; `configsys=default/edited/file`; `configsys_file=<Amiga path>` |
| `developer` | `trace=false/true`; `profile=false/true`; `breakpoint=off/before/after`; `frame=1..10000000`; `handoff=default/serial/pipelined`; `poll_ms=default/1/4/20` |

OpenGL remains experimental. Software scanlines require fullscreen with FPS off.
Monochrome colour applies to Hercules. Booted DOS controls its own text colours
and runs until closed. Custom disk images and floppy boots use their on-disk
startup files. Image sessions support profiling and polling choices but not
trace breakpoints, memory tools, OpenGL, Adaptive or scanlines. External MIDI
requires CAMD and the selected output connection; PCM audio is not implemented.

This is a supported subset for this port, not a general DOSBox-X/Staging parser.
Options such as `cputype=pentium`, 256 MiB guest RAM, `cycles=max`, IDE emulation,
`int33=false`, Sound Blaster/mixer settings and EMS/XMS switches are not accepted.
The [DOSBox-X Windows 3.1x guide](https://github.com/joncampbell123/dosbox-x/wiki/Guide:Installing-Windows-3.1x)
is useful reference, but its sample configuration cannot be used unchanged.
