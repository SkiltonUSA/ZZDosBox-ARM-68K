# DOSBox on ZZ9000: initial feasibility

12 October 2026. Interpreter prototype built; this is a separate project from
the installed NES/SNES Betas. Full DOS/PC emulation remains in development.
Upstream is pinned in `amiga/dosbox/upstream.json`.

## Source findings

- `libretro/Makefile.common` maps `WITH_DYNAREC=arm` to `C_DYNREC` and
  `C_TARGETCPU=ARMV7LE`. The x86-to-ARM dynamic recompiler is a promising match
  for Cortex-A9; its existence does not establish speed on this machine.
- `src/cpu/core_dynrec/risc_armv4le-common.h` publishes generated code using a
  Linux cache-flush software interrupt. This must be replaced for bare-metal
  Core1. Never execute the unmodified Linux path on XX19c.
- `src/cpu/dyn_cache.h` allocates a generated-code cache and optionally uses
  `mprotect`. Our port needs a bounded executable region within its actual
  Exec-owned allocation, runtime relocation, and explicit cache synchronisation.
- `libretro/src/emu_thread.cpp` and `libretro.cpp` use real C++ threads,
  condition variables, atomics and exception-based cancellation. The existing
  libretro wrapper cannot simply be linked into our single-Core1 worker.
- The wrapper requires C++17 and uses `std::filesystem`; DOS drives also use
  file/metadata APIs. Some frontend VFS support exists, but it is not an entire
  bare-metal filesystem implementation. Disk operations need a bounded 68k
  service or an initial RAM-backed image with explicit ownership.
- SDL compatibility code and optional audio/media dependencies are present.
  Existing Amiga SDL2 binaries cannot run on ARM. Reuse our presentation/audio
  architecture at the interface level, adapting the DOSBox frontend separately.
- Libretro save serialization functions return zero/false. NES/SNES-style save
  slots are not an available upstream feature; DOS game disk saves are separate.
- Retain GPL-2.0-or-later and third-party notices. No DOS games or proprietary
  operating-system images are included in this project.

## Proposed division

ARM Core1: x86 execution, emulated PC devices, DOS services, VGA rendering and
sound generation. The built-in DOS environment is the initial target.

68k: Amiga files, keyboard/mouse/joystick forwarding, display coordination and
AHI playback. The MUI launcher comes after the execution test. Frames should be
converted on ARM into owned ZZ9000 buffers rather than copied through the 68k.

Keep one application owner on Core1. Reuse the verified launcher ownership,
mapping probe, cancellation and reset-before-free rules, adapting allocation
size only from a measured memory budget. Preserve firmware/Core0 memory.

## First milestones

1. Create an original DOS diagnostic executable and run it against the upstream
   interpreter on the host. Check deterministic arithmetic, RAM writes, DOS
   output and exit. This establishes a correctness reference, not ARM speed.
2. Run the interpreter diagnostic on Core1 with bounded RAM, execution and
   cancellation. Its INT 21h text/exit adapter is diagnostic-only. Integrate the
   actual DOS kernel, DOS shell and storage after this execution milestone.
3. Enable ARMv7 dynrec only after generated-code cache handling is validated.
   Compare results with the interpreter, including self-modifying guest code,
   invalidation/reuse and cancellation. Measure x86 throughput on real hardware.
4. Add VGA output and real keyboard input, then Sound Blaster/AdLib and AHI.
   Measure display cadence and audio gaps separately from CPU throughput.
5. Add writable DOS storage, game profiles and a simple MUI launcher.

Start with text-mode and modest 2D DOS workloads. No 386/486 equivalent speed,
Windows support, demanding protected-mode games or FPS target is established.
Omit Voodoo, networking, CD audio and software MT-32 synthesis from the initial
prototype. External MIDI can be a later 68k service; no MIDI test is authorised
by this review.

## Interpreter prototype

Sources are in `amiga/dosbox/`. `diagnostic.S` is an original DOS COM program;
no game or DOS distribution is required. It exercises 16/32-bit arithmetic,
flags, multiply/divide, shifts, stack and string operations, predictable guest
RAM writes, INT 21h text and exit. Independent expected RAM FNV-1a is
`f23411c5`; the expected DOS text is `ZZDOS x86 arithmetic/memory PASS`.

The pinned upstream normal interpreter, CPU, paging, flags, ModR/M, callback
and memory-access code runs these instructions. Only configuration/module
construction is trimmed; the interpreter itself is not replaced. FPU and
recompilation are disabled. The diagnostic host implements only INT 21h
AH=09h/4Ch. No DOS prompt, file APIs, VGA, audio or PC peripheral emulation is
claimed. The full libretro wrapper is not linked.

The ARM/68k diagnostic reuses the existing XACP/XX19c owned-memory launcher,
local MMU/L1 cache policy, nonce mapping probe, stop mailbox and reset-before-
free teardown. Allocation is 32 MiB, including 2 MiB guest RAM, roughly 23 MiB
BSS (largely upstream paging tables plus RAM and bounded heap), code, tables
and stack. ARM allocations never borrow firmware memory. Page-table storage
is enlarged to 96 KiB to cover every supported allocation alignment; the
16,384-alignment permission test passes. This is an initial correctness budget,
not an optimized memory footprint.

The ELF packer accepts only bounded code/data, high zeroed BSS, relative
relocations and validated constructor pointers. Constructors run explicitly.
Picolibc `setjmp.S` is compiled again with the worker's soft-float flags;
the inherited archive member contained VFP instructions and is unsuitable for
this integer-only worker. Repeated guest initialization frees callback strings.

### Local commands

```sh
python3 scripts/build_dosbox.py --sanitize
python3 scripts/build_dosbox_arm.py --container-command '["podman","--connection","nuflix-converter-root"]'
.context/venv/bin/python tests/amiga/dosbox/check_arm.py
```

Current build uses the existing `zzdosbox-toolchain` container (GCC 15.3.1,
picolibc 1.8.12), the PIC libc produced by `build_dosbox_libc.py`, and the
pinned Amiga cross-tool image. LLVM `clang`, `ld.lld` and `llvm-nm` are also
required. A fresh-machine bootstrap is still to be packaged. Build artifacts
and upstream staging remain below `.context/amiga/dosbox/`; builds never launch
hardware automatically. Shared NES/SNES sources and installed Betas are not
modified.

Native AddressSanitizer checks pass, including repeat runs, unsupported DOS
services, bounded execution and cooperative cancellation. ARM instruction-model
checks cover relocation, repeated execution and cancellation; these are not
physical cache/MMU or performance measurements.

Licensing: preserve upstream GPL-2.0-or-later notices and the existing
GPL-3.0-or-later launcher/MMU notices. A combined distribution must satisfy the
GPLv3-or-later terms as well as the retained component notices.

### Physical A4000TX result — 12 October 2026

`SD032G:Dev/ZZDosBox/ZZDosBox-Test` (0.1.0-dev) ran on ZZ9000 Core1 using
XX19c/XACP 1.7. Twelve completed diagnostic runs (verification, warmup and ten
samples) matched `f23411c5`, returned guest exit 0 and the expected DOS text.
The cancellation workload stopped cooperatively. The final report recorded
zero register mismatches, zero cache-witness errors, restored processor/cache
state, launcher exit 0 and release of the Exec-owned allocation. Bridge and
Roadie remained present; Core1 ownership was released.

The median reset-plus-diagnostic sample was **65.2085 ms** (64.981–65.313 ms).
This includes clearing guest RAM and initializing the large paging/TLB tables
on every run: it is **not** steady-state x86 throughput, a PC speed rating or
video FPS. Cancellation reported 134.483 ms including the host's initial
approximately 100 ms wait before sending stop.

Evidence: [`amiga/records/2026-10-12/dosbox-interpreter/`](../amiga/records/2026-10-12/dosbox-interpreter/).
Next: integrate DOSBox's actual DOS kernel/shell and a bounded host-backed file
interface; prove external COM/EXE execution. VGA/input and audio follow, with
ARM dynrec handled as a separately validated optimization. Current code is an
execution foundation, not a playable DOSBox release.

## DOS kernel milestone — 0.2.0-dev

The separate `amiga/dosbox/dos/` frontend links upstream DOS services, file
handles, EXEC, PSP/MCB memory management, tables, classes, IOCTL and devices.
`dos-integration.patch` records the build-time changes: remove configuration
construction, mount the bounded drive, substitute a captured text console,
and reject disconnected serial/raw-disk operations. The normal interpreter
and COM/MZ loader remain upstream code. The full libretro frontend is not linked.

The 68k host stages external files into shared memory. ARM runs DOS INT 21h
EXEC and file APIs against a flat private C: drive; the host retrieves and
saves the resulting output. This is a staged file round trip, not a live
AmigaDOS filesystem mount. The drive holds at most eight files of 64 KiB each,
uses restricted 8.3 names and rejects paths. Program input is currently limited
to 65,280 bytes. Directories, rename/delete, arbitrary host access, interactive
shell, BIOS peripherals, VGA, sound, FPU and dynamic recompilation remain
unimplemented. Directory enumeration code exists but is not yet independently
validated. Temporary-name seeding uses a fixed time; a guest clock is not wired.

Two original fixtures require no commercial software:

- `FileTest.com`: DOS version, open/read, seek/EOF, create/write/close/reopen,
  byte comparison and missing-file errors. Reads `INPUT.TXT`, creates
  `OUTPUT.TXT`; the Amiga host saves the latter as `FileTest.out`.
- `ExeTest.exe`: genuine MZ header with relocation, resize/allocate/free DOS
  memory, text output and process return. The internal staged executable name
  is `TEST.COM`; upstream EXEC recognises MZ from its header.

Native AddressSanitizer tests cover repeated COM/EXE runs, output comparison,
short-input failure and filename/capacity rejection. Ten ARM instruction-model
cases cover both executable types at two relocation bases, repeats and stop.
These do not establish physical cache behaviour or DOS game compatibility.

```sh
python3 scripts/build_dosbox_dos.py --sanitize
python3 scripts/build_dosbox_dos_arm.py --container-command '["podman","--connection","nuflix-converter-root"]'
.context/venv/bin/python tests/amiga/dosbox/check_dos_arm.py
```

Artifacts are under `.context/amiga/dosbox-dos/`. The existing toolchain
container also needs `libstdc++-arm-none-eabi-common` headers. DOS translation
units use `-fno-freestanding` for their upstream C++ headers; exceptions and
RTTI remain disabled and no libstdc++ archive is linked. Picolibc `srandom.c`
is compiled with the same non-TLS PIC policy as the existing runtime.

Allocation remains 32 MiB; the uncached shared region now ends at `0x1c0000`
(all offsets relative to the owned allocation) to include bounded executable,
input and output buffers. Runtime mapping, single-Core1 ownership, cache
witness, state restoration and reset-before-free remain mandatory.

### Physical DOS kernel result — 12 October 2026

Installed separately as `SD032G:Dev/ZZDosBox/DOS-Kernel/ZZDosBox-DOS`
(0.2.0-dev). `FileTest.com`, `ExeTest.exe` and `INPUT.TXT` are beside it.
The actual A9 run passed COM file operations (18 interpreter slices) and MZ
relocation/memory checks (7 slices), each with guest exit 0. Slice counts are
execution-budget units, not timing measurements. The host saved 256 output
bytes to `FileTest.out`; pulling this back to the Mac and comparing every byte
against input passed (SHA256
`34e74cd225ecbf83b8c1fd7424fbfa663bcfff126e5d23a674668c67ab9da38e`).

Cooperative cancellation, all 14 restoration checks, cache witness and owned
memory release passed. Launcher exit was 0; Workbench, Roadie and the bridge
remained available with no Core1 owner. No FPS/PC speed claim is established.
The first log pull timed out; retry retrieved the complete CRC-verified log.

Evidence: [`amiga/records/2026-10-12/dosbox-dos/`](../amiga/records/2026-10-12/dosbox-dos/).
Next: interactive text console and shell, then VGA/input and sound. This is a
DOS kernel integration prototype, not a playable DOSBox release.

## Streaming console work — 0.3.0-dev

