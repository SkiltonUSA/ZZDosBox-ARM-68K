# TinyGL internal-transform experiment

## Physical A9 result — 14 October 2026

**The internal NEON replacement did not improve complete scene rendering.**
Three clean runs on the A4000TX/ZZ9000 Cortex-A9 (666 MHz, XX19c) gave the
following medians, with profiling disabled during speed measurements:

| Scene | Scalar VFP | Internal NEON | Throughput change |
|---|---:|---:|---:|
| Geometry (1024 triangles) | 6.030 ms | 6.065 ms | -0.580% |
| Clipping (128 triangles) | 4.181 ms | 4.182 ms | -0.003% |
| Fill (32 triangles) | 77.614 ms | 77.614 ms | -0.001% |

Geometry was consistently slightly slower: scalar range **6.025–6.043 ms**,
NEON **6.060–6.065 ms**. Clipping/fill ranges overlap; their tiny median
changes are effectively unchanged. Keep this as an experiment, not a production
optimisation. These are offscreen native scenes, not DOSBox game FPS or display
refresh measurements.

Separate **instrumented NEON stage times** (median milliseconds):

| Scene | Transform + viewport | Clip/dispatch | Triangle drawing | Clear | Other | Profiling slowdown |
|---|---:|---:|---:|---:|---:|---:|
| Geometry | 2.503 | 0.475 | 2.414 | 1.208 | 2.487 | 1.499× |
| Clipping | 0.326 | 0.263 | 2.492 | 1.209 | 0.352 | 1.108× |
| Fill | 0.125 | 0.044 | 76.205 | 1.222 | 0.215 | 1.003× |

Triangle drawing accounts for **97.94%** of the fill scene's instrumented
wall time. Its profiling overhead is only about 0.28%, making the raster
bottleneck clear. Clipping itself is comparatively small even in the deliberate
edge-crossing scene. Colour/depth clearing costs about **1.2 ms** per frame,
which is significant in the smaller workloads.

Profiled transform spans fall by about 3%, but this does **not** establish a
production transform speedup: instrumentation changes execution and inflates
the geometry frame by roughly 50%. The unprofiled whole-frame measurement is
the deciding result. The earlier 2.71× standalone matrix / 1.59× batched-vertex
result does not transfer to TinyGL's one-vertex-at-a-time path.

The next useful experiment is the smooth-shaded **pixel-span/depth-test loop**,
with exact colour/depth comparison and scalar handling of span edges. Investigate
clear throughput separately. Clipping and additional matrix tuning are lower
priorities for these scenes. No gain is promised for textured scenes, different
geometry, or DOSBox's existing pixel-presentation path.

## Verification and evidence

- Native ARM64 ASan/UBSan: three nonblank scenes, byte-identical colour/depth
  buffers across both methods with profiling on/off; 1,000 matrix/vertex cases;
  perspective and constant W; matrix invalidation; method changes without matrix
  reload; original lighting fallback; context/buffer cleanup.
- Linked A9 instruction model: normal EGA/PIT lifecycle, cancellation during
  framebuffer copy, cancellation inside the native scene, and unavailable SIMD.
  Exact scene hashes/stage accounting; all D0–D31/FPSCR restored. These model
  tests establish correctness, not physical speed.
- Three physical runs: `errors=0`, stable colour/depth hashes, EGA/PIT and shell
  pass, `guest_exit=0`, `context_equal=1`, zero register mismatches and cache
  witness errors, normal launcher exit and owned-memory release.
- Installed runtime unchanged (CRC32 `5C74FFFC`). Diagnostic exists only in RAM
  and local evidence. Final bridge/Workbench checks show no Core1 task or
  Software Failure. No GitHub publication or Release was made for this experiment.

Diagnostic: 1157896 bytes, CRC32 `16EDEFE9`, SHA-256
`1cde084f0ce26481fa232892b0d22574d07ca6d2425b944ebcb4329eeacc03b3`. ARM image: 868,772 bytes; 2,300 relocations.

Exact executable/ELF, raw logs, results, pinned TinyGL licence/provenance,
validation logs, ARM disassembly, source snapshots and manifests:
`amiga/records/2026-10-14/tinygl-internal/`.

This is a separate native Cortex-A9 software-rendering diagnostic. It replaces
TinyGL's own unlit vertex transform and combined 4×4 matrix multiplication,
rather than transforming input vertices before handing them to TinyGL.
It does not intercept a DOS game's geometry or accelerate x86 instructions.
The installed DOSBox OpenGL presentation path still uses `glDrawPixels`.

## Implementation

- Pinned upstream: jserv/TinyGL `c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a`.
- Original scalar VFP and candidate NEON execute in the same diagnostic binary.
- `gl_M4_Mul` uses the independently implemented four-row NEON product in the
  candidate. `gl_M4_MulLeft`, texture transforms and lighting retain upstream code.
