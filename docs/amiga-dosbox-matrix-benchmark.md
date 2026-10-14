# Cortex-A9 matrix experiment

This is an isolated matrix and native 3D workload experiment. The current
ZZDosBox OpenGL presentation path calls `glDrawPixels` on an already rendered
DOS image; it does not offload the DOS game's geometry calculations. Matrix
results must not be presented as a Doom, Vette or general DOSBox FPS improvement.

## Implementation and scope

Three implementations share row-major single-precision inputs and non-overlapping
outputs: scalar VFP, pinned CMSIS-DSP NEON, and independent specialised ARMv7
NEON. The specialised 4x4 kernel keeps four output rows in vector registers.
The vertex kernel transforms four AoS `[x,y,z,w]` vertices at a time using
interleaved loads/stores, independent accumulators and bounded scalar tails.
Neither uses ARMv8 instructions or assumes fused multiply-add support.

The CMSIS wrapper computes batches as V times transpose(M); its transpose,
wrapper calls and upstream packing are inside the measured interval. The
implementation is unmodified CMSIS-DSP revision
`83a2d7bc98c81b4bbe4a6f48b1f2ecf179868a0b` (Apache-2.0). The current implementation
uses shared packing buffers. Explicit 16/32/32 KiB blocking budgets bound its
scratch allocation. These are algorithm tile budgets, not detected hardware
cache sizes; no cache-controller setting changes. This implementation is used
by a single worker, not concurrently. Upstream standalone compiler definitions
avoid importing CMSIS startup, interrupts or an RTOS.

The synthetic scene transforms 128 or 1024 coloured triangles, then submits
clip-space vertices to pinned jserv TinyGL at 640x480 with depth testing and
identity GL matrices. Timing includes transforms, colour/depth clears, API
submission, TinyGL's remaining vertex processing, clipping and rasterisation.
It excludes context creation, validation hashes and display handoff/scanout.
It is **offscreen software rendering**, not a new guest OpenGL driver or a
measured display frame rate. No lighting, textures, animation or game simulation
are included. A scene improvement would establish value only for this workload.

A host sanitizer found signed left shifts of negative edge deltas in TinyGL's
`ztriangle.h`. The diagnostic staging replaces those two shifts with multiplication
by 65536, bounded by the 640-pixel viewport. Existing installed GL code is not
modified by this experiment. Both versions of matrix math use the same patched
rasterizer. Provenance records retain the patch hash and original revision.

## Measurement method

One executable contains all three paths. After checked SIMD acquisition, each
case warms and measures scalar/CMSIS/specialised/specialised/CMSIS/scalar order.
The A9 global timer supplies ticks (333333333 Hz). The scalar baseline is
hardware VFP at `-O2`, with automatic vectorisation and FP contraction disabled;
it is not software floating point. The same optimisation level applies to the
NEON paths. No LTO or fast-math assumption is used. Matrix inputs, outputs and
render targets are in private cached ARM memory inside the Exec-owned allocation.

The shared report fits in the existing application-owned diagnostic region;
trace, memory-debug, pixel-benchmark and OpenGL presentation variants cannot
share this build. The original MMU/SIMD/cache-witness and single-Core1 protocols
remain in force. The benchmark checks stop requests between bounded batches
and closes its GL context and buffers before cancellation returns.

## Physical results — A4000TX, 14 October 2026

Three clean physical runs; medians below. Times are microseconds per complete
operation/batch or offscreen frame. Gains are throughput gains relative to scalar
VFP (`scalar time / candidate time - 1`), not reductions in elapsed time.

| Workload | VFP µs | CMSIS NEON µs | Specialised NEON µs | Specialised throughput gain |
| --- | ---: | ---: | ---: | ---: |
| 4x4 matrix product | 0.623 | 0.918 | 0.230 | +171.24% |
| 16 vertex transforms | 2.362 | 1.950 | 1.503 | +57.10% |
| 256 vertex transforms | 36.945 | 27.928 | 22.843 | +61.73% |
| 1024 vertex transforms | 148.501 | 115.678 | 93.118 | +59.48% |
| 128 triangles, 640x480 offscreen | 2317.211 | 2349.421 | 2305.728 | +0.50% |
| 1024 triangles, 640x480 offscreen | 10011.097 | 9995.583 | 9991.423 | +0.20% |

The specialised matrix kernel is **2.71x** the scalar VFP throughput; the
1024-vertex batch is **1.59x**. CMSIS improves that batch by **28.38%**, but its
general matrix/packing overhead makes the isolated 4x4 product **32.19% slower
in throughput** than scalar VFP. This is not a comparison of large square GEMM
workloads or all CMSIS routines.

