# Physical A9 Glide translation cost

The first CPU cost measurement is encouraging: **320 textured triangles took
1.077 ms median on the ZZ9000 at 800 MHz**. This is command encoding, not rendered
game FPS. It supports continuing to a bounded GPU rendering benchmark; it does
not yet establish that GTA can reach 25, 30 or 60 FPS.

## Results

Three independent launches each ran one discarded warm-up and ten measured
samples. All 30 samples produced the expected `58e7d18a` packet hash.

| Launch | Timer calibration | Minimum | Median | Maximum |
|---|---:|---:|---:|---:|
| 1 | 399,999,286 Hz | 1.0767 ms | 1.0789 ms | 1.0867 ms |
| 2 | 399,998,176 Hz | 1.0672 ms | 1.0698 ms | 1.0741 ms |
| 3 | 399,998,584 Hz | 1.0744 ms | 1.0785 ms | 1.0873 ms |

Combined median: **1.07717 ms**, approximately **3.37 µs per triangle** for this
workload. These figures come from raw timer ticks divided by each launch's
calibrated timer frequency, rather than the rounded microseconds in the console.
The held 800 MHz ARM setting was left unchanged.

| Target frame rate, not achieved FPS | Total frame budget | Encoding these 320 triangles | Budget used | Remaining for all other work |
|---|---:|---:|---:|---:|
| 25 FPS | 40.00 ms | 1.077 ms | 2.69% | 38.92 ms |
| 30 FPS | 33.33 ms | 1.077 ms | 3.23% | 32.26 ms |
| 60 FPS | 16.67 ms | 1.077 ms | 6.46% | 15.59 ms |

This is not a captured GTA scene. Its triangle count, state changes and texture
usage must not be assumed to represent GTA. Dividing one second by encoding time
would produce an encoder throughput figure, not a useful game FPS estimate.

## What was measured

The existing translator and descriptor code encode 64 batches of five in-bounds,
textured triangles, producing 57,344 bytes. The synthetic viewport is 640×480,
the texture descriptor is 64×32 RGB565, and batches vary origin and opaque,
source-alpha or additive blending. Coordinates, colours and perspective-weighted
texture coordinates vary. No texture or framebuffer memory is accessed.

The timed interval includes batch configuration checks, triangle validation,
floating-point descriptor calculation, packet construction and a cancellation
poll per batch. Vertex preparation, output hashing, FP-state preservation and
68k mailbox service are outside it. It excludes guest x86 execution, Glide call
decoding, clipping, texture allocation/upload, command transport, GPU rendering,
presentation and audio. No GPU command was submitted.

Core1 uses an Exec-owned 1 MiB allocation, the existing mapping probe and private
MMU/I/D-cache contract. Private memory uses inner write-back/write-allocate;
shared memory and outer cache remain noncacheable. The kernel uses ARMv7/VFPv3
softfp with floating-point contraction disabled. The entry and context wrapper
use soft-float; they check availability and preserve all D registers and FPSCR.
The model and hardware outputs match the native packet hash.

All three runs passed active cancellation, return epilogue, cache/MMU/register
restoration, guard/witness checks and owned-memory release. The bridge remained
responsive, Core1 returned idle and the final reported temperature was 58.8°C.
The temporary executable was removed. No installed emulator, firmware or clock
setting changed.

## Reproduce

```sh
python3 scripts/build_dosbox_glide_capacity.py --connection podman-machine-default-root
python3 tests/amiga/dosbox/glide/capacity_model.py
python3 scripts/test_dosbox_glide.py --arm
```

The build uses native Clang ASan/UBSan checks, the ARM cross-compiler and the
pinned m68k container. The model requires Unicorn. It executes the compiled
arithmetic and FP save/restore, but mocks feature discovery because Unicorn's
CP15/MVFR reporting differs from the physical A9. Its instruction timing is not
used. The unchanged physical helper performs the real availability check.

With exclusive Core1 access and the documented mapping/teardown prerequisites,
copy the generated `ZZGlide-Capacity` executable to RAM and run with a 65,536-byte
stack. The common harness calibrates the timer, verifies packets, takes eleven
samples and tests cancellation. Require `GLIDE_CAPACITY PASS`, restoration checks
and `LAUNCHER exit=0; owned memory released` before repeating. A timeout or
unverified cleanup stops the series.

The generated host reuses the common benchmark harness; its sample line prints
`triangles=320` twice because both the instruction and cycle fields are renamed.
They describe the same workload, not 640 triangles.

[Results](results.json), [build provenance](build.json), [artifact hashes](sha256.json),
[run 1](run1.txt), [run 2](run2.txt), [run 3](run3.txt),
[native checks](native.log), [compiled ARM checks](model.log),
and [encoder regression checks](encoder-tests.log) retain the evidence.

## Next decision point

Measure completed rendering of a small offscreen scene, including submission,
GPU retirement and pixel verification. That requires resolving the existing
completion/readback problem; a fast submission acknowledgement is insufficient.
Then integrate the guest renderer and measure completed GTA frame presentations.
These are the remaining costs needed for a credible FPS estimate. There is no
matched prior encoder benchmark, so no percentage speedup is claimed here.