- TinyGL's dirty combined-matrix update also caches four matrix columns. A
  single unlit vertex produces X/Y/Z/W in four NEON lanes. This fits the
  immediate-mode internal interface; the previous four-vertex batch kernel
  would otherwise fall back to scalar for each one-vertex call.
- Preserve upstream's implicit input W=1, constant-output-W shortcut, clipping,
  perspective divide and viewport mapping. Preserve operation ordering and
  disable contraction/autovectorisation in this diagnostic. Tests cover finite
  ordinary inputs, not a guarantee of bit-identical exceptional/subnormal math.
- Lighting remains scalar; there is no lighting-speed claim. Both methods
  refresh cached columns so switching methods without changing matrices is valid.
- Tests allocate a private 640×480 colour/depth buffer inside the launcher's
  owned RAM and release it before the DOS/EGA lifecycle fixture. No framebuffer
  scanout, Zorro transfer, display flip, firmware change or installed-app change.

## Measurement design

Three smooth-shaded, depth-tested scenes use real nonidentity model/projection
matrices and perspective W. Input generation is outside timing:

| Scene | Input triangles | Purpose |
|---|---:|---|
| Geometry | 1,024 | Many small triangles, relatively little pixel work |
| Clipping | 128 | Narrow triangles crossing the frustum; one triangle can split into two |
| Fill | 32 | Large overlapping triangles, colour/depth pixel work |

Each unprofiled batch is warmed. Scalar/NEON/NEON/scalar ordering gives 16 timed
frames per method per scene. Each pass checks colour plus depth hashes against
the original path. Separate profiled frames must reproduce the same output.

Exclusive nested timing buckets:

- **Transform:** combined-matrix work, vertex transform/clip-code calculation,
  and ordinary viewport mapping. This includes unmodified perspective/colour
  conversion, not only the new NEON instructions.
- **Clip/dispatch:** triangle classification, culling/dispatch and recursive
  clipping/interpolation, excluding child raster calls. Clipped-vertex viewport
  work remains here.
- **Raster:** triangle-fill dispatch and colour/depth drawing.
- **Clear:** colour/depth buffer clear.
- **Other:** matrix API setup, immediate-mode submission, colour changes, GL
  bookkeeping and profiler overhead outside child spans.

The existing read-only 333,333,333 Hz global timer is used; no PMU, shared L2,
firmware or timer configuration is changed. Stage times sum exactly to profiled
wall time. Per-vertex timer hooks perturb execution, especially small triangles:
**stage shares are instrumented observations, not unbiased production shares**.
Overall speed comparisons come exclusively from the separate timer-disabled
runs. Both paths still carry the same disabled profiling branches and matrix
column-cache maintenance; this is an instrumented-build A/B comparison, not
a comparison against an untouched production binary. The report records profiling slowdown to make this limit visible.

The 400-byte result is bounded within the existing diagnostic reply area.
Cancellation checks run between bounded batches, close the TinyGL context and
both buffers, then use the normal SIMD/MMU/register/cache restoration path.
The instruction model's quick flag reduces repetition only; its times are not
hardware performance evidence.

## Reproduction

```sh
python3 scripts/test_dosbox_tinygl_bench.py
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
  --pc-pic-vfp --pc-matrix-bench --pc-tinygl-bench
```

Use the resulting build directory with
`tests/amiga/dosbox/check_tinygl_arm.py --build DIR` in the ARM-model environment.
After checking the physical machine is idle, use
`scripts/measure_dosbox_tinygl.py stage --build DIR`, then `run --build DIR
--label tinygl-1` (fresh label for each repeat) in the MCP Python environment.
This stages only `RAM:ZZTinyGL`. The runner verifies executable CRC, waits for
closed logs, checks restoration/release, and confirms the installed runtime's
checksum did not change. Do not launch another worker after an observation
failure without inspecting machine state.

Analyse closed logs with `scripts/analyse_dosbox_tinygl.py LOG... --output JSON`.
Keep exact binaries, provenance, logs, tests and source hashes with the results.

## Raster follow-up

The [NEON span and zero-clear experiment](amiga-tinygl-raster-experiment.md)
subsequently measured 22.93% more visible-fill throughput, but 18.66% less
throughput for mostly occluded fill. Zero clearing improved 7.75%. These remain
separate native TinyGL diagnostics; the span candidate is not enabled globally.

## hlsl++ follow-up

The [hlsl++ comparison](amiga-hlslpp-benchmark.md) reproduced exact output on the
physical A9 but measured only 0.19% more geometry throughput than scalar TinyGL
and 0.71% more than the internal NEON candidate, with no material clipping/fill
gain. Its generic matrix/vertex routines were slower than the specialised NEON
kernels. hlsl++ remains confined to the benchmark; the installed renderer is unchanged.