The full scene gain is small: **0.50%** for 128 triangles and **0.20%** for
1024 triangles. The dense scene was slightly slower with specialised NEON in
one of the three runs; timing ranges overlap. There is no established useful
scene-frame-rate improvement here. Submission, remaining TinyGL transforms,
clipping, depth and rasterisation remain in the scene path. A separate stage
profile would be needed to assign the small benefit to specific bottlenecks.

The result supports retaining the reusable kernels for native ARM workloads.
It does not justify changing the DOSBox presentation default, installing a
new game runtime, or claiming faster emulated x86 matrix calculations.

### Validation and hardware state

- Host ASan/UBSan: 1000 random matrix pairs, bounded vector allocations/tails,
  double-oracle accuracy, nonblank identical scene and teardown.
- Actual A9 machine code in Unicorn: 366 calls; counts 0 through 4096 including
  unaligned-to-16-byte inputs, vector tails, strict access bounds, immutable
  inputs and output guards. Maximum scaled error against the double oracle:
  `1.5799924310432576e-7`, below the `2e-6` bound.
- Linked ARM lifecycle: original EGA/PIT fixture, display-copy cancellation,
  denied SIMD access, and cancellation inside the matrix scene. Colour/depth
  buffers and GL context closed; all D0–D31 registers and FPSCR restored.
- Physical runs: all six cases checked exact dyadic matrix/vector outputs and
  matching scene hashes with zero errors; EGA/PIT passed, MMU/register/cache
  witnesses restored, file handles closed and owned memory released.

The executable was staged only as `RAM:ZZMatrix/ZZMatrixBench`, with its
original fixture. No installed runtime, launcher, firmware, OS driver or saved
profile was changed. Normal runtime CRC32 remained `5C74FFFC`. Final bridge
and Workbench checks were responsive with no Core1 owner or Software Failure.

Measured diagnostic: 1169592 bytes, CRC32 `048382F7`, SHA256
`1ed574c1364eec041f2f1ab4cd32a56ac12af8d60b1de293195a2afaadfbd2bb`.

[Raw measurements, executable, provenance and validation](../amiga/records/2026-10-14/dosbox-matrix/)
retain all three runs and timing ranges. The executable is a diagnostic
harness, not a new distributable DOSBox release.

## Reproduction

```sh
python3 scripts/test_dosbox_matrix.py
python3 scripts/build_dosbox_dos_arm.py \
  --container-command '["podman","--connection","nuflix-converter-root"]' \
  --pc --pc-dynrec --pc-jit-mib 4 --pc-neon --pc-pipeline \
  --pc-scanlines --pc-presentation --pc-adapters --pc-pic-vfp --pc-matrix-bench
```

Use `tests/amiga/dosbox/check_matrix_arm.py --build <build-dir> --lifecycle`
with the existing Unicorn environment. This checks the actual ARM machine code,
ordinary finite values against a double-precision oracle, strict array bounds,
non-vector tails, scene equality and the normal/stop/denied-SIMD lifecycle.
Instruction-model repeats are shortened and cannot generate physical timings.
These tests do not establish NaN, subnormal or exception equivalence between
VFP and NEON, nor compatibility with x87 extended precision.

Read the hardware handoff before staging. `scripts/measure_dosbox_matrix.py`
requires explicit `stage` or `run`, a build directory, and a fresh evidence label.
It checks for an idle Core1 and Software Failure requesters, stages only in RAM,
verifies transfer checksums, and reads the log only after the run has closed it.
The installed runtime checksum must remain unchanged. Analyse completed logs with
`scripts/analyse_dosbox_matrix.py <logs...> --output <results.json>`.

## References

- Feng et al., *An Optimized Matrix Multiplication on ARMv7 Architecture*,
  JCIT 7(10), June 2012, DOI `10.4156/jcit.vol7.issue10.5` (user-supplied paper).
  Inspiration: independent accumulators, register reuse, cache blocking and
  contiguous data. No implementation code copied from the paper. Its 7–20x
  claims concern its own baselines/platforms. ARMv7 NEON does not implement
  double-precision arithmetic despite the paper's statement to that effect.
- [CMSIS-DSP](https://github.com/ARM-software/CMSIS-DSP/tree/83a2d7bc98c81b4bbe4a6f48b1f2ecf179868a0b)
- [Pinned TinyGL](https://github.com/jserv/tinygl/tree/c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a)

## Follow-up: actual TinyGL internals

The [internal-transform experiment](amiga-tinygl-internal-transforms.md) replaces
TinyGL’s own unlit transforms and profiles the pipeline on the physical A9.
It found no whole-scene improvement; the geometry workload was 0.58% slower,
and the fill workload was dominated by triangle drawing. This supersedes any
expectation that the isolated matrix/vertex gains automatically improve TinyGL.
