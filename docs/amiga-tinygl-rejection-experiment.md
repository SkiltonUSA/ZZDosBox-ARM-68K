# TinyGL hidden-pixel rejection experiment

This separate diagnostic compares exact integer span algorithms on pinned
jserv TinyGL `c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a`. It follows the
[raster experiment](amiga-tinygl-raster-experiment.md) and keeps the O2 compiler
settings selected by the [compiler experiment](amiga-tinygl-compiler-experiment.md).
It does not change the installed DOSBox runtime or establish DOS game FPS gains.

## Result: useful for occlusion-heavy scenes, not a universal replacement

Dedicated O2 builds on the physical ZZ9000 Cortex-A9 improved mostly occluded
scene throughput **47.30%** (40.597 -> 27.560 ms), mixed visibility **12.79%**
(63.625 -> 56.409 ms), and visible fill **2.89%** (77.599 -> 75.420 ms).
Small-triangle and clipping throughput regressed **2.80% / 2.88%**. Keep the
candidate diagnostic-only. These are native offscreen TinyGL scene measurements,
not DOSBox game FPS or display-frame rates. Current DOSBox GL presentation uses
glDrawPixels; this experiment does not accelerate emulated DOS game geometry.

Median scene times from three runs per dedicated build; lower is better:

| Scene | Original scalar (ms) | Hybrid (ms) | Throughput change |
|---|---:|---:|---:|
| geometry | 6.341 | 6.524 | -2.80% |
| clipping | 4.253 | 4.379 | -2.88% |
| fill | 77.599 | 75.420 | +2.89% |
| occluded-fill | 40.597 | 27.560 | +47.30% |
| clear-zero | 1.228 | 1.226 | +0.18% |
| clear-arbitrary | 1.084 | 1.083 | +0.11% |
| clear-colour-only | 0.725 | 0.725 | +0.01% |
| mixed-fill | 63.625 | 56.409 | +12.79% |

Clear controls use identical code and changed by at most 0.18%; no clear speedup
is claimed. A 47.30% throughput gain corresponds to 32.11% less scene time,
not a 47.30% time reduction. Per-run values and exact hashes are in
`amiga/records/2026-10-14/tinygl-rejection/fixed-results.json`.

The larger gain from skipping rejected prefixes survives removal of runtime
method selection. Visible-fill gains are much smaller than the initial
multi-method result. This reinforces the need to compare dedicated builds;
compiler register allocation and code layout matter on the A9.

Before integration, test a representative textured 3D scene and work on the
small-triangle overhead. Another candidate worth measuring is scalar prefix
rejection followed by the original unguarded NEON packer: the current hybrid
retains guarded packing, which may be unnecessary for mostly visible remaining
spans. Neither a benefit there nor a textured/game speedup has been demonstrated.

## Candidates

Four methods run in the same executable with scalar transforms and the original
clear routine. This removes the previous clear optimisation from the comparison.

1. Original TinyGL scalar spans.
2. Revision 2 NEON spans: four exact fixed-point pixels at a time, with rejected,
   fully visible and partially visible paths.
3. Guarded NEON: empty compiler-assembly read/write operands keep colour packing
   below the depth rejection branch. These are compiler dependencies, not CPU
   barriers or cache maintenance. Colour interpolation still advances each group.
4. Scalar-prefix/NEON hybrid: check groups of four depths using scalar arithmetic
   before constructing the colour vectors. Skip the wholly rejected prefix;
   adjust colour interpolation once, then use guarded NEON for the remaining
   span. A visible group ends prefix scanning; this is not a scan of every
   interior hidden region.

All methods preserve unsigned 32-bit wrapping, the full shifted depth comparison
before narrowing to 16-bit storage, equality semantics, depth-write selection,
short-span and scalar-tail handling, and stipple fallback. Only smooth untextured
non-blended triangles are candidates. Texture, blending and flat shading are
unchanged. No framebuffer transfers or scanout are measured.

## Workloads and correctness