The next build adds an Amiga console window with bidirectional text transport.
DOS INT 21h input/output still executes through upstream DOS services on Core1;
the 68k only handles the Amiga console and exchanges batches through owned
shared-memory rings. `Echo.com` is an original interactive x86 program, not a
replacement DOS shell. The upstream shell's BIOS keyboard/video dependencies
remain a later integration step.

Sources: `amiga/dosbox/console/`. Build with `--console` on the existing DOS
builders; artifacts go to `.context/amiga/dosbox-console/`. The new executable
is `ZZDosBox` and defaults to `PROGDIR:Echo.com`; one COM/EXE path may be supplied
on its command line. Its data drive retains the previous staged-file limits.
Text input is ASCII, Return and backspace. Ctrl-C or closing the console
requests cancellation. There is no PC scan-code keyboard, ANSI terminal,
CP437 font, command-line argument forwarding, DOS shell, VGA or audio yet.

The RX ring holds 256 bytes and TX 4096. Indices have separate cache lines and
single writers, use unsigned wraparound arithmetic, and publish data before
indices with the existing cache/barrier callbacks. ARM blocks when output is
full and checks stop while waiting for either input or output. All transport
storage remains within the previously validated uncached shared region. The
68k services the rings once per Amiga tick and writes output in batches.

Tests exercise DOS console availability, blocking and nonblocking reads,
echo/no-echo, buffered backspace editing, output larger than the ring, counter
wraparound, delayed consumption, cancellation from both blocking paths and
repeated initialization. Native tests also check that cancellation during an
echoing read cannot leak echo state into the next run. The old bounded
nonstreaming diagnostics retain their original API.

### Architecture guidance from Xanai

The owner relayed Xanai's review: the PicoDrive-style 68k/ARM split, framebuffer
handling and application-owned Core1 worker model will remain supported in
XACP 1.8 and later. Preserve that model. This does not independently validate
our additional MMU/cache/debugger code on later firmware, nor establish
our own 68030 compatibility. Physical validation currently targets XX19c.

For future audio, measure AHI servicing and synchronization frequency as well
as PCM bandwidth. Xanai recommends reasonably large buffers and batching;
reducing sample rate alone may miss the main 68k cost. No audio or firmware
change is part of this console build.


### Console build and physical check — 12 October 2026

```sh
python3 scripts/build_dosbox_dos.py --console --sanitize
python3 scripts/build_dosbox_dos_arm.py --console --container-command '["podman","--connection","nuflix-converter-root"]'
.context/venv/bin/python tests/amiga/dosbox/check_console_arm.py
```

Installed separately in `SD032G:Dev/ZZDosBox/Console/`. Run the demo with:

```text
Execute SD032G:Dev/ZZDosBox/Console/Start-ZZDosBox
```

It opens a native console, runs `Echo.com` on the A9 and accepts text. `exit`
returns normally; Close requests cooperative cancellation. Other supported
small DOS programs may be supplied as `ZZDosBox path/to/program.com` with a
65536-byte Shell stack. This is a program runner, not an interactive DOS shell.

The physical keyboard check typed `hello`, confirmed its display and echo,
then typed `exit`. The complete transcript is 118 bytes with FNV-1a `2e350fbb`,
matching the independently constructed expected transcript. Guest and launcher
exited 0. A second run closed the window while the guest awaited input and
returned cancellation 2, followed by successful teardown. Screenshot and raw
logs: [`console evidence`](../amiga/records/2026-10-12/dosbox-console/).

The console API test transfers 5,032 bytes (`5ade1b27`) with zero mismatches.
Separate physical tests stop a blocked input read and a full output ring.
All runs require zero restored-register mismatches, zero cache-witness errors
and owned-memory release. These are correctness/lifecycle checks, not FPS or
x86 throughput benchmarks. BIOS keyboard/video services, CP437/PC key mapping,
DOS shell and audio remain the next work.

## Upstream shell and live C: drive — 0.4.0-dev

The upstream command parser now runs on Core1, with DIR, TYPE, redirection,
COPY, REN, COM/MZ EXE launch and return to the prompt verified. This extends
the 0.3 console; it does not implement a graphical PC yet.

`amiga/dosbox/shell/` contains the shell bootstrap, live file transport and
AmigaDOS service. The build stages the pinned upstream shell, batch parser,
Program and CommandLine methods. SUBST, DATE/TIME commands and configuration
control are omitted; only the DOS root is mounted. The shell's C++ templates
are compiled as PIC, with the GCC list implementation and runtime exception
notice retained under `shell/vendor/`. History is bounded to 32 completed
commands. C++ runtime failures use the existing controlled worker fault path.

The 68k keeps a lock on the selected drawer and services one filesystem request
at a time. ARM requests contain literal uppercase 8.3 names, never Amiga paths.
Subdirectories, linked entries, path separators and wildcards are rejected by
the file service; DOS wildcard matching happens on the ARM. Files are streamed
in 4096-byte chunks, with 16 open handles and at most 4096 enumerated entries.
Read/write, seek, truncate, close, stat, rename and delete are implemented.
Metadata comes from AmigaDOS; the DOS disk-space report caps its 16-bit cluster
counts. These controls are not a security boundary against another process
concurrently replacing files in the mounted drawer.

The service uses owned uncached storage at offsets 0x186000 and 0x187000;
32 MiB allocation, mapping validation, Core1 ownership, private MMU/cache
policy and reset-before-free remain as before. Cleanup after cancellation
suppresses further ARM file requests and the 68k closes all remaining handles.
The shared launcher and existing NES/SNES sources were not changed.

Build and local checks:

```sh
python3 scripts/build_dosbox_shell.py --sanitize
python3 scripts/build_dosbox_dos_arm.py --shell --container-command '["podman","--connection","nuflix-converter-root"]'
.context/venv/bin/python tests/amiga/dosbox/check_shell_arm.py
```

The native check runs the actual upstream shell under AddressSanitizer and
the actual filesystem-service C source against a test AmigaDOS facade under
AddressSanitizer/UBSan. It exercises rejected paths/links, access permissions,
chunk limits, 100 KiB I/O, truncation, seek/EOF, rename, enumeration and handle
exhaustion/cleanup. This facade does not itself prove AmigaOS behavior.

Six ARM instruction-model cases cover two relocations, a 100,003-byte copy,
repeated COM/EXE execution and cancellation while waiting for input or a file
reply. These cases check every memory access for allocation bounds/alignment.
Optional `--vette` and `--keen` probes use supplied, gitignored game files;
those longer unpacker probes guard allocation edges without the exhaustive
alignment hook. No game files are distributed with the port.

### Physical results

Installed separately in `SD032G:Dev/ZZDosBox/Shell/`. Start the DOS prompt with:

```text
Execute SD032G:Dev/ZZDosBox/Shell/Start-ZZDosBox
```

Default C: is its `CDrive` drawer. `--drive drawer` selects a different flat
root; `--command command` runs a startup command followed by EXIT. The latter
has a 90-second host deadline. Normal interactive use ends with EXIT, window
Close or Ctrl-C. Input remains ASCII, not PC scan-code keyboard emulation.
The test-only `--selftest` writes named fixtures in CDrive; use a fresh fixture
copy for a repeat. `--cancel-read` checks cancellation at the prompt.

On the A4000TX, the shell test made 250 file requests, read 101,514 bytes and
wrote 100,271 bytes. Pulled-back outputs matched byte-for-byte:

| Output | Bytes | SHA256 |
| --- | ---: | --- |
| RENAMED.BIN | 100,003 | `635e9a7d2f64ce04a46b1503cbe287b8721795f215f4b8cf24b256908acaab1a` |
| SAVED.TXT | 12 | `0ceb58570e2c58b2ead9ae67840bff6bddc70df6e18d43f19875e281f02d3f82` |
| OUTPUT.TXT | 256 | `34e74cd225ecbf83b8c1fd7424fbfa663bcfff126e5d23a674668c67ab9da38e` |

Physical tests also covered input-wait cancellation, typing DIR/EXIT into the
window and window-close cancellation. All restored 14 registers with zero
mismatches and a clean cache witness, returned RET1, and released owned memory.

### Supplied game startup probes

The two supplied archives were extracted without modifying their game files,
then transferred with CRC verification to separate `Games/VETTE` and
`Games/KEEN6` drawers. Vette has 25 files, 706,099 bytes; Keen has five files,
714,476 bytes. Both exceed the earlier staged-drive limits.

| Game | Physical result | Interpretation |
| --- | --- | --- |
| Vette | With LOADFIX, 247,959 file bytes read; stopped at BIOS video AH=00, AL=0E | Executable unpacks and requests EGA graphics; graphics not implemented |
| Commander Keen 6 | 104,934 file bytes read; stopped at an unimplemented PC BIOS call, AX=0000 | Executable loads/unpacks to a BIOS dependency; game operation not established |

Vette initially printed `Packed file is corrupt`. An original COM helper
(`shell/loadfix.S`) shrinks its own DOS allocation, reserves 64 KiB and EXECs
the named program. With that low-memory workaround, the unchanged executable
reaches the EGA request in both the ARM model and on hardware. The helper takes
one executable name, without game arguments. Its source is included; game data
is not. This result supports the low-memory explanation for this particular
failure, not a general claim about other packed executables.

Unsupported BIOS/port operations are explicit worker faults. Both game probes
returned worker error 1 and launcher status 20 as expected, with normal register/
cache restoration and owned-memory release. These are successful diagnostic
shutdowns, **not successful game launches or FPS results**. Workbench and the
bridge remained available; final Core1 ownership was released.

Evidence: [shell and game startup records](../amiga/records/2026-10-12/dosbox-shell/).
Next is the PC BIOS/device foundation and EGA/VGA memory/port model, then direct
ZZ9000 frame presentation and PC keyboard input. Sound and dynrec remain later
milestones. No firmware, startup sequence or GitHub release was changed here.

## 2026-10-12 — 0.5.0-dev PC devices and direct display

Added actual upstream PIC/PIT, keyboard controller, CMOS, BIOS, INT10 and
EGA/VGA register/memory/scanline code from the same pinned DOSBox revision.
The normal x86 interpreter still runs on application-owned Core1 through XACP.
Sound, PC mouse/joystick, SVGA/LFB, disk images and dynrec are not implemented.
The ARM RTC is fixed at 1980-01-01; the emulated PIT/BIOS timer advances normally.

### Display and ownership

The ARM assembles a bounded complete frame, then scales it into a locked,
OS-allocated 640×480 true-colour RTG bitmap. Frame pixels stay on the ZZ9000.
The 68k exchanges descriptors and completion signals, services files, and
coordinates the three buffers with the existing XX19c deferred-PAN protocol.
It does not copy the full frame over Zorro III.

Every grant checks the current bitmap aperture, stride, format and endpoint
nonce. Temporary NC/XN page mappings are removed before the copy-complete
acknowledgement. The host retains the bitmap lock through the display ACK;
error cleanup resets Core1 synchronously before releasing display/owned memory.
The shared launcher and existing NES/SNES sources are unchanged. This is not
new evidence of tear-free scanout: display ACK and visual tearing are separate
checks.

The allocation remains 32 MiB, with 2 MiB emulated PC RAM. Upstream paging
metadata occupies about 21 MiB, so this build uses a bounded 2 MiB runtime
heap. The ELF packer checks the existing image/BSS/stack limits. A local static
`localtime` wrapper avoids the cross-toolchain libc's thread-local buffer.

### Reproduction

```sh
python3 scripts/build_dosbox_pc.py --sanitize
python3 scripts/build_dosbox_pc.py --sanitize --game keen
python3 scripts/build_dosbox_pc.py --sanitize --game vette
python3 scripts/build_dosbox_dos_arm.py --pc \
  --container-command '["podman", "--connection", "nuflix-converter-root"]'
.context/venv/bin/python tests/amiga/dosbox/check_pc_arm.py
clang -std=c11 -fsanitize=address,undefined -Iamiga/dosbox/pc \
  tests/amiga/dosbox/direct_plan.c -o .context/amiga/dosbox-pc/test-direct
.context/amiga/dosbox-pc/test-direct
```

