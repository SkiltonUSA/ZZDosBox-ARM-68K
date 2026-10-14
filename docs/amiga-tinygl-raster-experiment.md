# TinyGL NEON spans and buffer clears

Follow-up: [hidden-pixel rejection and dedicated-build measurements](amiga-tinygl-rejection-experiment.md).
The revision 1/2 results below are preserved as historical evidence.

This follow-up targets the triangle-drawing bottleneck identified by the
[internal-transform experiment](amiga-tinygl-internal-transforms.md). It is a
separate native A9 diagnostic. It does not intercept DOS game geometry or
change the installed DOSBox presentation path.

## Physical result: useful for visible fill, unsuitable as a universal replacement

Three fresh runs on the A4000TX's ZZ9000 Cortex-A9 at 666 MHz completed with
profiling disabled. Revision 2 improved the visible-fill scene from **78.356 to
63.740 ms** with NEON spans alone: **22.93% more throughput**, or **18.65% less
frame time**. Enabling zero clearing too gave **63.680 ms / 23.05% more
throughput**. These are complete offscreen scene times including clearing,
transformation, clipping and rasterisation, not isolated inner-loop timings.

The mostly occluded scene regressed from **41.179 to 50.625 ms**: **18.66% less
throughput**. Small-triangle and clipping scenes gained no meaningful speed from
the span replacement alone. Therefore keep the span candidate experimental and
disabled by default; it is not a general TinyGL or DOSBox speedup.

Zero colour/depth clearing improved from **1.228 to 1.140 ms**, **7.75% more
throughput**. Used alone, it saved about 0.10 ms in the geometry scene and
0.09 ms in the clipping scene (1.58% and 2.10% throughput gains respectively).
Nonzero clears retain the scalar implementation and remain effectively unchanged.

Median milliseconds across three physical runs; lower is better:

| Case | Original | NEON spans | Zero clear | Both |
|---|---:|---:|---:|---:|
| geometry | 6.557 | 6.557 | 6.455 | 6.460 |
| clipping | 4.403 | 4.402 | 4.313 | 4.314 |
| fill | 78.356 | 63.740 | 78.276 | 63.680 |
| occluded-fill | 41.179 | 50.625 | 41.085 | 50.532 |
| clear-zero | 1.228 | 1.227 | 1.140 | 1.140 |
| clear-arbitrary | 1.084 | 1.084 | 1.084 | 1.083 |
| clear-colour-only | 0.724 | 0.725 | 0.725 | 0.725 |

Exact per-run ticks, timing ranges, output hashes, throughput gains and time
reductions are in `amiga/records/2026-10-14/tinygl-raster/v2/v2-results.json`.
This compares switches within the same diagnostic executable. Even its scalar
baseline differs from the previous internal-transform build (geometry 6.557
versus 6.030 ms); added branches and generated-code/layout differences are not
isolated. A production-build comparison is still required before promoting
any change. These results do not measure DOS game FPS, glDrawPixels performance,
XACP transfer bandwidth or display scanout.

The first candidate was much slower (preserved below). Revision 2 combines
aligned stores, a longer-span threshold and separate fully rejected/fully
visible/mixed depth paths. Assembly confirms that rejected blocks avoid colour
loads and stores, while fully visible blocks avoid read/modify/write. The
compiler still hoists colour arithmetic and transfers depth-mask information
from NEON to ARM registers. Their individual costs have not been measured;
these observations do not prove the cause of the remaining occlusion regression.

The next useful experiment is an isolated comparison of rejection-heavy spans
and depth-mask handling, followed by a standalone production-build comparison
of zero clearing. Any automatic choice of scalar versus NEON spans needs its
own correctness and timing evidence. Do not enable the current span candidate
globally.

## Verified execution and retained evidence

All native sanitizer tests and all four linked ARM lifecycle tests passed. Each
physical run passed the 656-case kernel self-check and exact colour/depth hash
comparisons for every method. The first three scene hashes also matched the
previous physically verified scalar results. All runs completed EGA/PIT and
DOS shell checks, SIMD/MMU/register/cache-witness restoration and owned-memory
release without reported errors. The bridge and Workbench remained responsive;
post-run checks found no Core1 owner or Software Failure window.

Only the separate `RAM:ZZRaster/ZZRasterBench` diagnostic was staged:
1,171,224 bytes, CRC32 `8FAD32D1`, SHA256
`e7cad410fcc6299b75404622f6da59b13cf33e30adb8b20dbafcb3aa91cdbfa6`.
Installed DOSBox remained CRC32 `5C74FFFC`. No installed launcher, profile,
startup or firmware changes were made. No GitHub publication or release was
created for this experiment.

`amiga/records/2026-10-14/tinygl-raster/v1/` and `v2/` preserve exact binaries,
ELFs, compiler/build metadata, TinyGL provenance/license, source snapshots,
source hashes, native/model test logs, physical closed logs, bridge checks,
disassembly and machine-readable results. Each revision has a SHA256 manifest.

## Implementation

Pinned TinyGL: `jserv/tinygl`, revision
`c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a`.

The candidate replaces complete four-pixel groups in smooth-shaded, untextured,
non-blended 32-bit spans. It covers all four depth-test/depth-write combinations:

- Interpolate fixed-point colour and depth in four NEON lanes using exact
  unsigned 32-bit recurrence; no floating-point interpolation changes.
