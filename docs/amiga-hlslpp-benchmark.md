# hlsl++ on the ZZ9000 Cortex-A9

## Decision

**Keep the existing implementations.** hlsl++ works in the isolated A9 diagnostic,
but it does not offer a useful complete-scene improvement in these workloads.
It remains a benchmark dependency; no production renderer, launcher setting or
installed executable was changed.

The three-run median geometry improvement over scalar TinyGL was **0.19%**.
Clipping and fill were effectively unchanged. Against the experimental internal
NEON path, geometry improved **0.71%**, with no material clipping/fill gain.
These small changes do not justify adding the library and its ARM32 compatibility
patch to the renderer. They are not measurements of DOS-game or Windows FPS.

## Standalone maths

Physical A4000TX / ZZ9000 Core1, Cortex-A9 reported at 666 MHz, XX19c.
Times below are medians of three physical runs; lower is better.

| Workload | Scalar VFP (µs) | Specialised NEON (µs) | hlsl++ NEON (µs) |
| --- | ---: | ---: | ---: |
| 4x4 matrix product | 0.623 | 0.230 | 0.605 |
| 16 vertex transforms | 2.363 | 1.503 | 2.184 |
| 256 vertex transforms | 36.955 | 22.835 | 33.872 |
| 1024 vertex transforms | 148.331 | 92.817 | 136.284 |

hlsl++ gives roughly 3–9% more throughput than the scalar kernels here, but the
existing specialised NEON matrix product is **2.63× faster than hlsl++**, and
the 1,024-vertex batch is **1.47× faster**. The matrix product includes public
hlsl++ `load`, `mul` and `store`; the vertex loop uses `mul(matrix, vertex)`.
The specialised vector kernel processes four vertices together.

Disassembly shows additional byte-table shuffles, register moves and stack work
in the hlsl++ path. That is a plausible explanation for part of the difference,
not a measured attribution of every cycle. This result concerns this pinned
library, compiler and ARMv7 workload; it is not a general claim about SIMD libraries.

## Complete TinyGL scenes

These are private 640×480 offscreen, smooth-shaded, depth-tested native scenes.
They exclude display scanout and Zorro transfers. The internal replacement uses
hlsl++ for TinyGL's combined matrix multiplication and unlit vertex transforms;
lighting retains the original implementation. Input W=1 and TinyGL's constant-W
shortcut are preserved. Original cached matrix columns are reused.

Each row below is an A/B comparison within one binary. Do not combine absolute
times across the two builds as though they were one three-way trial: instruction
layout can change timing. All unprofiled comparisons use warmed ABBA ordering,
16 frames per method per scene per run, and three physical runs.

| Scene | Scalar baseline (ms) | hlsl++ (ms) | Throughput change |
| --- | ---: | ---: | ---: |
| Geometry (1024 triangles) | 6.091 | 6.080 | +0.19% |
| Clipping (128 triangles) | 4.142 | 4.143 | -0.04% |
| Fill (32 triangles) | 77.515 | 77.495 | +0.03% |

| Scene | Existing NEON candidate (ms) | hlsl++ (ms) | Throughput change |
| --- | ---: | ---: | ---: |
| Geometry | 6.148 | 6.105 | +0.71% |
| Clipping | 4.150 | 4.149 | +0.02% |
| Fill | 77.500 | 77.515 | -0.02% |

## Stage measurements

Separate instrumented hlsl++ frames from the scalar comparison, median milliseconds:

| Scene | Transform/viewport | Clip/dispatch | Triangle drawing | Clear | Other | Profiling slowdown |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Geometry | 2.359 | 0.472 | 2.396 | 1.151 | 2.565 | 1.472× |
| Clipping | 0.318 | 0.266 | 2.480 | 1.152 | 0.372 | 1.107× |
| Fill | 0.141 | 0.046 | 76.183 | 1.156 | 0.229 | 1.003× |

