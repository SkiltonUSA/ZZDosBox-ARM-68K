# ZZDosBox ARMv7 dynamic recompiler experiment

13 October 2026. Initially tested as a separate experimental executable.
Subsequently connected to the normal launcher at the owner’s request; see
the installation note below.

**Physical Doom Demo3: approximately 6.94 FPS, up from 3.10 FPS — 2.24×.**

| Core | Real timedemo duration | Real game FPS |
| --- | ---: | ---: |
| Normal interpreter | 689.143 s | 3.10 |
| ARMv7 dynamic recompiler | 307.311 s | 6.94 |

Same 2,134-tick demo and settings. This is one measured run per build, with
approximately one-frame endpoint uncertainty; it is not a general PC rating.
The gain comes from the ARM CPU execution path, not an SDL or firmware change.

## Source and execution model

Retains the pinned `libretro/dosbox-libretro` core at
`a563bb5e9753f207d74e9d8c417d793b784fa4be`. Reviewing
[DOSBox Pure](https://codeberg.org/schelling/dosbox-pure) at
`9911d3b60838cabf4cfe727ccdfc669770c7186d` confirmed its ARMv7 emitter and
switcher match this upstream; this is an integration of the existing dynrec,
not a change of emulator base or a newly written x86 translator.

Guest x86 blocks compile to ARMv7 code on application-owned Core1. The bounded
1 MiB code cache is inside the existing 32 MiB Exec allocation. Its enclosing
arena uses offsets 0x1e00000–0x1f20000, rounded inward to physical 4 KiB pages
at the verified runtime mapping. Normal data pages remain execute-never.
Metadata limits are 8192 blocks / 128 guest code pages; heap increases from
2 to 4 MiB within that same allocation. The linker reserves the JIT arena.

Replaces the upstream Linux cache-flush syscall with local A9 D-cache clean
to PoU, barriers, I-cache and branch-predictor invalidation. Private memory is
inner write-back, outer non-cacheable; shared L2 and Core0 are untouched.
Both generated blocks and the entry/link trampoline are published. A launch
witness rewrites and executes one ARM function three times before guest code.
Cleanup restores guest page handlers and releases dynrec metadata before PC
devices disappear. The existing XACP ownership and restoration contract remains.

## Build and verification

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-doom-clock \
  --container-command '["podman","--connection","nuflix-converter-root"]'
.context/amiga/mesen-bench/venv/bin/python tests/amiga/dosbox/check_dynrec_arm.py
clang -fsanitize=address,undefined -Iamiga/dosbox/pc \
  tests/amiga/dosbox/dynrec_plan.c -o .context/amiga/dosbox-dynrec/test-arena
.context/amiga/dosbox-dynrec/test-arena
```

Output: `.context/amiga/dosbox-dynrec/ZZDosBox-Dynrec`, 1,071,752 bytes,
SHA256 `e3d0718d9bb41e9b0ee51053c32e9a8b9c4ac7717625f86099325590d827f0aa`.
ARM image 793,436 bytes / 2198 relocations. Build metadata and exact staging
patch are retained beside this report.

The linked ARM model passed CPU/flags/memory, DOS COM/MZ execution and files,
self-modifying guest code, protected-mode demand paging, EGA pixels/PIT,
cancellation during framebuffer copying and host I/O, BIOS boot/A20,
HDD sector reads/writes/bounds, mouse/PS2 input and a second relocation base.
Both interpreter and dynrec MMU policies passed 16,384 allocation alignments;
arena bounds passed ASan/UBSan. The native interpreter EGA regression also
passed with sanitizers.

`model-regression.log` retains an initial harness failure: cancellation at
the DOS prompt legitimately had zero guest dispatches. The assertion now
allows that case; `model-remaining.log` verifies both read-stop and fs-stop.
This was not a worker crash. Model timing is not physical speed evidence.

## Physical A4000TX checks

Test binary transferred to `RAM:ZZJit1013/ZZDosBox-Dynrec`, MD5 verified.
The self-modifying COM fixture passed: cache witness 1, 20 commits, 1061
dynrec dispatches. EGA/PIT passed: witness 1, 30 commits, 283423 dispatches,
17 fullscreen frames/flips. Both recorded guest exit 0, zero checked register
mismatches/cache-witness errors, launcher exit 0 and owned-memory release.

## Doom comparison method

Use the supplied Doom 1.9 shareware files already installed in the private
game drawer. Verify no DEFAULT.CFG exists; run `DOOM.EXE -timedemo demo3`,
8 MiB guest RAM, VGA, 10000 cycles/ms, 640×480 fullscreen, no FPS overlay,
fresh game defaults, no PCM audio and no input/screenshots during playback.
These match the earlier interpreter benchmark.

The read-only observer validates loaded executable bytes/string and samples
Doom gametic 0/1 through 2134 against the A9 global timer, nominally
333333333 Hz. Approximately one-frame endpoint uncertainty remains. Calculate
real FPS from that elapsed time, **not** Doom's emulated realtics or host
display-refresh count. Preserve both numbers in the result record.

Baseline: about 689.143 real seconds / 3.09660 game FPS; see
`../dosbox-doom/result.json`. Dynrec: 307.311 seconds / 6.94410 game FPS,
about 124% higher throughput. Total worker lifecycle including load/exit was
322.340 seconds. `physical-doom.log`, `result.json` and `analyse.py` retain
the evidence and calculation. Doom's 6401 emulated realtics imply 11.67
guest-clock FPS, which is **not** the measured hardware score.

The run validated the Doom signature and endpoints (0 to 2134), with no
observer rejection. It reported 12,365 JIT commits, 24,818,803 dispatches,
instruction-cache witness 1 and peak heap usage 2,334,080 bytes. All 13,012
display flips completed. Guest exit 0 / SHELL PASS, zero checked register
mismatches/cache-witness errors, launcher exit 0 and owned-memory release.
Workbench, Roadie and the bridge remained available; Core1 ownership ended.

Game/OS media and guest RAM dumps are excluded. This experiment does not yet
establish Windows 3.11 compatibility, PCM audio, all-game stability, or a
general equivalent Intel CPU rating.

## Launcher installation

Installed the exact benchmark executable at the launcher's existing sibling
`PC/ZZDosBox` path, after verifying the staged copy. The old interpreter is
preserved as `PC/ZZDosBox-Interpreter`. Both remote MD5 checks and the unchanged
launcher configuration checksum are in `launcher-install.log`. No firmware or
launcher source changes were needed. A user-owned DOS session was active and
was left running; dynrec takes effect on its next launch. No additional hardware
run was started during that session. Empire and Windows dynrec compatibility
remain pending.

The owner subsequently requested stopping the active game. F10 closed it;
Core1 ownership ended and the launcher returned on Workbench, ready for the
next launch to use dynrec.