The container connection is this workspace's current toolchain connection;
select an available configured connection on another Mac. Game tests require
the user's privately extracted files; no game data is included in the source.
Generated upstream changes are captured in `pc-integration.patch` in the build
folder alongside the existing CPU/DOS/shell patches and source-hash manifest.

Native ASan checks pass COM/EXE regressions and the original EGA/PIT fixture:
all 128,000 capture pixels match its 16-colour planar pattern. Native game runs
reach Keen's animated intro and Vette's garage. The ARM instruction model passes
the EGA/PIT fixture at two relocation addresses, both supported 32-bit formats,
all 307,200 output pixels, guarded framebuffer span, filesystem service and
mapping revocation. The model must preserve Thumb state across execution
quanta: upstream soft-float routines include Thumb instructions.

The separate ASan/UBSan descriptor test passes 7,440 geometry/boundary/no-clobber/
unmap cases. Native/model results do not substitute for physical testing.

### Physical operation and limits

Installed separately at `SD032G:Dev/ZZDosBox/PC/ZZDosBox`. This is a timed CLI
prototype; it requires `--command`. The 0.4 Shell build remains the interactive
DOS prompt. Example Amiga Shell commands (stack 65536):

```text
SD032G:Dev/ZZDosBox/PC/ZZDosBox --command egatest
SD032G:Dev/ZZDosBox/PC/ZZDosBox --drive SD032G:Dev/ZZDosBox/Shell/Games/KEEN6 --command keen6c
SD032G:Dev/ZZDosBox/PC/ZZDosBox --drive SD032G:Dev/ZZDosBox/Shell/Games/VETTE --command "loadfix vette.exe"
```

The PC-device tests automatically inject Enter through the PC keyboard
controller at 4.0 emulated seconds (release at 4.1) and stop at 8 seconds.
This is not live Amiga-to-PC gameplay input. F10 requests cancellation; a
90-second host deadline also requests a cooperative stop. A host deadline or
transport/display failure is reported as failure, even if teardown succeeds.

On the physical A4000TX, the original EGA/PIT diagnostic passes and returns
through DOS normally (18 frames, 274 emulated milliseconds). Keen and Vette
both reach the scheduled 8,000 ms stop with 558 generated frames. Each closes
file handles, restores all 14 checked registers and the cache witness, returns
RET1 and releases owned memory. No game file writes occurred. The first Keen
screenshot shows its startup screen; Vette's first screenshot was taken while
still black/loading. Later captures and final-build timings are in the record
below.

**558 frames / 8 emulated seconds is not a 70 FPS hardware measurement.**
The interpreter is configured for 3,000 emulated cycles per millisecond and
has no real-time pacing. The current synchronous copy/grant/PAN waits and
20 ms host polling impose overhead. Next work is real PC keyboard input,
measuring CPU versus presentation costs, and reducing coordination overhead;
audio comes afterward. Full gameplay, speed, compatibility and tear-free
scrolling remain unverified.

Final physical build: 692,312 bytes, SHA256
`4fddc62bf3ea5a5790fa8ca48a4c0f8916d292ced5f0fd3171b03b73ca052493`.
The ARM ELF is unchanged from the first physical passes; the final host adds
elapsed timing and a required-command guard. All three tests were repeated
successfully after deploying this executable with CRC verification.

| Final physical run | Generated frames | Emulated time | Host run time | Whole-run generated FPS |
| --- | ---: | ---: | ---: | ---: |
| EGA/PIT fixture | 18 | 0.274 s | 1.871 s | Diagnostic only |
| Commander Keen 6 | 558 | 8.000 s | 41.335 s | 13.5 |
| Vette + LOADFIX | 558 | 8.000 s | 43.722 s | 12.8 |

Host run time excludes launcher setup but includes game loading, file service
and display waits. Vette also includes a screenshot request; Keen's final
measurement has no screenshot request. These are whole-startup-run averages,
not steady gameplay benchmarks or real-time emulation. The final display
counters report 557 completed flips each; cleanup can cancel the final pending
flip. Display ACK waiting accounted for 18.75 s in Keen and 19.43 s in Vette;
that includes the coarse host polling interval, not just display hardware
latency. It provides a concrete scheduling target, not a promised speed gain.

A later physical Vette capture shows its Corvette selection/garage screen.
The ARM model additionally rejects an incorrect framebuffer nonce and proves
mapping revocation before returning the error.

Evidence: [PC-device records](../amiga/records/2026-10-12/dosbox-pc/).
No firmware/startup changes or GitHub publication were made for this milestone.

## 2026-10-12 — Host scheduling optimisation

The first change targets the 68k host's unconditional `Delay(1)`, leaving the
ARM image, x86 interpreter, emulated cycle budget, pixel output and display
ownership protocol unchanged. `--poll-ms 20` retains that original DOS tick
wait for comparison. `--poll-ms 1` (new default) and `--poll-ms 4` use synchronous
`timer.device` requests. The timer request has completed before it is closed.
Timer failure requests a cooperative stop; mapping/ACK/reset-before-free rules
are preserved. No busy-spin host loop was introduced.

### Controlled physical comparison

Same executable and game commands, same 8,000 emulated milliseconds and 558
rendered frames. No screenshot requests during these comparisons. Host elapsed
time includes loading and display/file service; launcher startup is excluded.

| Game | Legacy tick wait | 1 ms timer | Improvement |
| --- | ---: | ---: | ---: |
| Commander Keen 6 | 41.397 s / 13.48 FPS | 19.148 s / 29.14 FPS | 2.16× throughput |
| Vette + LOADFIX | 42.166 s / 13.23 FPS | 25.298 s / 22.06 FPS | 1.67× throughput |

These FPS figures count generated VGA frames across the timed startup run,
not gameplay benchmarks. Both games remain below real-time emulation. The
unchanged ARM ELF SHA256 is
`25f08068b30229464a6f57225d20eea822120a71f0b2a4f5a30254b56b96366f`.
The gain is host coordination, not a faster x86 interpreter or reduced image
quality/frame count.

Keen's legacy wait made 1,214 calls totalling 39.375 seconds: about 32.4 ms
per call, rather than a precise 20 ms. With the 1 ms timer, 12,534 calls totalled
15.666 seconds, about 1.25 ms per call. The observed display-ACK wait fell from
18.713 to 5.009 seconds. These timings overlap: the ARM continues to work while
the host sleeps. They must not be added together or treated as CPU utilisation.

Keen with `--poll-ms 4` completed in 20.470 s (27.26 FPS) using 4,384 waits:
65% fewer wakeups than 1 ms, with roughly 6.5% lower throughput. This is a useful
lower-overhead option. It has not been tested on a 68030 or with audio; do not
infer slow-host compatibility from the A4000TX results.

Physical validation covers the EGA/PIT fixture at 1 ms, all five game comparison
runs, and input-wait cancellation. All report register restoration with zero
mismatches, clean cache witness and owned-memory release. Game reads and the
558-frame counts match the baseline, with zero file writes. Display counters
are 557 or 558 flips depending on whether the last pending flip completes before
cleanup. No claim of newly verified tear-free scanout is made.

### Remaining optimisation candidates

1. **Overlap frame production, copying and presentation.** Three slots exist,
   but the host currently withholds the next copy grant while a PAN is pending.
   Preserve ACK-based slot reuse and the held bitmap lifetime when pipelining.
   Do not fix this by writing the active scanout slot or dropping the ACK check.
2. **Specialise the scaler/copy.** The current generic loop performs coordinate
   scaling and a format choice for every output pixel. Common 320- and 640-pixel
   inputs allow simpler loops and paired stores. Measure the copy first and
   validate the exact pixels, pitch bounds and cancellation after any change.
3. **Profile the x86 interpreter and VGA raster work separately.** The ARM
   build currently uses `-Os`; selective speed optimisation is a candidate,
   subject to the existing image/BSS bounds. Dynamic recompilation is a larger
   later project. Neither has been measured by this host-only change.

The final build also records host-observed grant-to-copy-completion time. It
includes mapping, scaling, stores, unmapping, ARM scheduling and the host's next
poll; it is not an isolated memory-bandwidth or ARM-kernel measurement.

Records: [DOSBox scheduling comparisons](../amiga/records/2026-10-12/dosbox-scheduling/).

### Final profile and installed default

The final host build repeats the 1 ms game runs at 19.197 seconds for Keen
(29.07 FPS) and 25.294 seconds for Vette (22.06 FPS). Observed copy totals are
4.711 and 4.690 seconds respectively, approximately **8.4 ms per frame** for
both games. Observed PAN-ACK waiting totals are 5.150 and 5.049 seconds. The
longer Vette run therefore is not explained by these two measured phases
alone; CPU/VGA/file-service profiling is still needed. The generic scale/copy
is a useful target, but it is not the whole remaining cost.

Both final runs restored registers/cache witness and released owned memory.
The updated executable is now the normal `PC/ZZDosBox`; `PC/ZZDosBox-PreWait`
preserves the previous installed binary. The default wait is 1 ms; use
`--poll-ms 4` for the measured lower-overhead option or `--poll-ms 20` to
reproduce the legacy scheduling. This remains the timed 0.5.0-dev prototype,
without live PC keyboard controls or audio.

Final executable: 695,036 bytes, SHA256
`11e71fdbfa8f9e561fb7ea8f0c09843986bd0dfaa015210f8cfd14dd42832627`.
No firmware, shared launcher, emulator core, ROM contents or GitHub release
was changed for this optimisation.

## 2026-10-12 — ARM scaler specialisation

The next change is in ARM code: `pc/scale.h`, called from `pc/direct.cpp`.
320-pixel rows duplicate each source pixel into two output pixels; 640-pixel
rows copy directly. Pixel format is selected outside the inner loop. Other
widths retain the original nearest-neighbour calculation. Vertical scaling,
640×480 output, row-level cancellation, nonce checks, temporary mapping and
reset-before-free ownership rules remain unchanged. Scalar integer operations
avoid adding any VFP/NEON context requirements.

### Correctness checks

```sh
clang++ -std=c++17 -O2 -fsanitize=address,undefined -Iamiga/dosbox/pc \
  tests/amiga/dosbox/scale.cpp -o .context/amiga/dosbox-scale/test-scale
.context/amiga/dosbox-scale/test-scale
.context/venv/bin/python tests/amiga/dosbox/check_scale_arm.py
# After the normal --pc component build:
.context/venv/bin/python tests/amiga/dosbox/check_pc_arm.py
```

Native ASan/UBSan and the compiled A9 row model each check all 1,024 input
widths and both output formats against the reference formula: 1,310,720 pixels
per test, with destination guards. Native sources have exact-size allocations;
ARM-model sources end on an unmapped page boundary. The linked ARM model also
passes EGA/PIT, both relocation bases and formats, complete output pixels and
bad-nonce rejection/mapping revocation. The full component build preserves the
existing image/BSS/allocation limits.

### Physical comparison

Fresh baseline and candidate runs use the same 1 ms host wait, supplied ROM
files, 8,000 emulated milliseconds and 558 generated frames. No screenshots
were taken. Host run time excludes launcher startup and includes game loading,
file service and display waits.

| Game | Previous scaler | New scaler | Throughput gain |
| --- | ---: | ---: | ---: |
| Commander Keen 6 | 19.195 s / 29.07 FPS | 16.780 s / 33.25 FPS | 14.4% |
| Vette + LOADFIX | 25.355 s / 22.01 FPS | 23.055 s / 24.20 FPS | 10.0% |

Observed grant-to-copy-completion fell from 8.43 to 4.18 ms/frame in Keen and
8.44 to 4.25 ms/frame in Vette, roughly halving that phase. This includes
mapping, scaling, copying, unmapping and polling delay; it is not an isolated
kernel benchmark. The overall gains are smaller because CPU/VGA work and
presentation waits remain. These are startup throughput figures; both runs
still take longer than real time for their eight emulated seconds.

Physical EGA/PIT, both games and input-wait cancellation pass with register/
cache-witness restoration and owned-memory release. Game frame counts are
unchanged and file writes remain zero. No new tear-free-scanout claim is made.