- Compare the full shifted depth against existing 16-bit depth **before**
  narrowing for storage. Preserve greater-than-or-equal depth semantics.
- Test the depth mask per four-pixel block: wholly rejected blocks perform no
  colour load or stores; wholly visible blocks store directly; mixed blocks use
  per-lane masks. All stores operate only on the owned RAM buffer.
- Preserve scalar handling of tails and short spans. A minimum span of 32 pixels
  avoids NEON setup on tiny triangles. Scalar leading pixels align vector colour
  stores to 16 bytes. Four-pixel groups never read across the span boundary;
  clipping and triangle-edge traversal remain upstream code.
- Polygon stipple, when compiled/enabled, uses the original scalar span. Flat,
  textured and blended rasterisation are unchanged.

The separate clear candidate uses NEON stores for 32-bit colour and 16-bit depth,
with scalar tails. Revision 2 selects this path only when all selected buffers
are being cleared to zero; nonzero clears retain upstream code. It preserves
row pitch, padding, values and buffer selection. It falls back for unsupported
render formats/dirty-rectangle builds.
No clear is skipped and depth correctness is not relaxed. The A9 build uses
`-fno-strict-aliasing` for TinyGL/raster units because this toolchain defines
`uint32_t` as unsigned long while TinyGL pixels are unsigned int; both are
asserted to be 32 bits before using NEON memory intrinsics.

The unsuccessful internal-transform optimisation stays disabled. Both candidate
switches default off; only the separate benchmark selects them. No firmware,
XACP, shared L2, framebuffer scanout, installed application or startup changes.

## Measurement design

Four methods run in the same binary: original, spans only, clear only, and both.
The warmed order is A/B/C/D/D/C/B/A. Profiling is disabled throughout timing.
Input generation and colour/depth hashing are outside timed batches.

Four 640×480 scenes include the previous 1,024 small triangles, 128 clipping
triangles and 32 large overlapping triangles, plus the large scene in reverse
depth order to measure mostly rejected pixels. Each method gets
16 timed frames per scene. Colour and depth output hashes must match the original
before and after every timed batch. The three scene hashes must also match
the previous verified physical scalar run, guarding against a shared regression
in both methods.

Three additional cases measure clearing alone, 128 calls per method:
zero colour/depth, arbitrary colour/depth, and colour-only with depth preserved.
The spans-only switch has no effect on these cases, providing an internal
control. Clearing is timed independently rather than inferred from frame gains.

The existing read-only A9 global timer is used at 333,333,333 Hz. The report uses
352 bytes in the existing bounded diagnostic reply area. All buffers live inside
the launcher's owned allocation. Cancellation between bounded batches closes
TinyGL and its buffers before normal CPU/context restoration. The instruction
model reduces repetitions only; its timings are not physical speed evidence.

## Validation design

- Native ASan/UBSan: 10,304 span cases covering widths 0–643, alignment, all four
  depth modes, integer wrapping, untouched prefix/tails and exact scalar output.
- Clear tests cover 536 length/alignment cases and 560 combinations of odd
  widths, padded row pitch, selection flags, zero/nonzero/negative depth values.
- Sixteen complete scene/depth-mode combinations compare all four methods byte
  for byte across both colour and depth buffers.
- A bounded 656-case span/clear self-check also executes inside the ARM payload
  before benchmarking, including guards and full-width depth comparison.
- The linked lifecycle model checks normal execution, framebuffer-copy
  cancellation, cancellation inside a scene, unavailable SIMD and complete
  SIMD/FPSCR restoration before physical execution is accepted.

## Reproduction

```sh
python3 scripts/test_dosbox_tinygl_raster.py
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
  --pc-pic-vfp --pc-matrix-bench --pc-tinygl-bench --pc-tinygl-raster
```

Choose the running cross-compiler container with `--container-command` if needed.
Run `tests/amiga/dosbox/check_tinygl_arm.py --build DIR` in the ARM-model environment.
After those tests pass and the physical machine is idle, use
`scripts/measure_dosbox_tinygl_raster.py stage --build DIR`, then
`run --build DIR --label raster-1`, with a fresh label for every run, in the MCP
Python environment. Staging is confined to `RAM:ZZRaster`. The runner verifies
CRC, waits for closed logs and checks cleanup and the installed runtime checksum.

Use `scripts/analyse_dosbox_tinygl_raster.py LOG... --output JSON` for physical
results. Speedup means baseline time divided by candidate time; throughput gain
and elapsed-time reduction are recorded separately.

## Initial candidate: measured regression

Three physical runs passed all output and restoration checks. Nevertheless,
branchless masked read/modify/write regressed visible fill from 78.325 to
109.307 ms, and mostly occluded fill from 40.840 to 109.384 ms. Those results
reject the first span implementation. Assembly shows an unconditional colour
load/store and depth store even for rejected pixels; this is additional work
compared with the original early rejection. That observation motivated the
second implementation; it is not a measurement of the individual overheads.

Zero clearing improved from 1.228 to 1.121 ms, while arbitrary colour/depth and
colour-only clears regressed. Revision 2 therefore keeps nonzero clears scalar.
The original executable, sources and three logs are preserved under
`amiga/records/2026-10-14/tinygl-raster/v1/`. Revisions must be compared against
their own same-binary scalar baseline; the generated code and layout differ.
