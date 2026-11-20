# Guest triangle batching

`draw_stream.c` connects decoded Glide 2 `grDrawTriangle` calls to ordered
RasterIX packets. It is a standalone integration component, not yet wired into
the DOSBox diagnostic port or a live Core0 transport. The overlay continues to
report no rendering board.

## Behaviour

- Read all three 60-byte vertices through checked DS-relative guest addresses.
- Preserve the selected global or TMU0 reciprocal W and clip window-space geometry.
- Emit the clipped polygon as an ordered triangle fan, splitting at packet capacity.
- Keep multiple calls in one packet until full, explicitly flushed or reconfigured.
- Validate new rendering state before flushing the previous state.
- Stop after an encoding or submission failure; never retry an ambiguous batch.

The transport callback receives borrowed packet bytes and must return success
only after retirement, or after copying them into an offline recorder. FIFO
acceptance alone is insufficient. It must use the tagged firmware completion
path before reusing the packet, texture or target. Failure leaves recovery with
the transport; opening a fresh stream is not permission to reuse a faulted GPU.

There is no texture allocator, guest state decoder, board advertisement,
`grBufferSwap`, framebuffer lock, depth support or presentation in this component.
`grx_draw_configure` accepts an already validated subset configuration with a
previously uploaded texture. Unknown Glide calls return `GRX_UNSUPPORTED`.

## Validation

```sh
python3 scripts/test_dosbox_glide_guest.py
# arm-none-eabi GCC with newlib headers and Unicorn are required for --arm:
python3 scripts/test_dosbox_glide_draw.py --arm --firmware /path/to/ZZ9000OS/src/main.c
```

The fixture sends 320 triangles per texture/origin/blend combination: 12
combinations, 3,840 input triangles. It compares packet bytes and ordering with
direct encoding. Other cases cover clipping across packets, TMU W, malformed
pointers, invalid state, empty geometry and transport rejection during rollover.
The unchanged validator extracted from the recorded firmware accepts the packets.
ASan/UBSan pass; a mutant that ignores submission failure is detected.

The same harness executes in a Cortex-A9 instruction model with GCC 13.2,
software floating point and VFP softfp. Both match the native packet digest.
Floating-point contraction is disabled on both sides for this comparison,
following the existing clip-model test. Leaving native contraction enabled
produced a different digest even though each platform's local reference passed.
This is not a production compiler change or an ARM performance measurement.

The initial stub failed the first stream-open assertion before implementation.
The full guest regression suite also passes, including no-board behaviour and
bounded argument decoding. [Recorded results](../validation/guest-draw/result.json).

Next: add a bounded transport for these packets, verify a larger offscreen
workload, then implement the game-required state/texture/depth and presentation
interfaces. The existing firmware benchmark accepts fixed scenes only.