The normal `SD032G:Dev/ZZDosBox/PC/ZZDosBox` is updated; `ZZDosBox-PreScale`
preserves the previous scheduling-optimised executable. Still the timed
0.5.0-dev prototype, with gameplay input and audio pending. Final executable
695,244 bytes, SHA256
`68faf71c85135dc1e915141a82d821a522a7769598b82596d0c6077ec4ed4b4b`.
ARM ELF SHA256
`c800d2e1361dec8f82556810e6ff1b64d040e062d4ec665ae543021af4027af2`.

Next is overlapping frame production/copy/presentation while preserving slot
ownership and firmware ACK checks, plus profiling interpreter versus VGA work.
Records: [ARM scaler comparison](../amiga/records/2026-10-12/dosbox-scaler/).
No firmware, shared launcher or GitHub release changes.

## 2026-10-12 — Copy/PAN overlap experiment

Added an opt-in `--pipeline-video` host path using the existing third buffer.
A second ARM copy can run while the preceding PAN awaits its firmware ACK.
Batches are limited to two copies before draining and unlocking, so window
messages can be serviced regularly. The front and pending buffers remain
unavailable for rendering. Normal completion drains the last display ACK;
errors retain the bitmap lock until synchronous Core1 reset and cleanup.
`--serial-video` and the default use one-copy batches.

The ASan/UBSan event model (`tests/amiga/dosbox/present_plan.c`) passes 98
latency schedules / 168,000 frames, counter wrap, ordered presentation,
slot exclusivity, bounded batches and timeout/stop lock retention. Hardware
EGA/PIT and both game runs restore registers/cache witness and owned memory.
The ARM ELF is unchanged from the scaler milestone.

| Timed startup | Serial | Pipeline | Overlapping copies |
| --- | ---: | ---: | ---: |
| Keen 6 | 16.808 s / 33.20 FPS | 16.749 s / 33.32 FPS | 0 |
| Vette | 23.092 s / 24.16 FPS | 23.071 s / 24.19 FPS | 0 |

All game runs generate and acknowledge 558 frames over 8,000 emulated ms,
with 1 ms host waits and no screenshots. Differences of 0.1–0.4% do not
establish a performance improvement. The next frame was never ready early
enough to overlap a pending PAN. Serial presentation therefore remains the
default. Hardware has not exercised the overlapping-copy branch; the model
covers that branch, but does not prove physical behaviour or tear-free scanout.

The tested experimental executable was 698,208 bytes, SHA256
`1240008bff62accb65f8bfbcbc6e6ed209129d5094d45530ad8e82e78e0123c5`,
with pipeline enabled by default for this A/B test only. It was staged as
`PC/ZZDosBox-Opt`; the normal installed executable was retained. Subsequent
source defaults to serial and exposes the explicit pipeline option.
Records: [display scheduling experiment](../amiga/records/2026-10-12/dosbox-pipeline/).

## 2026-10-12 — Speed-optimised x86 interpreter

Compile only upstream `src/cpu/core_normal.cpp` with `-O2` instead of `-Os`.
The rest of the ARM build retains `-Os`; source, guest cycle budget, scaler,
cache policy, 640×480 output and 1 ms host wait are unchanged. This remains
DOSBox's normal interpreter, not a dynamic recompiler. Serial presentation
is the default after the unsuccessful pipeline experiment.

The PC build now defaults to this per-file override. For a reference build:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-cpu-opt Os \
  --container-command '["podman","--connection","nuflix-converter-root"]'
```

`build.json` records the per-file flag in `arm_overrides`; object cache keys
include it. The ARM image grows from 521,784 to 538,432 bytes (+16,648),
inside the existing 32 MiB allocation and text/BSS limits. No NEON/VFP use
or generated-code executable cache is introduced.

The linked ARM model passes exact EGA/PIT output in both pixel formats and
two relocation bases, invalid-nonce rejection and mapping revocation. It
now also runs the original x86 arithmetic/memory, DOS file and MZ relocation
fixtures through this actual PC image at both bases.

Physical startup comparisons use 558 frames / 8,000 emulated ms each, no
screenshots, same serial display flow and 1 ms waits:

| Game | Size-optimised interpreter | Speed-optimised interpreter | Throughput gain |
| --- | ---: | ---: | ---: |
| Keen 6 | 16.808 s / 33.20 FPS | 15.608 s / 35.75 FPS | 7.7% |
| Vette | 23.092 s / 24.16 FPS | 22.157 s / 25.18 FPS | 4.2% |

Both generated and acknowledged all 558 frames, made no game file writes,
and passed register/cache-witness restoration and owned-memory release.
Observed copy latency remains about 4.2–4.3 ms/frame; the improvement comes
from compiler optimisation of the interpreter, not a new display route.
These single-run comparisons measure startup throughput, not steady gameplay
or real-time PC emulation. The model is correctness evidence, not a speed test.

Physical EGA/PIT and input-wait cancellation also pass. The final default
build is byte-identical to the explicitly selected `--pc-cpu-opt O2` test.
Executable: 714,964 bytes, SHA256
`927b5dd8738de7e0b44e6a08dc289a215dc7741fd2a673e0cecca4ec8dfb6379`.
ARM ELF SHA256
`83f12352d8a6607e32af8882fd422d85181d4966799a432e45adfaaca8d30ad7`.

Records: [interpreter optimisation](../amiga/records/2026-10-12/dosbox-cpu-o2/).
Live PC gameplay keys and sound remain pending. Further performance work
should profile interpreter/VGA costs, then evaluate ARM dynrec with bounded
executable memory and explicit code-cache synchronisation.

Installed as the normal `SD032G:Dev/ZZDosBox/PC/ZZDosBox`; the prior scaler
build is preserved as `ZZDosBox-PreCPU`. Active Keen Ctrl-C also passed
(216 frames, 3,108 emulated ms, timed_done=0), restoring registers/cache witness
and releasing the allocation. Still 0.5.0-dev; no firmware or GitHub changes.

Post-install EGA/PIT passed through the default executable path; final
Workbench/bridge are available and Core1 ownership is released.

## 2026-10-12 — Optional ARM profiling

`--profile` records exclusive timer ticks for CPU execution, PIC/timer events,
scanline pixel conversion, framebuffer grant wait, map/scale/copy/unmap and
other work. It reads the existing mapped 64-bit global timer, without changing
timer control or PMU registers. Counters stay private until final publication
in the application's owned shared mailbox. Normal runs disable profiling.

CPU includes emulated guest memory accesses and VGA memory handlers called
from the interpreter. Events includes upstream VGA line generation and PIC/
timer scheduling, excluding separately timed nested pixel conversion/copy/
grant work. Other includes DOS/BIOS callbacks, file service waits, startup and
cleanup. Grant is the ARM's wait for permission to copy, not the full PAN ACK
wait measured by the host. Neither is a measurement of Zorro bandwidth.

Explicit scopes avoid C++ destructors across the frontend's longjmp exits.
Cancellation unwinds the timing stack before cleanup; invalid depth/overflow
marks a profile invalid without changing guest execution. Each timestamp uses
a stable high/low/high timer read. Summed exclusive ticks must equal the total.
Raw ticks and percentages avoid assuming a fixed timer frequency. Instrumented
runs include timer/stack overhead and are not used for headline FPS comparisons.

```sh
clang -std=c11 -O2 -fsanitize=address,undefined -Iamiga/dosbox/pc \
  tests/amiga/dosbox/profile.c -o .context/amiga/dosbox-profile/test-profile
.context/amiga/dosbox-profile/test-profile
python3 scripts/analyze_dosbox_profile.py <captured-Amiga-log>
```

The native model covers nested scopes, exclusive sums, longjmp-style unwind,
low-word wrap and rejected invalid stack operations. The linked ARM model also
checks publication and sums with a synthetic timer; synthetic ticks are never
performance evidence. Physical measurements follow below.

### First physical profile

Both eight-second emulated startup runs completed 558 frames/ACKs and clean
restoration. Exclusive ARM time with profiling enabled:

| Phase | Keen 6 | Vette |
| --- | ---: | ---: |
| Interpreter (including guest memory handlers) | 31.6% | 31.3% |
| PIC/timer/VGA events | 21.4% | 26.4% |
| Scanline colour conversion | 27.1% | 22.9% |
| Waiting for framebuffer grant | 3.8% | 2.7% |
| Framebuffer mapping/scaling/copy | 11.2% | 8.4% |
| Other, including file service and startup | 4.9% | 8.3% |

Totals exceed the timer's low 32-bit range; the new 64-bit accounting handles
this without overflow. The analyser validates exact sums before presenting
percentages. Instrumented wall times: Keen 15.909 s, Vette 22.934 s. Fresh profiling-disabled references for
the same executable are Keen 15.511 s / 35.97 FPS and Vette 22.093 s / 25.26 FPS.

The conversion phase is a substantial target. Both games use RGB565 output
from upstream VGA (Keen 320×400, Vette 640×400 in the native startup fixtures).
The next candidate replaces per-pixel RGB565 arithmetic with two 256-entry
constant tables, indexed by each source byte and ORed into the same RGB888
result. This uses 2 KiB of read-only data. Format selection moves outside the
pixel loop; 32-bit rows use the application's existing scalar memcpy once per
row. Source alignment, exact colours and full frame counts remain required.

Native ASan/UBSan and the isolated Cortex-A9 model pass all 65,536 RGB565
colours and all 1,024 supported widths at 8/16/32 bpp. Native tests cover four
source alignments; ARM sources end at an unmapped page boundary. Both check
destination guards. The ARM harness must link the production scalar
`amiga/common/arm/memory.c`; linking the libc archive's VFP memcpy was
a harness error found and fixed during testing. No VFP support was enabled.

### Physical colour-converter comparison

Profiling disabled for both baseline and candidate; 1 ms polling, 558 frames
and 8,000 emulated ms each, no screenshots. No guest cycle-budget, resolution,
frame skipping, CPU compiler flags or display-path change.

| Startup workload | Previous converter | Table converter | Throughput gain |
| --- | ---: | ---: | ---: |
| Keen 6 | 15.511 s / 35.97 FPS | 13.332 s / 41.85 FPS | 16.3% |
| Vette | 22.093 s / 25.26 FPS | 19.337 s / 28.86 FPS | 14.3% |

Both games acknowledge all 558 frames, make zero file writes, restore checked
registers/cache witness and release owned memory. These are single-run startup
throughput comparisons, not steady gameplay FPS or real-time DOS emulation.
The converter remains scalar and exact; no frame or rendering work is omitted.

The full linked ARM PC tests also pass, including both relocations/formats,
exact EGA pixels, CPU/DOS/MZ fixtures, profile sums, nonce rejection and
cancellation. The physical EGA/PIT smoke with profiling enabled passes.
Final executable: 720,548 bytes, SHA256
`a3e8c8cf41590d5b72b53bb2896ba100824b14c4954311c7bf8f20920fb3a034`.
ARM ELF SHA256
`36a8d323e72bf922c4a1852aa777569aa0988b893d64e3d684b6e88edafc8558`.
ARM image: 541,596 bytes, same 32 MiB owned allocation. Profiling stays off
by default. Gameplay keyboard input and audio remain pending.

Follow-up profiles reduce raw colour-conversion ticks by approximately 50%
for both games. Its share falls from 27.1% to 15.8% in Keen and 22.9% to 13.3%
in Vette. Interpreter and PIC/VGA events together now account for about 60%
and 65% of measured ARM time respectively. These are the next major targets;
the interpreter percentage includes guest accesses to emulated VGA memory.
Instrumented totals remain separate from the profiling-disabled FPS table.

Installed as the normal `SD032G:Dev/ZZDosBox/PC/ZZDosBox`; previous interpreter
build preserved as `ZZDosBox-PreVGA`. Physical input-wait cancellation and
Ctrl-C during Keen also pass with clean restoration/release and valid profiles.
Still 0.5.0-dev. No firmware, startup or GitHub publication changes.
Records: [profiling and conversion](../amiga/records/2026-10-12/dosbox-profile/).

Post-install default-path EGA/PIT passed (18 frames / 18 ACKs). Final state:
Workbench, Roadie and bridge available; Core1 ownership released.


## PIC and VGA compiler optimisation (12 October 2026)

The follow-up profile made PIC/timer/VGA event work the next target. Compile
only upstream `src/hardware/pic.cpp`, `vga_draw.cpp` and `vga_memory.cpp` with
`-O2`; the interpreter remains `-O2` and other ARM units remain `-Os`.
No source algorithm, floating-point policy, guest cycle budget or display
ownership change. The build records per-file flags and includes them in
object-cache keys. The PC default now selects this combination; the previous
hardware-module settings remain reproducible with:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-hardware-opt Os \
  --container-command '["podman", "--connection", "nuflix-converter-root"]'
```

