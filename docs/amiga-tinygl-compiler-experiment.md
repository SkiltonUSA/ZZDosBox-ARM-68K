# TinyGL Cortex-A9 compiler experiment

This follows the [NEON raster experiment](amiga-tinygl-raster-experiment.md)
and the [ARM compiler-options reference](https://gist.github.com/alphanu1/86dfd5eb7343192712c084dcbc9029ba).
It tests compiler changes on the physical ZZ9000 without changing installed
DOSBox, firmware, startup files or launcher settings.

## Result: retain O2; no broad compiler-flag speedup

Nine fresh physical runs completed: three per profile, on the A4000TX's ZZ9000
Cortex-A9 at 666 MHz. All output hashes and cleanup checks passed. Neither O3
profile is suitable as a broad replacement for the existing O2 settings.

The original visible-fill method regressed from **78.355 ms to 90.904 ms** with
O3 (**13.80% lower throughput**) and **91.005 ms** with automatic vectorisation
(**13.90% lower throughput**). Mostly occluded fill also regressed by about 2.2%.
O3's smaller gains on geometry and clipping do not justify those losses.

With both handwritten NEON spans and zero clearing selected, visible fill was
**63.699 / 63.808 / 63.749 ms** for baseline/O3/O3-vector respectively: no gain.
The original raster experiment's benefit therefore comes from its targeted
span implementation, not simply raising the compiler optimisation level.

The custom zero clear gained only **1.60%** with O3 (1.140 -> 1.122 ms, clear-only
switch). O3-vector made original zero clearing **1.55%** slower in throughput,
arbitrary colour/depth clearing **3.14%** slower and colour-only clearing **2.53%**
slower, despite successfully generating vector instructions. Do not replace
original nonzero clears on the strength of compiler vectorisation alone.

Original raster/clear method, median milliseconds; lower is better. Percentages
compare throughput with the same method in the O2 build:

| Case | O2 | O3 | O3 vector | O3 gain | O3 vector gain |
|---|---:|---:|---:|---:|---:|
| geometry | 6.561 | 6.539 | 6.590 | +0.34% | -0.43% |
| clipping | 4.406 | 4.372 | 4.406 | +0.77% | -0.01% |
| fill | 78.355 | 90.904 | 91.005 | -13.80% | -13.90% |
| occluded-fill | 41.097 | 42.033 | 42.014 | -2.23% | -2.18% |
| clear-zero | 1.227 | 1.227 | 1.247 | +0.01% | -1.55% |
| clear-arbitrary | 1.085 | 1.083 | 1.120 | +0.16% | -3.14% |
| clear-colour-only | 0.725 | 0.724 | 0.743 | +0.04% | -2.53% |

Fresh O2 original-path medians were within about 0.2% of the earlier revision-2
measurements. Full per-method/per-run values and ranges are retained in
`amiga/records/2026-10-14/tinygl-compiler/comparison.json` and each profile's
`results.json`. Small sub-percent changes should not be promoted as a useful
application gain. Code-size/layout and instruction scheduling effects were not
isolated; the experiment establishes the net result, not a single cause.

The next useful target remains rejected-span handling in the handwritten
rasteriser. The optional compiler profiles are retained for reproducible
experiments; default compiler settings and installed applications stay unchanged.

## Physical verification and evidence

Both candidate builds passed native sanitizer/exact-image checks and all four
linked ARM lifecycle tests before execution. All nine physical runs passed the
656-case self-check, seven-case colour/depth comparisons, EGA/PIT/shell checks,
SIMD/MMU/register/cache-witness restoration and owned-memory release. Final
bridge/Workbench checks remained responsive, with no Core1 owner or Software
Failure. The installed DOSBox checksum stayed `5C74FFFC` throughout.

Only `RAM:ZZRaster/ZZRasterBench` was replaced for each diagnostic. Its final
staged profile is O3-vector, CRC32 `CF00BF01`; it is not an installed emulator
upgrade. No firmware, startup, launcher or profile changes; no GitHub push or
release. Exact binaries, ELF files, source snapshots, generated-source excerpts,
compiler diagnostics/disassembly, test logs, physical logs, bridge checks and
SHA256 manifests are in `amiga/records/2026-10-14/tinygl-compiler/`.

## Controlled changes

Three compiler profiles use the same revision-2 raster sources and diagnostic:

| Profile | Target routine flags |
|---|---|
| Baseline | `-O2 -fno-tree-vectorize` |
| O3 | `-O3 -fno-tree-vectorize` |
| O3 vector | `-O3 -ftree-loop-vectorize -ftree-slp-vectorize -fvect-cost-model=dynamic` |

Only `ztriangle.c`, `zbuffer.c` and `amiga/dosbox/tinygl_raster/clear.c`
receive the experimental flags. Every other ARM object is byte-identical to
the baseline. All use `-mcpu=cortex-a9`, ARM mode, `softfp` with NEON on these
units, `-mno-unaligned-access`, `-fno-strict-aliasing` and `-ffp-contract=off`.
No fast-math setting is enabled. Transform routines, the timing harness, DOS
CPU emulation, scheduler, ABI and runtime memory/cache contract are unchanged.

The default build profile remains baseline. The new `--pc-raster-compiler`
option is restricted to the separate `--pc-tinygl-raster` diagnostic. Compiler
vectorisation reports and disassembly are retained rather than assuming that
enabling a flag vectorised a particular loop.

`softfp` permits hardware floating-point instructions while retaining the
base calling convention. Switching to hard-float would change the ABI. Also,
`-mcpu=cortex-a9` already selects Cortex-A9 tuning; adding the matching `-mtune`
is not a new optimisation. See [GCC's ARM options](https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html).

## Generated code

Compiler: `arm-none-eabi-g++ (15:15.3.rel1-2) 15.3.1 20260627`.
Both candidates changed exactly three ARM objects; all other ARM objects
matched the preserved O2 baseline byte for byte.

| Build | ARM image bytes | Amiga executable bytes | Executable CRC32 |
|---|---:|---:|---|
| Baseline | 882,664 | 1,171,224 | `8FAD32D1` |
| O3 | 902,712 | 1,191,272 | `CAEABDA6` |
| O3 vector | 907,016 | 1,195,576 | `CF00BF01` |

GCC reports 16-byte automatic vectorisation of the original colour/depth clear
loops. In triangle drawing, the reported vectorisation covers two flat,
no-depth-test main loops and leading alignment loops in two smooth,
no-depth-test variants. It does **not** report vectorising the main
smooth depth-tested loop exercised by the four benchmark scenes. The explicit
NEON span remains present and is enabled only by the runtime method switch.

O3 inlines the custom clear helpers: their standalone symbols disappear and
`zz_clear_buffers` grows from 224 bytes to 604 bytes (584 with automatic
vectorisation). The main smooth depth-tested/write-enabled raster function
grows from 3,156 to 3,312 bytes in both O3 builds. These are code-generation
observations, not independent proof of a performance improvement or regression.

## Measurement and correctness

The same seven cases and four runtime methods from the raster experiment run:
original raster/clear, handwritten NEON spans only, zero clear only, and both.
Compiler comparisons match the same method across binaries; they do not mix
an O2 original path with an O3 handwritten span path and attribute the whole
difference to compiler flags.

Physical timings use the A9 timer with profiling disabled, private 640×480
colour/depth buffers, warmed A/B/C/D/D/C/B/A batches, 16 frames per scene and
128 calls per clear case. Three fresh runs per profile are required. Images
and depth must match within each executable, across compiler profiles and,
for the first three scenes, the previous verified scalar hashes.

Native ASan/UBSan tests cover 10,304 span cases, 536 clear tail/alignment cases,
560 pitched clear cases and 16 complete scene/depth combinations. Native
Clang testing checks correctness only; its timings and code generation are
not evidence for GCC performance on the A9. The actual linked ARM builds also
run normal execution, copy cancellation, denied SIMD and mid-scene cancellation
tests before physical execution. Each hardware run includes 656 kernel
self-checks, DOS EGA/PIT/shell checks and CPU/context/memory cleanup checks.

Separate binaries can differ in placement and code size. Measured changes
therefore include code-layout effects, not solely individual instruction
changes. This experiment measures native TinyGL workloads, not DOS game FPS,
guest OpenGL acceleration or framebuffer transfer speed.

## Reproduction

Use the existing raster build command from the earlier report, adding
`--pc-raster-compiler o3` or `--pc-raster-compiler o3-vector`. Snapshot each
completed build before building another profile: they share the existing
diagnostic output directory so unchanged objects can be reused.

Run `scripts/test_dosbox_tinygl_raster.py --compiler-profile PROFILE` for native
checks and `tests/amiga/dosbox/check_tinygl_arm.py --build DIR` in the model
environment. Use `scripts/measure_dosbox_tinygl_raster.py` in the MCP environment
to stage and run the checked binary in RAM, with a unique label for each run.
Never stage a different profile while a previous run is active.

Analyse each set of physical logs with `scripts/analyse_dosbox_tinygl_raster.py`,
then compare the three result files using
`scripts/analyse_dosbox_tinygl_compiler.py --baseline BASE.json --o3 O3.json
--vector VECTOR.json --output comparison.json`. This verifies matching hashes
and reports same-method throughput gain and time reduction separately.

## C-Chads TinyGL reference review

Reviewed [C-Chads/tinygl](https://github.com/C-Chads/tinygl), archived November
2023, at `36a7987e7bebfda19615ea33341b1cc0ff9c3b13`, against our pinned
[jserv tree](https://github.com/jserv/tinygl/tree/c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a).
Source comparison shows extensive shared raster/math structure. C-Chads tests
depth settings inside pixel paths; our pinned version generates separate
functions for depth-test/write combinations. Switching repositories is not
an established optimisation.

No hand-written NEON intrinsics or ARM assembly were found in its `src` and
`include` trees. Its SIMD approach uses compiler vectorisation, alignment and
OpenMP SIMD directives. Its optional threaded pixel/copy operations would need
an execution runtime; they cannot simply recruit firmware-owned Core0 in our
application-owned Core1 model. Neither repository's desktop performance claims
prove an A9 result.

Recommendation: retain our pinned source and examine specific loop/alignment
ideas separately. Do not replace the renderer or enable OpenMP globally. This
was a source review; C-Chads was not built or benchmarked on the Amiga.