Eight cases: small triangles, clipping, visible fill, mostly occluded fill,
three clear controls, and mixed visibility. Each fill scene draws 32 triangles
into a private 640x480 colour/depth buffer. The mixed case offsets triangle
positions and alternates near/far depths. It is not merely a different draw order
for the same coverage.

Host-only instrumented revision 2 spans counted these four-pixel groups:

| Scene | Fully rejected | Fully visible | Partly visible | Colour/depth hash |
|---|---:|---:|---:|---|
| Visible fill | 0 | 698,498 | 0 | `88077dd1` |
| Mostly occluded | 677,230 | 22,581 | 98 | `ccddff61` |
| Mixed visibility | 318,344 | 400,877 | 1,302 | `6e9cefb9` |

Instrumentation exists only in a separate generated host-test header. It is not
compiled into the timed ARM executable. Counts cover vector groups, excluding
short scalar spans and tails; they are not total scene pixel counts.

Native ASan/UBSan checks passed 123,648 span cases over three implementations,
four depth modes, all tail lengths and offsets, randomized wraparound, rejection,
equality and alternating visible/hidden groups. Clear tests cover 536 tail and
alignment cases and 560 pitched-buffer comparisons. Twenty complete scene/depth
combinations match the original colour and depth buffers exactly in all four
methods. The ARM binary also runs 7,872 bounded span self-checks before timing.

Report format `RST2` has eight 48-byte rows and a 16-byte header (400 bytes),
within the existing 416-byte diagnostic reservation. Old `RST1` reports remain
supported by the analyser; combining report revisions is rejected. Each method
is warmed and measured in order 0,1,2,3,3,2,1,0. Hashing is outside timed batches.
Physical runs use 16 scene draws or 128 clear calls per method. Instruction-model
runs use reduced repetitions and cannot establish performance.

## Initial physical comparison

Three physical runs passed exact output, EGA/PIT/shell, SIMD/MMU/register/cache
restoration and owned-memory release checks. Median milliseconds:

| Scene | Scalar | Revision 2 NEON | Guarded NEON | Hybrid |
|---|---:|---:|---:|---:|
| geometry | 6.489 | 6.490 | 6.490 | 6.494 |
| clipping | 4.399 | 4.423 | 4.402 | 4.401 |
| fill | 78.734 | 78.279 | 71.910 | 72.547 |
| occluded-fill | 41.108 | 65.871 | 45.103 | 27.627 |
| clear-zero | 1.229 | 1.229 | 1.229 | 1.228 |
| clear-arbitrary | 1.083 | 1.083 | 1.084 | 1.083 |
| clear-colour-only | 0.725 | 0.725 | 0.725 | 0.725 |
| mixed-fill | 64.525 | 75.900 | 62.492 | 54.972 |

The hybrid improves mostly occluded throughput by 48.80% and mixed-scene
throughput by 17.38% in this multi-method executable. Its visible-fill gain
is 8.53%. These are preliminary to the dedicated-build comparison below;
the old NEON path also acquired different register allocation and stack traffic,
so its regression cannot be attributed to one isolated cause.

## Dedicated-build validation

Compile-time scalar and hybrid builds remove runtime algorithm selection from
the triangle routines. They differ in exactly one ARM object: ztriangle.c.
All other ARM objects are byte-identical. Both retain O2, the original clears
and the same benchmark. Each report column now repeats the same fixed method.
Average all four equal-length columns per physical run, then use the median
of three runs per build. Do not interpret columns as different methods here.

Both builds passed native sanitizers and produced the same 20 complete
scene/depth hashes. All four linked ARM lifecycle checks passed for both builds. Three physical
runs per dedicated build passed; the final table above uses those six runs.

| Dedicated build | Executable bytes | CRC32 |
|---|---:|---|
| Original scalar | 1,169,896 | `6E7A07D5` |
| Hybrid | 1,172,952 | `16B0FC11` |

The scalar and hybrid binaries and their exact source/object hashes are retained
in `amiga/records/2026-10-14/tinygl-rejection/fixed-scalar/` and `fixed-hybrid/`.
Native `SCENE` records match for all 20 combinations across these builds.

