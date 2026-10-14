# A9 floating-point scheduler optimisation

The ZZDosBox Direct runtime now uses the Cortex-A9 hardware floating-point
unit for DOSBox PIC event scheduling. Doom Demo3 improved **4.68%** in a
matched, repeated hardware comparison. The executable is the exact build
used for those measurements.

| Build | Run 1 FPS | Run 2 FPS | Mean FPS |
| --- | ---: | ---: | ---: |
| Software floating-point baseline | 7.2598 | 7.3291 | 7.2944 |
| Hardware floating-point PIC | 7.6373 | 7.6344 | 7.6358 |

Run order: baseline, candidate, candidate, baseline. Pairwise gains were
5.20% and 4.17%. Tests used an A4000TX/ZZ9000, XX19c/XACP, 8 MiB guest RAM,
10000 cycles, Direct fullscreen, Normal presentation, no scanlines, overlay,
profiling or trace. No frames were skipped. Game FPS is measured over Doom's
2134-gametic Demo3 interval using the A9 global timer, not the emulated DOS
clock or display-refresh counter. These are two runs per build on one system.

The final baseline reported 6401 emulated realtics rather than 6400 and three
additional display frames. The raw evidence retains that variation; all four
runs should not be described as identical instruction traces.

## Implementation

Only `pic.cpp` switches from software floating point to scalar VFPv3, using
`-mfloat-abi=softfp -mfpu=vfpv3 -ffp-contract=off`. The calling convention,
queue algorithm, event order and guest cycle budget are preserved. No fast-math
is enabled. The existing SIMD guard checks firmware access, saves D0-D31 and
FPSCR, establishes round-to-nearest with gradual underflow, and restores the
original state at exit. Firmware, shared L2 policy, XACP buffer ownership and
the 68k display/audio paths are unchanged.

This helps arithmetic previously performed in software. It does not make
all floating-point routines faster, and it does not change 2D pixel drawing.
The standard Direct build recipe enables `--pc-pic-vfp`; the lower-level
builder keeps the flag explicit for control builds and other renderer variants.
OpenGL, the Developer launcher and the separate Windows runtime are not
promoted by this change.

## Verification and limits

Four paired ARM instruction-model cases produced matching scheduler traces,
guest output/time and pixels at 500/3000/10000 cycles and a live 6000-to-2500
change. Six additional cases covered self-modifying code, paging, BIOS/A20,
framebuffer/filesystem cancellation and denied SIMD access. These checks do
not establish physical speed.

Physical EGA/PIT, all four Doom runs and timed Vette/Empire startup checks
restored SIMD/MMU/cache witnesses and released owned memory. The installed
executable also passed EGA/PIT. The previous Direct executable is retained
for rollback.

| Eight-second emulated startup | Baseline wall time | VFP wall time |
| --- | ---: | ---: |
| Vette with LOADFIX | 16.718 s | 16.095 s |
| Empire | 20.356 s | 20.382 s |

Vette startup took 3.7% less time in one pair; Empire startup was effectively
unchanged. These are startup/cancellation checks, not gameplay FPS. Empire's
city-attack slowdown, extended play and Windows compatibility still need
separate validation. The result does not establish a universal PC-speed rating.

[Measured executable and build inputs](../artifacts/development/pic-vfp) ·
[Raw results and analysis](../amiga/records/2026-10-14/dosbox-pic-vfp)

## Reproduce

Restore the saved inputs using `python3 scripts/restore_build_inputs.py --pic-vfp-build`.
Use `make build-current` to build the Direct runtime; neither command deploys
or launches hardware. With a Unicorn-enabled Python, run:

```sh
python3 tests/amiga/dosbox/check_pic_vfp_arm.py \
  --baseline .context/amiga/dosbox-pic-vfp-baseline \
  --candidate .context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp \
  --record .context/pic-vfp-model.json
python3 amiga/records/2026-10-14/dosbox-pic-vfp/analyse.py
```

For future XACP/firmware work, preserve the published application-owned Core1,
framebuffer handoff and processor-state restoration interfaces. The measured
gain here is application code optimisation; it requires no firmware update.