Fresh profiling-disabled physical runs use 1 ms polling and the same 558
frames / 8,000 emulated ms, with no screenshots:

| Startup workload | Previous build | Hardware modules at O2 | Throughput gain |
| --- | ---: | ---: | ---: |
| Keen 6 | 13.291 s / 41.98 FPS | 12.990 s / 42.96 FPS | 2.3% |
| Vette | 19.321 s / 28.88 FPS | 18.192 s / 30.67 FPS | 6.2% |

Both pairs retain identical slice counts, file bytes read and output hashes.
All 558 frames are acknowledged; neither workload writes files. Checked
registers/cache witness are restored and owned memory released. These remain
single-run startup-throughput comparisons, not steady gameplay measurements.

Full linked ARM checks pass both relocation bases/output formats, exact EGA
pixels, CPU/DOS/MZ fixtures, profile publication/sums, invalid nonce handling
and cancellation. Physical EGA/PIT also passes. The default build is
byte-identical to the explicitly selected `--pc-hardware-opt O2` candidate.

Executable: 728,548 bytes, SHA256
`12667663cfa1426196620bfa5a5396ba9f64e615098f971d373a1d72bc01a519`.
ARM ELF SHA256
`885a85f983e344a482f14595f8c81ba7732b01923c46e796f7635c01e15a0975`.
ARM image grows by 7,980 bytes to 549,576; the owned allocation stays 32 MiB.

Separate instrumented comparisons against the preceding converter build show
14.5% fewer event ticks for Keen and 18.9% fewer for Vette, with identical
bucket call counts. CPU ticks change by +1.2% / -0.8%; the main measured
benefit is event work, not faster interpretation. Capture ticks rise by
3.7% / 4.2% despite unchanged converter code; these profiles include layout,
cache and run-to-run effects. No per-module causal split is claimed. Event
shares now measure 21.5% / 25.4%; interpreter shares are 37.4% / 36.7%.

Physical input-wait cancellation and active Keen Ctrl-C pass, with valid
profile sums and clean restoration/release. Ctrl-C stops at 262 frames /
3,757 emulated ms (`timed_done=0`). Installed as the normal prototype;
previous converter build retained as `ZZDosBox-PreEvents`. Version remains
0.5.0-dev; live gameplay controls and audio are pending.
Records: [PIC and VGA optimisation](../amiga/records/2026-10-12/dosbox-events/).

Post-install default-path EGA/PIT passed (18 frames / 18 ACKs). Final state:
Workbench, Roadie and bridge available; Core1 ownership released.

## Launcher and disks — 0.6.0-dev

The new MUI launcher (`amiga/dosbox/launcher/`) provides per-program C: drawers,
LOADFIX, default/edited AUTOEXEC.BAT, an ASCII line editor with import/export,
a DOS terminal, terminal colours, 1/2 MiB guest memory, VGA/EGA/CGA and fixed
1000/3000/6000 CPU cycles. It starts the existing application-owned Core1 player
through a bounded session file, then returns to the launcher. F10 exits a game;
`exit` or the close gadget closes the terminal. MUI 3.8+ is required; no custom
MCC is needed. The runtime gains live PC keyboard press/release transport and
unlimited sessions. The original timed diagnostics remain available.

CONFIG.SYS is retained per profile for editing/export only. The embedded DOS
kernel does not execute CONFIG.SYS device drivers. XMS, EMS, BIOS disk boot,
PC sound and joystick emulation are not implemented in this milestone.
Guest memory selection is validated at both ends of the transport; it does not
change the 32 MiB application-owned allocation or the conventional 640 KiB map.

The writable C: drawer grows with the host filesystem and is visible in
Workbench. The current filesystem bridge admits root-level DOS 8.3 files only;
subdirectories and multiple guest drives remain pending. Free-space reporting
uses the existing DOS allocation geometry, capped at 65,535 x 32 KiB (~2 GiB),
not a preallocated 2 GiB image or reservation.

