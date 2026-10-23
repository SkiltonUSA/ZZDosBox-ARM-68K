# TinyGL rasterisation references

Reviewed 14 October 2026 alongside the ZZDosBox software-GL presentation experiment.
These are source reviews, not measurements of either repository on the A9.

## Niravaana/TinyGL

[Repository](https://github.com/Niravaana/TinyGL), reviewed revision
`16d3e74ec1f33f6f221e8a32a3d0d6ac6ff69821`. Despite the shared name, this is
a separate educational C++ renderer, not the Bellard/jserv TinyGL used here.

`src/OpenGL/Context.cpp` implements:

1. Model-view and projection transforms, perspective division and viewport mapping.
2. A screen-space bounding box per triangle and pixel-centre coverage sampling.
3. Three floating-point edge functions to determine coverage and barycentric weights.
4. Interpolated depth with a depth-buffer comparison, then interpolated RGB colours.
5. A separate conversion from float RGB to RGBA8 and `stbi_write_png("output.png", ...)`.

It is a readable illustration of Gouraud-shaded triangles. The inspected code has
no ARM/NEON path, tile traversal or incremental edge stepping. It evaluates the
edge functions and divides their weights by triangle area inside the pixel loop.
It does not clip triangles in homogeneous space before dividing by `clip.w`,
and has no explicit shared-edge top-left coverage rule. Textured perspective
interpolation is not implemented in the raster loop. Windows headers and an HDC
are part of its context, so it is not a freestanding A9 drop-in.

Useful directions for an independent A9 experiment:

- Clip before perspective division; define shared-edge ownership and test cracks.
- Compute reciprocal triangle area once and increment edge values across spans.
- Evaluate several adjacent pixels with NEON and use coverage/depth masks.
- Reject empty tiles early; measure whether tiling offsets its setup cost.
- Keep packed colour/depth buffers bounded, preallocated and cache-aware.
- Render only into an owned XACP back buffer and retain the existing flip protocol.

These are proposed optimisation techniques, not measured gains. A useful test
matrix would include small/large triangles, overdraw, clipping, textured/untextured
paths and exact image comparisons before hardware throughput measurements.

## oaktree11/tinygl-arm-versatilepb

[Repository](https://github.com/oaktree11/tinygl-arm-versatilepb), reviewed revision
`eee6875eac6624691aaf5c06486eee0e87c116d9`. Bare-metal TinyGL examples, launched by
QEMU VersatilePB. C targets ARM926EJ-S; startup assembly targets Cortex-A9. No
NEON rasteriser was found. `lcd.c` points TinyGL at alternating PL110 display
buffers. Its direct LCD register writes and optional GPU registers at
`0xD0000000` are not ZZ9000 interfaces. The swap routine has no explicit vblank
wait/acknowledgement. Its allocator never frees and has no heap-bound check.

Its useful reference is binding a renderer to a display back buffer. Our
experiment adapts that idea through the existing checked XACP mapping, never
through copied PL110 addresses or that allocator.

## Scope in ZZDosBox

The current renderer uses `glDrawPixels` on an already rendered DOS image.
Triangle improvements cannot accelerate that path or the emulated x86 CPU.
Guest OpenGL/3dfx support would require a separate guest/API integration. A
future native Amiga 3D application could submit geometry to the ARM instead.
Keep the pinned jserv implementation; neither reviewed repository establishes
a reason to replace it. No source was copied from these repositories.

## ARM cache-controller reference

The supplied DDI0246H link is the L2C-310 manual, section 2.5.5.
[Readable ARM PDF](https://documentation-service.arm.com/static/5e8e1df188295d1e18d365bb).
[AMD's Zynq description](https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/Basic-Functionality?contentId=TJb1LGALdGeY973v~OqT5Q)
confirms the CPUs share L2. Treat configuration as firmware-owned. Next useful
investigation is a read-only audit of firmware/controller revision and enabled
features against XACP's contract, before considering changes. No L2 configuration
was changed in either renderer candidate, and this reference does not diagnose
the observed freeze. Keep CPU-local code improvements separate from shared
controller tuning.


## Follow-up: diagnostic build and SIMD paper (14 October 2026)

The failed loop refinement remains withdrawn. Source/disassembly review found
no demonstrated out-of-bounds access in its external-buffer branch: it still
uses the bounded horizontal sample table, validated pitch and scalar 32-bit
stores. It does not introduce vector stores or change cache-controller state.
The private fallback uses a tightly packed context-owned buffer. This review
has not established a root cause; the missing completion log prevents locating
the failing phase. A whole-machine freeze remains a hardware failure despite
passing native and instruction-model checks.

`--pc-video-trace` now creates a separate `-trace` output directory and labelled
diagnostic executable from the earlier tested renderer. It adds a 16-record
ring in the existing uncached communication region, after the GL report and
before boot text. Records contain commit sequence, phase, frame, detail and
A9 timer ticks. The 68k reader checks the commit twice, reports dropped records
and flushes progress output only when the display is unlocked and no flip is
pending. This avoids invoking console/file requesters while holding the bitmap
lock; sustained rendering can therefore drop records, which is reported. It records prepare, grant wait/acquisition, map,
nonce verification, render, rendered, unmap, completion and GL close.

Tracing does not alter buffer ownership or authorize recovery. It has no
firmware callbacks, shared-L2 writes or new allocator. Its extra barriers and
logging affect timing: do not use this executable for FPS comparisons. A full
bus/OS freeze can prevent logging; the ring is volatile and lost on reboot.
No promise of crash-proof capture is made. Live Amiga log files must still not
be read through the bridge; collect only after the completion marker, or observe
Shell output directly during an attended test.

The short physical Hercules checks subsequently passed on 14 October 2026,
staged separately with Stack 65536 and an idle Core1 exclusive-access check. Direct and
OpenGL both paused before frame 2, stepped to frame 3 and continued to clean
completion. Both also stopped cleanly after frame 2 with tracing disabled.
These are lifecycle checks, not game benchmarks or a diagnosis of the earlier
freeze. The withdrawn binary was not retried; installed runtimes remain intact.
See `amiga/records/2026-10-14/dosbox-developer/physical-method.md`.

### What the existing profiles already show

Reanalysis of the completed 14 October Doom logs gives these exclusive buckets:

| Bucket | Direct | First direct-target GL |
| --- | ---: | ---: |
| Emulated CPU execution | 53.395% | 39.291% |
| Events | 21.081% | 15.180% |
| VGA capture | 5.165% | 3.785% |
| Buffer grant wait | 5.479% | 4.228% |
| Mapped presentation | 9.062% | 33.239% |
| Other | 5.818% | 4.276% |

These are whole profiled runs, including startup; not the narrower Doom tic
interval. Profiling itself has overhead, especially across millions of scopes.
The copy bucket includes mapping, rendering, optional overlay and unmapping;
the grant bucket is not a pure measurement of scanout-flip latency. CPU and
event scheduling deserve investigation before assuming display waits dominate.
Halving the entire Direct copy bucket would give about 4.75% overall speedup
under an otherwise unchanged workload; that is an estimate, not a measured gain.

### SIMD reference

[Use of SIMD Vector Operations to Accelerate Application Code Performance on
Low-Powered ARM and Intel Platforms](https://users.cecs.anu.edu.au/~Eric.McCreath/papers/Use_of_SIMD_Vector_Operations_To_Accelerate_Application_Code_Performance_on_Low_Power_ARM_and_Intel_Platforms.pdf)
compares hand-written intrinsics with compiler vectorization in image kernels,
including Cortex-A9 platforms. Its compiler baseline is GCC 4.6.x, whereas our
ARM build uses GCC 15; its kernel speedups cannot predict our application FPS.

Our existing `pixels_neon.cpp` already vectorizes RGB565 conversion, 32-bit
copies, 1:1/2x scaling, byte swaps, black rows and dimming. General-ratio scaling,
15-bit capture and the TinyGL draw-pixels loop are possible follow-ups, subject
to profiling and exact-pixel/edge tests. Indexed palette lookup needs separate
care because pixels select nonconsecutive table entries. The x86 instruction
stream and event dependencies do not become parallel merely by enabling NEON.
No SIMD implementation was changed for this reference review.


### Developer tab (local 0.11.1-dev)

The MUI tab starts with **Development only: diagnostics affect performance.**
Controls are Trace logging, CPU/video profiling, Frame breakpoint (Off / Before
frame / After frame), Frame number, Display handoff (Default / Serial /
Pipelined), Host polling (Default / 1 / 4 / 20 ms), and Open log. All overrides
are session-only and default off; existing program profile/session formats stay
unchanged. The normal application retains its default runtime selection.

Trace or checkpoint selection chooses `ZZDosBox-Trace` or
`ZZDosBox-OpenGL-Trace` alongside the launcher. Missing optional runtimes fail
before launch rather than silently ignoring the request. Profiling/handoff/
polling alone use the normal selected runtime. The new launcher is a local
build, not an installed or released version.

A checkpoint is cooperative, not an instruction/address breakpoint. It pauses
before acquiring a presentation buffer or after revoking its ARM mapping and
publishing completion. The host still services files, MIDI, input and display
acknowledgements. Only after the display lock and pending flip are cleared does
it show Continue / Next frame / Stop. The requester is polled without blocking
the host loop. Continue disables the breakpoint; Next frame retains its phase
and advances the requested presentation number by one; Stop uses the existing
cooperative cancellation and teardown. Adaptive skipped frames do not count.
Neither checkpoint control nor tracing can recover an arbitrary ARM/OS freeze.

CLI equivalents on a Trace runtime:

```text
--trace-video
--break-before 2
--break-after 2
```

Choose one breakpoint phase. Frame range is 1..10000000. Trace is independent
of checkpoints. Existing `--profile`, `--serial-video`, `--pipeline-video`,
`--poll-ms 1|4|20` switches back the other tools. Frame pauses disable the
host's automated 90-second workload watchdog; Ctrl-C/Stop and the existing
post-stop timeout remain. Paused time invalidates benchmark comparisons.

Reproduce the local builds with the existing full adapters/NEON/pipeline build
flags plus `--pc-video-trace`; add `--pc-opengl` for the GL sibling. Output goes
to a separate `-trace` directory. Build the launcher with
`python3 scripts/build_dosbox_launcher.py`. Native tests:
`python3 scripts/test_dosbox_developer.py` and
`python3 scripts/test_dosbox_opengl.py`. Linked ARM checks:
`tests/amiga/dosbox/check_video_trace_arm.py --out <trace-build-directory>`
using the existing Unicorn environment. These do not replace physical tests.

Physical MUI review confirmed disabled defaults, frame-field enablement, runtime
selection and reset on application exit. Serial handoff, profiling and 4 ms
polling reached the Direct runtime. Open log initially failed because it only
searched `C:MultiView`; the launcher now also checks `SYS:Utilities/MultiView`
and `SYS:System/MultiView`. The rebuilt launcher opened the completed log in
MultiView. The default launch after reset used the normal runtime with no
trace/checkpoint/profiling output. All five runs restored register/MMU/cache
witnesses, SIMD state and released owned memory; GL reported context closure.
