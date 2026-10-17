# Experimental x87 support

ZZDosBox can build the pinned DOSBox/libretro portable x87 implementation
with `--pc-x87`. This is guest x86 floating-point emulation on the ZZ9000's
Cortex-A9, not use of the Amiga CPU's FPU. No TF4060 or firmware change is
introduced by this feature.

The interpreter and ARM dynamic recompiler share the upstream x87 register
state and instruction helpers. Each guest session initializes the FPU, and
the BIOS equipment word advertises it. Arithmetic helpers use A9 VFPv3
binary64 operations with the existing softfp calling convention. SIMD/FPSCR
preservation remains mandatory around the entire Core1 session. No fast-math
or fused arithmetic is enabled.

## Build and test

Native interpreter with AddressSanitizer:

```sh
python3 scripts/build_dosbox_pc.py --compat --x87 --sanitize --game x87
```

ARM candidate, kept separate from the installed runtime:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
  --pc-pic-vfp --pc-compat --pc-x87
```

The candidate is built in the directory ending `-compat-x87` and retains
`ZZDosBox-Compat` as its executable name. The normal build without `--pc-x87`
remains available for comparison and rollback.

`X87TEST.COM` checks BIOS presence, initialization/control word, arithmetic,
square root, all four integer rounding modes, real-mode FPU
save/restore, extended-real memory conversion, and transcendental
operations. It prints `x87 arithmetic rounding state PASS` only after all
checks pass. Run it on the target dynamic core as well as the native
interpreter before promoting a candidate. The linked ARM instruction-model check also exercises two base
addresses and inherited FPSCR policies:

```sh
python3 scripts/test_dosbox_x87.py PATH_TO_BUILD
```

## Precision and scope

The portable upstream FPU stores values as binary64 doubles. It is not a
bit-exact 80-bit 80387 emulator: extended precision, exception details and
some rounding behaviour differ. Upstream environment save/restore also
selects its layout from the code-segment size; operand-size override fidelity
is not established by these tests. Supporting x87 instructions does not imply
64-bit guest CPU support, MMX/SSE, or a general compatibility guarantee.

The current experiment adds x87 to the EMS/Sound Blaster compatibility
runtime. Separate Windows, network, developer and OpenGL runtimes are not
changed. Quake's existing profile selects the compatibility runtime.

## Results

Native AddressSanitizer: x87 fixture passed; existing PC, EMS/XMS, SB Pro 2,
SB16, PC speaker and date fixtures passed. Quake 1.06 with 8 MB guest RAM and
`quake.exe -nosound -nocdaudio` rendered the start map (with `+map start`). These are native
interpreter results, not Amiga frame-rate measurements.

The linked ARM instruction model passed at two relocated base addresses with
non-default inherited FPSCR controls. All 32 SIMD registers and FPSCR were
restored; the existing PIT math/policy and denied-SIMD checks also passed.

Physical XX19c / A4000TX: the x87 fixture passed under ARM dynrec (32 JIT
commits, 31 dispatches). SIMD context comparison was equal; register mismatches
and cache-witness failures were zero, and owned memory was released.

The physical EMS/XMS and SB16 DMA5/IRQ7 regressions also passed. SB16 delivered
3,072 frames through AHI with error 0. The installed compatibility executable
is now `0.20.1-dev-compat-x87`; the launcher remains `0.20.1-dev`. Existing
EMS/Sound Blaster profiles, including Quake, select this executable without
another checkbox. The previous executable is retained as `.pre-x87`.

## Quake: first physical timing sample

Quake 1.06 shareware ran a short, deliberately truncated `demo1` sample:

| Setting/result | Value |
| --- | --- |
| Guest/output | 320×200 VGA, 640×480 Workbench window |
| CPU / memory | 6,000 cycles, 8 MiB guest RAM, EMS enabled |
| Audio | Quake `-nosound -nocdaudio`; host Sound Blaster off |
| Sample | First 32 demo packets; Quake counted 27 rendered frames |
| Real elapsed time | 21.07–23.45 seconds (polling bounds) |
| Real throughput | **About 1.2 FPS** (1.15–1.28 FPS bounded estimate) |
| Quake's internal result | 7.8 emulated seconds, 3.5 guest-clock FPS |

A `wait` followed by an `echo ZZBENCH_BEGIN` in the benchmark script marks
the second host frame, after initial map loading. The Mac records monotonic
time bounds while polling Quake's `-condebug` log once a second, then detects
the timedemo result. The interval includes log/poll overhead. This is a short
sample, not a complete standard timedemo or a general gameplay average.

[Quake's timedemo implementation](https://github.com/id-Software/Quake/blob/master/WinQuake/cl_demo.c)
counts frames and divides by its own `realtime` clock. That guest clock is
slower than wall time in this run; its 3.5 FPS must not be presented as physical
throughput. The emulator's VGA refresh count is also not unique Quake frames.

The initial physical Quake run recorded 647,996 paging fallbacks. This guest
uses paging, so the existing safety guard switches from JIT to the interpreter.
The x87 arithmetic helpers still use A9 VFP, but enabling x87 does not remove
that CPU-emulation bottleneck. Paging-safe JIT work would be a separate change;
this experiment does not disable the guard.

Quake returned through F10 with SIMD context equal, zero register/cache-witness
errors and owned memory released. Temporary benchmark files were removed.

Runtime SHA256:
`104e7f861e9c0f43ad0f3660ffd034052dfc1d99786d98cf9cca16ae7a4f94ab`.
