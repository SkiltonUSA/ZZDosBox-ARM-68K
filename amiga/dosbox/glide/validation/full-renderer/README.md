# Full renderer and completion simulation

The upstream RasterIX renderer now passes pixel checks through the proposed
response receiver, memory-error observer and completion mailbox. This is an
offline integration result. **It supplies no physical GPU or GTA FPS.**

## Results

Both command variants passed: the ordinary scene stream and an additional fence
before the marker's state changes. Both retain an end-of-batch fence followed by
an epoch/sequence response. These envelopes are experimental; the current XACP
validator does not accept them.

| Case, repeated for both variants | Result |
| --- | --- |
| Clear, solid red, additive green, textured and alpha-blended scenes | Five correct scene interiors |
| Delayed write responses and read beats | Same five pixel results; acknowledgement waits for outstanding transactions |
| Cancellation at response-token acceptance | Same five scenes retire before cancelled acknowledgement |
| Injected write-response error | Sticky fault; no completion acknowledgement |
| Injected read-response error | First two scenes pass; the first reading scene is quarantined |
| Deliberately force “drained” true | Test fails on acknowledgement with outstanding memory transactions |

In total: **34 checked framebuffer results, 124,890 interior-pixel comparisons,
four error-quarantine cases and two detected premature-completion mutations**.
These are repetitions of five small scenes, not 34 different workloads. The
existing oracle excludes triangle edges and texture boundaries, and allows its
documented alpha tolerance. Framebuffer guard words and texture contents stay
unchanged. Acknowledgements remain available while the consumer waits 25 cycles.

The delayed case holds each write response for 37 cycles and each read beat for
three cycles. It gates both sides of the handshake. Cancellation requests drain;
they do not discard unfinished memory work or authorize early memory release.

## What executes

- Complete upstream `RasterIX_EF`, one TMU, 64-bit AXI data, six-bit transaction
  IDs, coalescing enabled, texture cache disabled, fixed-point precision 18 and
  float precision 24. Remaining parameters use the pinned upstream defaults.
- Existing translator and physical-probe scene generators, with 64×72 RGB565
  framebuffers and synthetic texture memory.
- Proposed response-token receiver, AXI response-error observer and completion
  mailbox connected to the actual renderer's `idle` output.
- Upstream AXI RAM model, bounded to a synthetic 256 KiB aperture. The addresses
  in this test are simulator fixtures, not physical allocations.
- Verilator 5.054 with `UNITTEST` behavioural RAM. This is two-state simulation;
  it does not prove reset/X behaviour, clock-domain safety or metastability.

The generated simulation copy replaces two constant output destinations in
`TextureReader.v` with unused wires. No rendering arithmetic changes. Original
and modified hashes are recorded. Upstream sources remain unchanged.

This does **not** yet include Xanxi's board-specific command-credit changes,
the AXI4-to-AXI3/HP3 path, PS7 DDR arbitration, CPU caches, firmware validation,
mailbox AXI-Lite crossing, display scanout or GTA. Depth is not exercised. A
100 MHz testbench clock and synthetic cycle counts cannot predict board FPS.

## Reproduce

Use RasterIX commit `3e2ee5aa5214b829c2a824250c445ce799ac8dc0` and Float commit
`83485654e380392a726ee67f8e5d2f4f8f73dd32`, each with its `rtl` tree under a parent
directory. Requires Clang, Python 3, Verilator and its C++ build tools.

```sh
python3 scripts/test_dosbox_glide_full_renderer.py --upstream /path/to/sources \
  --ordered-state --output .context/full-renderer-ordered
python3 scripts/test_dosbox_glide_full_renderer.py --upstream /path/to/sources \
  --output .context/full-renderer-control
```

The expected fatal assertion in `unsafe-drain` is a successful mutation check.
The runner fails unless that specific failure is observed. The generated output
directory contains compiled tools and source copies; keep it outside tracked
source. [Ordered results](ordered/results.json), [control results](control/results.json)
and the [manifest](manifest.json) record source, harness and evidence hashes.
The [build log](ordered/build.log) retains upstream warnings; this is not a
warning-free RTL audit.

## Investigation log and next gate

1. Connected the full renderer to the existing scene encoder and completion
   components. Initial Icarus experiments produced incorrect pixels and a stall.
2. Traced an Icarus startup/X issue in crossbar combinational selection. Those
   observations were not sufficient evidence of a physical FIFO/renderer defect.
3. Used upstream's Verilator simulation approach. All five scenes passed with
   only the unused-output wiring adjustment. The unfenced-marker control also
   passed, so no marker-ordering fix or speed improvement is claimed here.
4. Added delayed responses, cancellation, errors, memory guards and a deliberately
   broken drain signal. Recorded passes and the expected mutation failures.

Next integrate the board-specific renderer and HP3 completion boundary, then
build and validate the matching firmware/FPGA interface. The first useful
hardware benchmark remains a bounded offscreen scene timed through verified
retirement. Earlier physical freezes remain unresolved. No Amiga software,
clock or firmware changed during this simulation work.