`prepare_dosbox_disks.py` generates FAT12 mountlists for five 1.44 MiB images,
using existing fat95 and [diskimage.device 37.7](https://aminet.net/package/disk/misc/diskimage)
by Thore Boeckelmann. Images mount READONLY; Workbench can copy their files into
the shared writable C: drawer. Do not reuse device units 0–4 if another image
application owns them. Mounting in Workbench does not implement PC BIOS disk
services or install/boot MS-DOS. No OS or game media belongs in the repository.

The [DOSBox 0.74-3 manual](https://www.dosbox.com/DOSBoxManual.html) distinguishes
host-directory MOUNT, image IMGMOUNT, BOOT, startup `[autoexec]` and LOADFIX.
It is a reference for the upstream feature set, not a claim that this reduced
bare-metal port implements every command. Our small LOADFIX.COM helper is
separate from upstream's internal utility and currently accepts a single
program filename. Next disk milestone: bounded block I/O and BIOS INT 13h,
boot a supplied floppy, then add a writable FAT16 hard-disk image with exclusive
ownership between the emulator and Workbench. CONFIG.SYS drivers must be tested
only after real DOS boot and the relevant memory services exist.

Upstream remains `libretro/dosbox-libretro` at the revision in `upstream.json`.
[DOSBox-X](https://github.com/joncampbell123/dosbox-x) is a separate fork and a
possible reference; no code from it has been integrated in this milestone.

Build the launcher with `python3 scripts/build_dosbox_launcher.py`; build the
runtime with `python3 scripts/build_dosbox_dos_arm.py --pc` and the local
container connection override when needed. The launcher model test is
`tests/amiga/dosbox/launcher.c`; the linked ARM suite is
`tests/amiga/dosbox/check_pc_arm.py`. The latter includes a stop during a
framebuffer write and verifies mapping revocation before cooperative unwind.

Physical verification on the A4000TX passed the configured EGA/CGA fixtures,
shared C: write/read, PC BIOS keyboard input, unlimited Keen with F10 exit,
and launcher-generated default AUTOEXEC followed by an interactive `exit`.
Workbench disk eject/remount/repeated-mount and all seven MUI tabs were checked.
Each successful worker run restored the checked registers/cache witness and
released its owned memory. The launcher uses temporary batch/session files and
removes them after return. Original runtime and initial UI configs are retained
as local backups. See `amiga/records/2026-10-12/dosbox-launcher/` for logs and
build provenance; no additional gameplay performance claim is made.

## DOSBox-X comparison and 0.7.0-dev

Reviewed and built DOSBox-X `378e6f8a25768594df87c9f5c99b5df9b3f6a8b0`
(2026.10.01, SDL2) as a separate native Mac reference. ZZDosBox remains based
on pinned dosbox-libretro; this is not a switch to DOSBox-X. The Mac build
uses `--disable-avcodec` because the installed host FFmpeg has a missing x265
dylib. No changes to the Mac's FFmpeg installation were needed.

| Capability | ZZDosBox 0.7.0-dev | DOSBox-X reference |
|---|---|---|
| Amiga MUI / XACP / owned Core1 / direct ZZ9000 display | Implemented | Needs a new backend |
| x86 execution | Normal interpreter, 386-class, no FPU | More CPU models, interpreters, dynamic recompilers, FPU |
| Host-directory C: | Writable files and nested directories, restricted 8.3 names | Broader filesystem and mounting support |
| Guest RAM | 1, 2, 4 or 8 MiB; same 32 MiB host allocation | Wider configurable range |
| Real DOS boot | Read-only 1.44 MiB floppy | Floppy and hard-disk images, IDE and installation workflows |
| CONFIG.SYS | Reference editor in embedded DOS; booted DOS executes its disk's file | Embedded configuration facilities and real DOS boot |
| Extended memory | Booted MS-DOS can load HIMEM; embedded XMS/EMS pending | Embedded XMS/EMS/UMB support |
| PC audio / MIDI | Pending | Sound Blaster, speaker, MIDI and more |
| Mouse / joystick | Pending | Supported |
| Save states | Pending | Supported, with version/configuration constraints |

The same original DOS file/handle/seek and MZ relocation fixtures passed in
both our native core and DOSBox-X. The DOSBox-X runs used `core=normal` and
`core=dynamic_rec`, 386 CPU, 2 MiB RAM and 3000 fixed cycles. These are
functional comparisons on the Mac, **not A9 performance measurements**.
No FPS advantage for DOSBox-X has been established.

### Changes made after the comparison

The file transport now carries validated relative directory components.
`MD`, `CD`, `RD`, directory listings, nested files and cross-directory renames
use the real AmigaDOS service. Components are checked before locking, links
are rejected, and the service never accepts an arbitrary Amiga path. Limits:
256-byte canonical DOS paths, 16 open files, 64 distinct search directories
per session, and 4096 enumerated entries per directory. Names currently admit
letters, digits, `_`, `-` and one extension dot; they are not the full DOS
filename character set. Read/write permissions and the existing bounds remain.

The existing upstream BIOS disk implementation is now staged into the PC
build. One immutable 1.44 MiB floppy is loaded through the file bridge into
application-owned ARM RAM, then served by BIOS INT 13h. Writes and formatting
return write-protection errors. Reads check image bounds; boot requires the
exact supported image size and a boot-sector signature. No host image is
modified, and booted DOS cannot see the host-directory C: drive. The launcher
adds a disk selector and **Boot disk** button, with F10 returning to MUI.
`--boot FLOPPY.IMG --drive drawer --memory 8` provides the same CLI path.

The supplied boot disk reached MS-DOS 6.22's A: prompt in the native build.
`VER` and `MEM` confirmed DOS 6.22 and 8,192 KiB total memory, including
7,168 KiB extended memory managed by the floppy's HIMEM driver. This does not
establish EMM386, protected-mode game compatibility, or a hard-disk installation.
The disk's CD-ROM driver reports no drive; IDE/CD-ROM emulation is absent.
Booted DOS uses its on-disk AUTOEXEC.BAT and CONFIG.SYS; the launcher editors
do not alter an immutable boot image.

### Useful DOSBox-X work to carry forward

1. **Image/drive ownership and boot separation.** Adopted the explicit separation
   between host folders and BIOS images. The initial read-only boot path uses
   code already present in our pinned upstream, rather than importing a second
   disk subsystem. Writable FAT16/IDE needs exclusive ownership and flush/error
   handling before it can be shared with Workbench.
2. **ARM dynamic recompilation.** The largest likely CPU opportunity, but still
   unmeasured on the A9. Both upstreams' ARMv7 cache-close code uses Linux
   `sys_cacheflush` SWIs. Bare-metal XACP needs a bounded executable code cache,
   ARM cache maintenance and self-modifying-code tests. Enabling a build macro
   alone is unsafe and does not constitute a working port.
3. **Render-on-demand.** DOSBox-X can coalesce scanline rendering, while flushing
   up to the emulated beam position before relevant register/memory changes.
   This is relevant to our measured PIC/VGA cost. Copying only its end-of-frame
   loop would lose mid-frame effects; port it with those write hooks and EGA,
   CGA, palette, scrolling and split-screen regression cases.
4. **Compatibility profiles.** Retain the small MUI launcher and expose only
   features backed by the current core. XMS/EMS, audio batching, mouse input
   and eventually save states are more useful near-term additions than the
   full desktop DOSBox-X UI and its PC-98/Windows/peripheral surface.

No DOSBox-X source has been merged into the runtime. Preserve DOSBox/libretro
and Xanai/XACP attribution; any future copied DOSBox-X code must retain its
component licence notices.

Primary references: [DOSBox-X](https://github.com/joncampbell123/dosbox-x),
[CPU settings](https://dosbox-x.com/wiki/Guide:CPU-settings-in-DOSBox%E2%80%90X),
[MS-DOS installation](https://dosbox-x.com/wiki/Guide:Installing-MS%E2%80%90DOS-6),
[render-on-demand implementation](https://github.com/joncampbell123/dosbox-x/blob/378e6f8a25768594df87c9f5c99b5df9b3f6a8b0/src/hardware/vga_draw.cpp),
[ARM cache code](https://github.com/joncampbell123/dosbox-x/blob/378e6f8a25768594df87c9f5c99b5df9b3f6a8b0/src/cpu/core_dynrec/risc_armv4le-common.h).

The larger guest-RAM/floppy build initially failed the linker allocation bound.
Upstream's full software TLB occupied 21,103,704 bytes on ARM. The PC build now
selects upstream's existing banked TLB (`USE_FULL_TLB` disabled), retaining full
32-bit guest address coverage with lazily allocated higher banks. This is not
an unchecked reduction of address-table indices. The 32 MiB Exec allocation,
shared-mailbox layout and runtime mapping checks remain unchanged. The linked
ARM suite includes an original high-address guest fixture to check rejection
inside the core. This configuration is documented upstream as compatible with
the normal interpreter and dynrec, but not the x86-specific dynamic core.

Also corrected `WA_CustomScreen=NULL` on unthemed console launches: omit the
custom-screen tag when using Workbench. The configured/themed launcher path
already worked. Keep the failing baseline CLI log as evidence of why the
comparison runs use identical launcher session files.

### Final validation and memory/performance trade-off

Both the ARM/68k runtime and MUI launcher build as `0.7.0-dev`. The 68k build
uses `-m68020` and treats its compiler warnings as errors. The linked ARM suite
passes both allocation relocations, both framebuffer formats, exact EGA pixels,
CGA/keyboard/options, malformed options/nonces, read/filesystem/copy-time stop,
BIOS read/write-protection/bounds and unsupported guest addresses. Native ASan
runs pass the shell/DOS fixtures, supplied DOS boot with HIMEM, and bounded Keen,
Vette and Elite startup. These native runs are not Amiga performance evidence.

The banked TLB now occupies **1,442,964 bytes**, saving **19,660,740 bytes
(18.75 MiB)** against the old full table. High banks still need space in the
bounded 2 MiB runtime heap: the deliberately unsupported `0xf0000000` guest
access fails with `Out of Memory`; the first-bank address beyond guest RAM
fails with `Guest page bound`. Both unwind inside the worker. Do not interpret
this as general high-address/protected-mode compatibility.

A further local patch moves high-bank lookup/allocation into a non-inlined
helper, keeping that rare path out of each normal memory access. ARM image
size drops from **731,464 to 619,856 bytes**. An initial inline-bank Keen run
needed 15,829 ms; the revised code needed roughly 15,229 ms. The inline result
is one exploratory sample, not a three-run comparison.

Three matched hardware trials per final variant used the same Keen profile:
2 MiB RAM, VGA, 3000 cycles, identical scripted Enter, 8000 emulated milliseconds.
Each produced 558 frames and 246,596 interpreter slices, with the same console
hash and file-read totals.

| Build | Median wall time | Mean delivered FPS |
|---|---:|---:|
| Previous 0.6.0-dev, full TLB | 14,346 ms | 38.91 |
| Final 0.7.0-dev, banked TLB with rare path separated | 15,229 ms | 36.63 |

The final build takes **6.16% longer** in this startup workload. The larger RAM
and boot support are compatibility/memory improvements; they are not a net FPS
improvement over 0.6.0-dev. Keep the previous executable for comparison. These
figures do not measure steady gameplay and say nothing about DOSBox-X on A9.

Physical testing additionally verified nested directory creation, file write,
read and removal on the shared C: drawer. MS-DOS 6.22 boot, real keyboard
`VER`/`MEM`, 8 MiB RAM reporting and F10 exit passed on the A9. The MUI Boot disk
button was exercised through launch, guest input and return to MUI. Checked
registers/cache witness were restored and owned memory released. Disk media
was read-only throughout. No firmware or system startup changes were made.

Evidence: `amiga/records/2026-10-12/dosbox-07/`. The `inline-tlb/` subdirectory
retains the earlier banked candidate and its checks; final build records and
repeat timings are separate. The same launcher binary was used for both.
Remaining milestones are writable BIOS hard-disk images and a real DOS hard-
disk installation, embedded XMS/EMS, PC audio/input devices and a validated
bare-metal ARM dynrec. Full DOSBox-X replacement is not required for these.

## Writable BIOS hard disk — 13 October 2026 (0.8.0-dev)

Adds one fixed 32 MiB raw image: 128 cylinders, 16 heads, 32 sectors, 512-byte
sectors. `--hdd DOS622.IMG` boots its MBR; combining `--boot FLOPPY.IMG` with
`--hdd DOS622.IMG` boots the read-only floppy with the hard disk attached.
Both image names are checked 8.3 names beneath `--drive`. Existing host-folder
C: remains a separate embedded-DOS mode. Images and licensed DOS files are not
part of the public source or release.

The original DOSBox BIOS services call bounded sector RPCs. The 68k opens an
existing, exactly sized image using `Lock(EXCLUSIVE_LOCK)` then `OpenFromLock`,
without truncation. File links and image paths outside the selected drawer are
rejected. Each write checks its full length and flush result; host teardown
flushes/closes the image after ARM ownership has ended. No shared DDR layout,
firmware or allocation size changes. Multi-sector BIOS requests may cross a
track, as MS-DOS SYS requires, but cannot access outside the image. BIOS write
errors now return their actual status instead of leaving AH at function 03.

Workbench uses the existing diskimage.device/fat95 stack on reserved unit 5,
`DOSHD:` with FAT95 primary-partition discovery. The new `ZZDosDisk` helper
flushes/inhibits FAT95 before changing media, operates on **raw UNIT 5**, then
uninhibits it. The first physical mount attempt used `DRIVE DOSHD:` while the
filesystem was inhibited; this likely blocked DiskImageCtrl resolving the
filesystem and subsequently blocked bridge access. No ARM job had started.
The user reported a Guru during the subsequent restart; the rebooted bridge
retained no crash record, so its exact cause is unconfirmed. The helper now uses
UNIT 5, checks device registration without resolving a filesystem path, places
its path buffers in static storage, checks an 8 KiB minimum stack, gives child
commands 32 KiB and logs each mount step. Its compiled main stack frame is
76 bytes (previously approximately 8 KiB of local path arrays alone).
Read-only mount/browse/eject passed on the real Amiga before writable testing.

Completed native/model evidence:

- Actual filesystem backend under ASan/UBSan: exclusive/shared lock conflict,
  no truncation when another opener is refused, sector/count bounds, last-sector
  persistence after reopen, alongside the existing directory/file tests.
- Original BIOS HDD fixture on linked ARM code: geometry, cross-track read/write,
  last valid sector and rejection beyond the end. Full existing ARM PC suite
  still passes, including immutable floppy and mapping/cancellation tests.
- MS-DOS 6.22 SYS from the supplied installation disk transferred the system
  using the native build of our BIOS. Subsequent hard-disk boot accepted VER/MEM;
  writes to CHECK.TXT survived two separate processes. HIMEM reports 7,104 KiB
  free XMS with 8 MiB RAM. `DOS=HIGH` stalls and is excluded from the supplied
  CONFIG.SYS; EMM386 is present among DOS utilities but not enabled/validated.
- DOS utilities were expanded with the supplied Microsoft's EXPAND.EXE under
  Mac DOSBox-X; expanded lengths match the compressed headers. This is a
  prepared DOS installation, not a completed interactive Microsoft SETUP run.
- Native sanitized embedded-DOS/EGA fixture, read-only floppy boot and HDD boot
  pass. `make test-amiga-arm test-amiga` passes (138 MCP tools).

The image contains DOS utilities and copies of the owner's supplied Keen,
Vette and Elite files. Its compressed transfer passed CRC validation; gzip
reported success and produced 33,554,432 bytes on the Amiga. The bridge's
whole-image CHECKSUM request timed out; do not describe that request as a pass.
No FPS benchmark has been run for this candidate.

Physical A9 validation after recovery passed:

- DOS boot was refused while Workbench held the image, with clean ARM teardown.
- Workbench mounted and ejected the FAT16 disk through the corrected helper.
- A9 DOS boot identified MS-DOS 6.22; HIMEM reported 7,104 KiB free XMS at 8 MiB.
- A raw diskimage.device open on another unused unit was refused with
  "object is in use" while the BIOS held its exclusive image handle.
- DISKTEST wrote PERSIST.TXT through BIOS sector writes. F10 returned with
  successful flush/close, register/cache-witness restoration and memory release.
- A new ARM process read the same text; Workbench subsequently read it too.
- The MUI controls passed mount, refusal to boot while mounted, eject, HDD boot,
  VER, F10 return, test-file cleanup and final eject. No Core1 owner remains;
  the launcher is on Programs. Both installed runtime/UI CRC32 match the builds.
  The crash-inducing path was not repeated; the exact restart Guru cause
  remains unknown despite the successful corrected path.

Local work/evidence: `.context/amiga/dosbox-hdd/`. Previous installed binaries
and configuration were copied to `ZZDosBox-PreHDD`, `Launcher-PreHDD`,
`Launcher.cfg-PreHDD`. The canonical runtime/UI are now 0.8.0-dev;
`ZZDosBox-HDD` retains the matching candidate for comparison.

AmigaDOS ownership reference: [ROM Kernel Reference Manual: DOS, section
6.2.3](https://developer.amigaos3.net/sites/default/files/downloads/2024-10/Amiga_ROM_Kernel_Reference_Manual_DOS.pdf).
Media transfer used [gzip 1.2.4 respin 2](https://aminet.net/package/util/pack/gzip124x2)
in RAM only; no system library or startup changes.

## Installed 0.8 game and profile checks — 13 October 2026

Tested the existing executable through the MUI launcher, with no binary or
firmware changes. Keen reached its world map; arrows moved the character and
scrolled the map. Vette with LOADFIX reached car, difficulty, opponent and
course selection, then asked its original manual question. Driving remains
unverified. Elite with LOADFIX reached Commander Jameson's status and the
launch/front-view screen; extended flight remains unverified. All three F10
exits restored the checked registers/cache witness, released owned memory and
returned to MUI. No new Guru was observed; the bridge had no crash record.

A temporary fifth profile tested Save and reload in a fresh launcher process:
8 MiB, EGA, 1000 cycles, green on black and an edited AUTOEXEC. Saved bytes
matched, the reloaded UI displayed these values, and Run AUTOEXEC printed a
unique marker and exited normally. The runtime log confirmed the RAM/video/
cycle options. This shell test does not establish EGA rendering or exercise
all guest memory. The original four profiles were restored, with Keen selected
for the final launcher. Temporary test profiles are not retained.

### Controlled startup timing

Same installed 0.8 binary, 2 MiB guest RAM, VGA, 3000 cycles, serial presentation,
1 ms host poll, 8000 simulated milliseconds. The timed harness injects Enter
at 4000 ms and releases it at 4100 ms. No screenshots or manual input during
these runs. File loading before the host's reported interval is not included
in `wall_ms`; the interval covers its guest run/service loop. These are
refresh/presentation rates, not necessarily distinct game images or steady
gameplay FPS. Instrumented timings are kept separate.

| Game | Trials | Generated/presented frames per trial | Wall time | Refresh FPS | Emulated speed |
|---|---:|---:|---:|---:|---:|
| Keen 6 | 3 | 558 / 558 | 15.000 s median | 37.168 mean | 53.3% at median |
| Vette + LOADFIX | 1 | 558 / 558 | 20.038 s | 27.847 | 39.9% |
| Elite + LOADFIX | 1 | 559 / 559 | 14.114 s | 39.606 | 56.7% |

Keen wall times were 15.000, 14.989 and 15.050 s. The earlier 0.7 results
averaged 36.627 FPS, but this is a later-session baseline, not a matched
before/after optimisation experiment. No speedup is claimed.

Separate profiling runs retained identical frame counts and clean teardown.
Exclusive ARM timer fractions:

| Phase | Keen | Vette |
|---|---:|---:|
| x86 interpreter and guest-memory handlers | 38.0% | 36.6% |
| PIC/timer/VGA events | 18.7% | 23.1% |
| Scanline conversion | 14.6% | 13.4% |
| Waiting for a framebuffer grant | 12.6% | 9.1% |
| Framebuffer mapping/scaling/copy | 11.5% | 8.9% |
| Other, including file service | 4.5% | 9.0% |

Next work should isolate interpreter memory-access and event-dispatch overhead
with matched native/ARM correctness tests and physical timing. The existing
PAN acknowledgement and memory-ownership checks remain required. Display
frame skipping is a separate optional tradeoff: it could reduce rendering
cost, but would not establish equal visual output or faster CPU execution.
Audio remains unimplemented and needs its own measured buffering budget.

Raw logs, screenshots and machine-readable results:
`amiga/records/2026-10-13/dosbox-08-games/`. No ROM/game/DOS media is included.

## Scheduler optimisation experiments — 13 October 2026

Tested two ways to reduce repeated soft-float deadline calculations in the
upstream PIC queue. Both retained original event timing and ordering. The first
cached `entry.index * CPU_CycleMax`, invalidating on event reuse, tick rebasing
or cycle-budget changes. The second additionally used equivalent floor/ceiling
integer deadlines within a proven nonnegative range, falling back to original
float operations elsewhere. Neither changes emulated cycles or skips frames.

Native differential tests exercised the real PIC queue's equal deadlines,
negative/fractional delays, removal/reuse, recurring handlers and CPU-budget
changes within callbacks. Boundary/random deadline tests and cached lookups
checked the original arithmetic on every native lookup. Queue traces and
game/DOS results matched the reference, including final image hashes. ARM
instruction tests, physical EGA/PIT, and physical game teardown also passed.

Matched physical tests: same 2 MiB/VGA/3000-cycle profiles and 8000 simulated
milliseconds as above, alternating baseline/candidate order, no screenshots.
Every timed run produced 558 frames and completed the restoration checks.

| Candidate | Trials per game/build | Keen wall ms, baseline → candidate | Vette wall ms, baseline → candidate |
|---|---:|---:|---:|
| Product cache | 2 | 15030.5 → 15118.5 (means) | 20062.5 → 19818 (means) |
| Integer deadline cache | 3 | 15032 → 15324 (medians) | 20047 → 19913 (medians) |

**Rejected as defaults:** both slowed Keen and only marginally helped Vette.
Production scheduler source and installed runtime remain unchanged. Experiment
sources, patches and logs are retained under
`amiga/records/2026-10-13/dosbox-scheduler/` for reproducibility.

A separate CPU-only `-O3` build is available via `--pc-cpu-opt O3`; `-O2`
remains the default. The O3 build, ARM instruction model and physical EGA/PIT
lifecycle passed. Two matched trials per game/build found no gain:

| Game | O2 mean wall ms | O3 mean wall ms | O2 → O3 refresh FPS |
|---|---:|---:|---:|
| Keen | 15034.5 | 15100 | 37.115 → 36.954 |
| Vette | 20075 | 20156 | 27.796 → 27.684 |

Both O3 runs per game were slower than both baseline runs. With only two
trials and a roughly 0.4% difference, this does not establish a general compiler
regression; it gives no reason to promote O3. All eight runs produced 558
frames/8000 simulated ms and passed register/cache restoration and memory
release. These remain startup refresh measurements, not steady gameplay.
The original 0.8 runtime is still installed; O3 remains a separate experiment.

During icon capture, an unrelated injected Workbench sizing gesture stalled
Intuition/bridge while no Core1 application was active. The bridge source
explicitly documents this modal drag/size hazard. Following a physical-button
release request, communication recovered and the owner confirmed recovery;
a button-up cleared the remaining injected state. No reboot was observed and
no crash data was recorded. The O3 staging file was then independently
checksummed before execution. Avoid synthetic Workbench icon/drag/size gestures.
Final checks found the bridge responding, launcher present, no Core1 owner,
and the installed baseline unchanged (CRC32 1FCAF3E4).

The supplied DOSBox icon was installed and independently read back before the
stall. It is a classic 64x64 Workbench image retaining the launch stack and
metadata; no extra icon library is required beyond the target's existing OS.


## Workbench window / DOS selection / ARM FPS — 0.9 development

Both guest text mode and graphics use the same 640x480 presentation path.
Embedded shell output mirrors into the VGA text buffer; BIOS keyboard input
and CALLBACK_Idle keep keyboard interrupts and VGA refresh running while the
shell waits. The host console-device screen is no longer opened by this PC
frontend. Older diagnostic/shared console components remain available.

Window mode allocates an offscreen Picasso96 bitmap on ZZ9000, locks and
challenges its mapping before each ARM write, then unlocks after completion
and uses a clipped BltBitMapRastPort/WaitBlit to the Workbench window. The ARM
never maps Workbench's screen bitmap. The current window path requires a
32-bit RTG Workbench. Fullscreen retains XACP deferred PAN/acknowledgement
and slot ownership. F10 or the window close gadget requests cooperative stop;
allocations survive until synchronous Core1 reset. Focus loss releases keys.

The optional overlay is calculated and drawn on Core1 after scaling, without
modifying guest pixels. It reports completed video refreshes over a rolling
real-time half-second interval, using XX19c's nominal 333.333 MHz global timer.
It is not guest simulated FPS, distinct game-image count, or a 68k UI timer.
The counter includes normal rendering/presentation stalls. Screenshots can
pause execution and temporarily lower its displayed value.

Session/profile format 2 saves display, FPS and DOS choice. Version 1 imports
preserve fullscreen with FPS off and embedded DOS. New profiles default to
window mode. The added MS-DOS terminal profile boots the existing DOS622.IMG;
exclusive image ownership still applies. Booted DOS executes its own startup
files. Commands entered for MS-DOS refer to files inside its disk, not Amiga
host drawers; the boot command waits for the root C:\> prompt before keyboard
injection. No DOS media or system preferences are replaced.

Physical acceptance passed for both DOS choices in windows, Keen window
output, FPS enabled/disabled, and MS-DOS fullscreen. F10 and the Workbench
window close gadget returned cleanly. MUI saved options survived a restart and
Open terminal launched the selected installed DOS. Register/cache restoration,
owned-memory release, native ASan and full ARM-model tests passed. Installed
as 0.9.0-dev; evidence: `amiga/records/2026-10-13/dosbox-window/`.
Earlier timings above remain historical, not a new performance claim.

## Resizable Workbench window — 0.9.1-dev

The DOS terminal tab now groups DOS selection, Display and Window size. Show
FPS is in Settings. The three starting sizes are 320×240, 640×480 and 960×720. Fullscreen
ignores the window preset and disables its control. Profile/session format 3
stores the preset; versions 1 and 2 import with 640×480. Programs, terminal
sessions and disk boot all use the selected display settings.

The native Intuition size gadget adjusts the client between 320×240 and
960×720. Each new frame is fitted to a centred 4:3 rectangle, with black
margins. A stable, owned 960×720 P96 bitmap avoids allocation changes during
resize. The host publishes fitted width/height at video-mailbox +44/+48 before
the grant sequence. Core1 validates those dimensions, pitch, aperture and both
nonce endpoints before temporarily mapping and drawing. Only a completed,
unmapped frame is blitted into the OS-clipped window. If the window shrinks
while a frame is in flight, the completed old frame is cropped to the new
client until the next grant uses its geometry. Fullscreen remains 640×480.

Local checks cover profile migration, invalid presets, every 4:3 output width
from 320 to 960, FPS bounds, mapping limits and 354,801 client geometries.
The ARM instruction model alternates 320×240, 960×720, 508×381 and 640×480,
checks exact EGA output including untouched row padding, and confirms mapping
revocation in both 32-bit formats. Native ASan and the shared ARM/MCP suites
also pass. These checks do not establish physical drag-resize behaviour.

Physical acceptance passed for all three starting sizes, raw keyboard VER,
FPS and F10 exit; MS-DOS 6.22 fullscreen; saved MUI presets and disabled size
control in fullscreen; and MUI-launched 6.22 at 640×480 with the native close
gadget. Each run restored register/cache witnesses and released ownership.
Show FPS was subsequently moved back to Settings at the owner's request;
that layout-only launcher rebuild is installed. Physical dragging of the
size gadget remains a user check: bridge-injected window sizing previously
stalled Intuition, so no synthetic resize gesture was used. Evidence is in
`amiga/records/2026-10-13/dosbox-window-sizes/`.

## Startup files, data CDs and external MIDI — 13 October 2026

The 0.10 development change groups Terminal, AUTOEXEC.BAT, CONFIG.SYS and
CD-ROM beneath DOS terminal. Each startup file selects Default, Edited or
an Amiga file. DOS 6.22 defaults are read from its actual disk. Custom startup
files temporarily replace the disk versions for a session, with reserved
backups and a recovery marker; normal return restores the originals. Embedded
DOS executes the selected batch file but does not process CONFIG.SYS.

The owner's [CONFIG.SYS/AUTOEXEC guide](https://ctrl-alt-rees.com/archive/madsenworld.dk/con_auto/index-uk.htm)
is a useful reference, especially its DOS 6.22 section and matching CD driver
`/D:` names. Windows/DOS 7 examples are not defaults for this port. This change
does not add EMM386 or DOS=HIGH support.

Data CD sources are an ISO9660 image or a mounted Amiga CD device, exposed
read-only as D:. The host uses bounded 2048-byte reads; physical reads have a
five-second timer and use the mounted device's actual driver/unit. It never
writes/ejects the CD or reconfigures Poseidon. Embedded DOS uses the pinned
DOSBox ISO/MSCDEX implementation. Booted DOS uses the original ZZDOSCD.SYS
request gateway plus the user's installed Microsoft MSCDEX.EXE. Audio CDs,
CUE/BIN tracks and hot media changes are outside this version.

Two integration failures were found in native tests: read-ahead request 82h
must not write a caller buffer, and an ISO test fixture needs valid path tables
for real MSCDEX. Both were corrected before target deployment. The bare-metal
ISO path walker uses strtok_r with caller-owned state instead of libc's TLS
strtok. No DOSBox firmware changes are involved.

MIDI settings distinguish an external Roland MT-32 (intelligent MPU-401 at
330h, upstream IRQ9) from external General MIDI (UART at 330h). The ARM runs
pinned DOSBox MPU-401 code; a bounded 4096-byte shared ring carries MIDI bytes
to CAMD on the 68k. CAMD retains serial/USB-device ownership. The output is
selectable, default out.0, and off by default. Host batches respect serial
MIDI wire rate and apply a conservative 300ms processing gap after MT-32 SysEx.
Queue backpressure is cancellable; no silently dropped bytes. Exit cancels
sustain and sends All Notes Off to the selected output.

This is **external MIDI routing, not an MT-32 software synthesizer**. No Roland
ROM files are needed for a real module. Choose Roland MT-32 in the game's
own setup too; General MIDI and MT-32 instrument mappings differ. The
[DOSBox sound documentation](https://www.dosbox.com/wiki/Sound) describes this
separation between emulated PC hardware and an external synthesizer.

Sound-card follow-up: connect a PCM mixer/AHI pipeline for PC speaker and
AdLib/OPL first, then Sound Blaster with DMA/IRQ and separate MIDI music.
Tandy needs its own device work. The screenshot's IBM Music Feature option
is not implemented. These devices must not appear as working launcher choices
until their guest I/O and output paths are tested. AHI/ZZ9000AX playback is
separate from the real MT-32's analogue audio output.

Validation: native ASan and ARM instruction-model CD/MIDI fixtures pass,
including cross-sector ISO copy, intelligent/UART handshake, running
status/SysEx byte preservation and ring wrap/backpressure. Startup transaction
fault tests preserve present/absent originals and recover interrupted renames.
Physical embedded DOS read the original test ISO and listed AMINET13 through
UMSD: (`usbscsi.device` unit 0, ISO9660, 325420 sectors). The physical MPU-401
fixture delivered 5015 bytes into an internal CAMD capture: 5005 messages,
three notes, one SysEx and no capture errors. No external Roland notes were
sent, and audible playback remains untested.

The MUI File selection physically booted MS-DOS 6.22, executed the selected
AUTOEXEC, loaded Microsoft MSCDEX 2.23 and read the test ISO as D:. Return
restored both original disk startup files byte-for-byte and ejected DOSHD.
The original launcher profile was restored; MIDI and CD are off by default.
The custom 87-byte ZZDOSCD.SYS remains installed in the DOS disk root for
future CD sessions. The full pre-test disk image and previous binaries/config
are preserved on the Amiga. These runs passed register/cache witnesses and
owned-memory release. The startup run used runtime 0.10.0 and UI 0.10.1;
display-control follow-up is recorded separately below.

The 0.10.1 follow-up adds the requested “DosBox ZZ9000 project” welcome text,
project URL, version and AmigaOS/32-bit identification. The Cortex-A9 is
ARMv7-A, not an AArch64 processor; the host build's bitness does not imply
64-bit x86 guest support. The MUI About button uses an existing OpenURL
installation if available.

Live controls: Alt+Return changes display; Ctrl+minus/plus requests CPU cycles
in 500-cycle steps, bounded to 500–10000 cycles/ms. ARM acknowledges speed at
a millisecond tick boundary. Higher requested cycles do not increase physical
CPU capacity. Display switching waits for completion/unmapping and a completed
PAN acknowledgement, retains both OS bitmap allocations, and frees them only
after the existing synchronous Core1 reset. No firmware change is involved.

Settings can inspect CAMD version and the selected cluster, AHI version and
advertised ZZ9000AX AHI modes. The detector opens AHI without a playback unit;
it does not allocate audio channels, play test tones or change preferences.
An available mode/cluster is not proof of connected speakers or an MT-32.

### Idle-CPU research

The owner highlighted DOSBox Staging's `cpu_idle` option. Its
[CPU manual](https://www.dosbox-staging.org/0.83/manual/system/cpu/)
describes guest HLT-based idling, an off switch for third-party DOS power
management tools, and reduced desktop host load with maximum cycles. That
desktop percentage is not a ZZ9000 measurement or a promise of game speedup.

Source review of this port finds an existing `CPU_HLT`/`HLT_Decode` emulator,
but `CALLBACK_Idle` and blocking BIOS INT 16h still execute short NOP loops.
The embedded VGA prompt calls `CALLBACK_Idle` while awaiting keyboard input.
Thus guest idle-loop work is a candidate for reduction. The bare-metal ARM
also polls shared-memory handshakes, and the 68k services display, files and
MIDI with a timed polling loop. Emulating guest HLT does not automatically
sleep either physical processor.

Follow-up: prototype an optional guest idle path, compare interpreter work
and prompt responsiveness on/off at matched cycles, then measure host polling
separately. Preserve timer IRQs, cursor blink, keyboard wake-up, cancellation,
MIDI delivery and display ownership. Do not introduce ARM WFI/WFE without a
verified XACP wake-up contract. Test embedded DOS and booted 6.22 separately;
Windows 3.1 and WQGHLT compatibility are unverified. No `cpu_idle` control is
implemented in 0.10.1.


### Display controls and welcome acceptance

Physical 0.10.1 display tests passed four mode changes per session, starting
and exiting once in a Workbench window and once in fullscreen. CPU hotkeys
requested 3500 then 3000 cycles/ms and the ARM acknowledged 3000. Each exit
restored register/cache witnesses and released owned memory. Early automation
sent later keys before Intuition had completed focus changes; clicking the
window allowed clean exit. The final implementation defers activation until
after the asynchronous screen switch and outside bitmap locks. Acceptance
waits for the actual active window/screen before sending the next key, rather
than treating a captured image as proof of input focus.

Settings physically detected CAMD 37.14 with out.0, AHI 4.180, and two
ZZ9000AX modes. Default startup-file pages were checked after disk reads
completed and showed the actual DOS disk files. Initial captures taken while
those reads were pending still showed the previous editor text.

The owner's extended welcome template is adapted through an original DOS
helper, `ZZHELLO.COM`, called by the temporary startup batch. It draws a blue
panel, CP437 double-line border, ASCII ZZDosBox logo, version, detected DOS
version, project URL and real shortcuts. It uses text-mode video memory and
BIOS services; no ANSI.SYS, Windows COLOR command, fake Windows/NC choices or
unsupported load-high configuration is introduced. User-selected AUTOEXEC
commands still execute, and both original disk startup files are restored.
The helper is installed only when absent, or reused when byte-identical;
conflicting files are preserved and reported. Native ASan validation checks
the border character/attribute, DOS version, title, controls and prompt return.

The final MUI launch passed physically in both embedded DOS 5 (fullscreen)
and real MS-DOS 6.22 (Workbench window). Screenshots show the new panel and
correct DOS version; both exits passed register/cache restoration and owned
memory release. The original AUTOEXEC/CONFIG files were checked byte-for-byte
after the booted-DOS run. The helper is also present in the embedded C: drawer.
Final binary checksums and evidence are under
`amiga/records/2026-10-13/dosbox-startup-cd/`. No physical resize gesture or
external MIDI instrument playback was tested.


## Windows 3.11 and physical WinBench — 13 October 2026

The Windows experiment now boots on the physical ZZ9000 A9. Fast-A20 port 0x92,
ROM write protection, upstream full-core page-fault recovery and smaller sparse
TLB banks resolved the installation/startup blockers. The bounded allocation
remains 32 MiB with 8 MiB guest RAM. The tested DOS runtime remains available
separately; Windows has its own executable and disk.

WinBench 3.11 Graphics WinMark completed at **1,923,752 pixels/sec**, standard
VGA 640x480/16 colours and 3000 cycles/ms. Its displayed 37 MHz 80386 estimate
uses the guest clock; no real PC speed equivalence is established. Command-to-
result observation took at most 267.25 real seconds, including UI/capture delays.
Windows exited to DOS and the emulator released its owned resources with zero
register/cache-witness mismatches. The full ARM model regression also passed.

See [test evidence](../amiga/records/2026-10-13/dosbox-wfw/findings.md).
Guest mouse, PCM audio and Windows networking remain pending. No Microsoft
software, WinBench or guest disk image is distributed with the project.

## ARMv7 dynamic recompiler experiment — 13 October 2026

A separate `--pc-dynrec` build enables the pinned upstream x86-to-ARM JIT.
Its generated-code arena is bounded inside the existing owned allocation;
Core1 performs local instruction-cache publication instead of the upstream
Linux syscall. The real A9 passed generated-code rewrite and guest
self-modification checks, then fullscreen EGA/PIT with clean restoration.
The same Doom Demo3 workload improved from approximately **3.10 to 6.94 real
FPS (2.24×)**, with all 2134 ticks completed and clean teardown. This is one
physical run per build; broader compatibility remains unproven.

Build instructions, model coverage and physical logs are in
[`dosbox-dynrec/method.md`](../amiga/records/2026-10-13/dosbox-dynrec/method.md).
At the owner’s request, the tested dynrec binary now occupies the normal
launcher’s runtime path, with the interpreter preserved as a fallback. Windows
and broader game compatibility need separate tests.


## NEON pixels, 4 MiB cache and display overlap — 13 October 2026

A separate candidate adds checked integer NEON capture/scaling, a 4 MiB JIT
cache and bounded overlap of a spare-buffer write with the preceding PAN
acknowledgement. Total owned allocation remains 32 MiB; benchmark guest RAM
remains 8 MiB. The code-cache metadata and heap budget grow within that bound.

Real Doom Demo3 results: initial dynrec 6.94 FPS, overlap 6.96 FPS, larger cache
plus overlap 6.97 FPS, and NEON plus both 7.29 FPS. The approximately 5% total
gain comes predominantly from the NEON candidate; the other single-run
differences are too small to establish a gain. This puts the tested workload
within Thandor's 386DX-33 results (6.89–7.47 FPS), not a general PC equivalence.
All timed runs used 10000 cycles/ms and cleanly restored ARM state and released
memory. Model checks cover pixel guards, JIT bounds, SIMD context restoration,
self-modification, paging and display ownership.

The separate 3000-cycle interactive check switched into a Workbench window,
but injected return/exit keys did not work; CLI cancellation was clean. The
physical-key repeat passed both switches and F10, with clean restoration and
memory release. The tested candidate is now the normal launcher runtime,
with initial dynrec and interpreter fallbacks preserved. See
[methods, logs and limitations](../amiga/records/2026-10-13/dosbox-tuning/method.md).

## Software Classic scanline copy experiment — 13 October 2026

A separate experimental build measured Doom Demo3 at 7.3281 FPS normally,
7.4409 FPS with black rows rewritten each frame, and 7.5970 FPS with those
rows initialized once per buffer then preserved. The last result is +3.67%
vs normal and +2.10% vs rewriting the same mask; single runs, so preliminary.
This reduces final ARM framebuffer-copy work, not guest CPU/VGA rendering.
It is independent of the Workbench FPGA scanline controls.

All timed runs restored state and released memory. Separate physical F9
Full -> Skip -> Off captures verified black rows and restored normal output;
F10 exited cleanly. The user was not watching; screenshots do not establish
tear-free monitor scanout. Native/ARM-model checks also passed. The normal
installed executable remains unchanged; the candidate is RAM-only.
See [method and raw results](../amiga/records/2026-10-13/dosbox-scanlines/method.md).

## DOSBox Staging manual review — 13 October 2026

The linked 0.83 documentation is marked as a development website. Its
configuration names and defaults are not a drop-in interface for this
libretro-derived, bare-metal XACP port.

- [CPU](https://www.dosbox-staging.org/0.83/manual/system/cpu/): investigate
  finer per-profile fixed cycles, an interpreter compatibility choice, then
  mode-specific budgets and idle yielding. Current launcher presets remain
  1000/3000/6000, with runtime adjustment 500–10000. Staging's desktop defaults
  and approximate CPU-equivalence table do not establish A9 throughput.
- [DOS](https://www.dosbox-staging.org/0.83/manual/system/dos/): candidates
  include reported-version overrides, shell history and compatible file locking.
  Reporting 6.22 would not replace our separate booted MS-DOS installation.
- [Graphics adapters](https://www.dosbox-staging.org/0.83/manual/graphics/adapters/):
  S3/VESA and Tandy/PCjr/Hercules are compatibility candidates beyond current
  CGA/EGA/VGA. Audit upstream code, memory bounds, BIOS and renderer coverage
  before exposing them. Emulated video-memory delay is a compatibility cost,
  not a framebuffer throughput optimisation. Software-emulated Voodoo or
  ReelMagic does not itself provide ZZ9000 hardware acceleration.

No additional CPU, DOS or adapter feature was enabled by this review.
Normal/Adaptive controls final presentation, independently of guest CPU cycles.

## Graphics adapter / rendering references — 13 October 2026

The Staging [aspect-ratio guide](https://www.dosbox-staging.org/0.83/manual/graphics/rendering/aspect-ratios-and-scaling/)
explains why 320×200 and Hercules 720×348 require non-square-pixel correction.
Our existing output fits a 4:3 rectangle with black window borders, including
Hercules; no square-pixel or integer-scaling selector was added in this change.

The [colour guide](https://www.dosbox-staging.org/0.83/manual/graphics/rendering/colour-and-image-adjustments/)
informed the four monochrome labels. Our Hercules palette approximations are
independent of its OpenGL colour-management path. CGA monochrome (`cga_mono`)
is not a new machine option in this release.

The [shader guide](https://www.dosbox-staging.org/0.83/manual/graphics/rendering/shaders/)
requires OpenGL output. Adaptive CRT shader selection is a visual effect choice;
it is unrelated to this port's Adaptive presentation cadence. A separate TinyGL
experiment implements a partial fixed-function software path, not GLSL support.
See the [adapter evidence](../amiga/records/2026-10-13/dosbox-adapters/method.md)
and [OpenGL integration](../amiga/dosbox/opengl/README.md).