## Generated ARM code

In `ZB_fillTriangleSmoothNOBLEND_DT1_DW1`, disassembly shows the guarded loop
branching around colour shifts/masks/packing after its depth-mask test. The
scalar prefix contains halfword depth loads, shifted comparisons and additions;
colour-vector setup follows the prefix. The empty assembly statement itself
emits no instruction, but it can cause register moves and alter scheduling.

The retained revision 2 path also has compiler-generated stack traffic in this
larger multi-method function. Consequently a timing difference between methods
cannot be attributed solely to the removed colour operations. Before promotion,
the chosen algorithm needs its own standalone build comparison against the
original; do not treat this diagnostic's code layout as production performance.

## Validation and final hardware state

Native correctness and workload-coverage checks passed. The ARM/68k build and
four linked ARM-model lifecycle checks passed: normal completion, cancellation
during framebuffer copying, denied SIMD access, and cancellation during scene
rendering. SIMD registers/FPSCR and owned-buffer cleanup passed.

After the user exited the game and Core1 became available, the diagnostic was
staged to RAM only. The initial three physical measurements passed. Comparison executable:
1,175,064 bytes, CRC32 `E03D33A0`, SHA256
`a0abdc64b321ebd10399411f3ddc464290d75e7531277e4deb97f3281459bca2`.
Evidence: `amiga/records/2026-10-14/tinygl-rejection/v1/`.

All nine physical runs (three comparison, three scalar, three hybrid) passed
7,872 bounded kernel checks per run, exact scene colour/depth comparisons,
EGA/PIT/shell checks, SIMD/MMU/register/cache-witness restoration and owned-memory
release. All twelve linked-model lifecycle checks passed across the three builds.
Final bridge and Workbench checks were responsive, with no Core1 owner or
Software Failure. The staged RAM diagnostic is now the fixed hybrid, CRC32
`16B0FC11`. Installed DOSBox remains CRC32 `5C74FFFC`; no installed launcher,
profile, firmware or startup changes were made. No GitHub push or release was
made for this experiment.

The hybrid remains experimental despite the measured gains. Changes in code layout
and diagnostic branches can affect even the scalar baseline; comparisons must
use the methods inside this executable, with the earlier build retained as
historical context.

## Reproduce

```sh
python3 scripts/test_dosbox_tinygl_raster.py
python3 scripts/test_dosbox_tinygl_rejection_coverage.py
python3 scripts/build_dosbox_dos_arm.py \
  --container-command '["podman","--connection","nuflix-converter-root"]' \
  --pc --pc-dynrec --pc-jit-mib 4 --pc-neon --pc-pipeline \
  --pc-scanlines --pc-presentation --pc-adapters --pc-pic-vfp \
  --pc-matrix-bench --pc-tinygl-bench --pc-tinygl-raster \
  --pc-raster-compiler baseline
```

Run `tests/amiga/dosbox/check_tinygl_arm.py --build BUILD` with the Unicorn
Python environment before hardware staging. `scripts/measure_dosbox_tinygl_raster.py`
provides separate RAM-only `stage` and `run` actions, verifies checksums and
Core1 ownership, reads only closed logs, and checks restoration and memory release.
Never interrupt a different application's Core1 worker to run this diagnostic.

For dedicated builds, add `--pc-raster-fixed scalar` or `--pc-raster-fixed hybrid`.
The build directory is reused, so snapshot each binary, ELF and build metadata
before switching variants. Native tests accept the matching `--fixed-mode`.
Analyse each set of three closed logs with
`scripts/analyse_dosbox_tinygl_raster.py --fixed-build BUILD --output RESULTS LOG...`,
then compare with `scripts/analyse_dosbox_tinygl_rejection.py`. The dedicated
analyser checks the fixed selector, binary CRC, matching scene hashes, three
runs per build and that only the ztriangle ARM object differs.

## Follow-up

The [packing and triangle-dispatch experiment](amiga-tinygl-packing-experiment.md)
adds dedicated unguarded/dispatch builds and textured workloads. Its fresh
baselines are separate from the timings above.