The fill scene spends about 98% of its instrumented frame time drawing triangles.
Matrix changes cannot remove that work. Timer hooks slow the geometry scene by
about 47%, so these stage observations are diagnostic, not unbiased production
stage costs. Full-frame gains above come only from profiling-disabled runs.
Raw stage results for both methods, ranges and every repeat are in the JSON files.

## Compatibility and correctness

- Pinned [hlsl++](https://github.com/redorav/hlslpp/tree/3d9af2e77a89ddd645928f1600a0688dc898dedc),
  revision `3d9af2e77a89ddd645928f1600a0688dc898dedc`, MIT licence retained.
- Pinned jserv/TinyGL `c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a`.
- ARM GCC 15.3.1; Cortex-A9 ARM state, softfp ABI, NEON, `-O2`, contraction and
  auto-vectorisation disabled for the compared kernels. Scalar uses VFPv3.
- The upstream NEON header did not compile unchanged with this ARM32 GCC.
  The recorded patch supplies explicit bit reinterpretations and correct
  intrinsic argument types for shuffle, mask, load and packing helpers.
  No lax-vector-conversion flag, ARM64 instruction requirement or global cache
  configuration change was introduced. Only the matrix/vertex paths are fully
  exercised here; this is not certification of every hlsl++ operation.
- ARM64 native ASan/UBSan: 1,000 random matrix cases, 22 vector sizes including
  zero/tails, guards, immutable inputs and XYZ read bounds. Three scenes match
  colour/depth byte-for-byte across scalar, NEON and hlsl++, with profiling on/off.
  Another 1,000 internal cases cover matrix invalidation, method switching,
  lighting fallback, and constant/perspective W.
- Linked A9 model: 366 direct matrix/vector calls against a double-precision
  oracle, maximum scaled error `1.58e-7` (limit `2e-6`), strict memory bounds.
  All three linked builds passed EGA/PIT lifecycle, framebuffer cancellation,
  scene cancellation, unavailable SIMD, and restoration of D0–D31/FPSCR.
- Nine physical runs: zero reported errors, exact colour/depth hashes, EGA/PIT
  and shell pass, zero MMU/register mismatches and cache witness errors,
  normal launcher exit and owned-memory release. The shared report remains
  within the existing bounded diagnostic area.
- Installed DOS runtime, Windows interpreter, launcher, library and dosbox.conf
  checksums are unchanged. Bridge responsive; no Core1 owner or Software Failure
  at completion. Executables were staged only in RAM. No release or GitHub push.

Finite ordinary float inputs are covered. Exceptional/subnormal arithmetic and
arbitrary scenes are outside this validation. The private offscreen buffers use
the established owned-allocation and cache contract; no SDK DDR address is assumed.

## Reproduce

```sh
python3 scripts/test_dosbox_hlslpp.py
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
  --pc-pic-vfp --pc-matrix-bench --pc-hlslpp micro
```

For each internal comparison, add `--pc-tinygl-bench` and replace `micro` with
`scalar` or `neon`. Supply the existing `--container-command` override when needed.
Native sanitizer tests require an ARM64 host; the linked-model checks use the
existing Unicorn environment. Use `check_matrix_arm.py --build DIR --lifecycle`
for micro and `check_tinygl_arm.py --build DIR` for internal builds under
`tests/amiga/dosbox/` before physical execution.

Use the existing `measure_dosbox_matrix.py` or `measure_dosbox_tinygl.py` stage/run
commands in the MCP environment, with fresh evidence labels and an idle Core1.
The runners verify the staged CRC and completed logs, check cleanup, and confirm
the installed runtime is unchanged. Analyse using `--candidate hlsl`; internal
analysis also takes `--baseline scalar` or `--baseline neon`.

[Evidence, exact executables, ELF files, licences, patches, disassembly, validation,
raw results and source snapshots](../amiga/records/2026-10-14/hlslpp/).
