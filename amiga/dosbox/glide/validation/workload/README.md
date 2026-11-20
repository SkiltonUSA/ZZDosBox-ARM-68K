# Larger offscreen workload — staged, hardware timing pending

The ABI2 test adds 320×240 and 640×480 profiles with **320 textured triangles
per frame**. Geometry goes through the guest-vertex decoder and draw-stream
batcher. Each frame clears its target, then submits 64 five-triangle batches.
Every packet must retire before the next is submitted; only all 65 retirements
complete a frame. A final verifier checks red, blue and clear pixels in each
of the 320 cells (960 samples). The original five correctness scenes run first.

The timed interval includes Core0 guest-fixture preparation, clipping, encoding,
submission and completion polling. It excludes initial checks, final readback,
startup and shutdown. Every batch retains the encoder's colour commit; no
claim of optimal batching or representative game complexity is made.

| Check | Result |
| --- | --- |
| Native counter and pending-frame mutant | Pass, including 65 packets per frame |
| Generated packets through recorded firmware validator | 130 packets pass |
| Native pixel-verifier checks | Expected samples pass; corruption detected |
| ARM firmware service model | 15 scenarios across three profiles pass |
| ARM firmware and 68020 host builds | Pass |
| Bootgen payloads, preserved FSBL and FPGA | Match inputs |
| Firmware writer | Recovery and candidate writes completed; no card readback |
| Larger-workload physical FPS | Pending restart and testing |

Profile 0 retains the existing 64×72 benchmark. Profile 1 uses 320×240; profile 2
uses 640×480. For example, `ZZGlide-Measure --candidate 1 1` requests one larger
frame. Run only with the matching staged firmware, an idle GPU/Core1 and the
shared bridge reservation. ABI2 capability and selected-profile checks prevent
silently interpreting an old tiny-scene result as a larger workload. Any fault
stops the session and retains the GPU claim until recovery.

`GRXONE.bin` on the firmware card preserves the previous, tested completion
firmware. `GRXBASE.bin` remains the earlier recovery image. No released player or
launcher was changed. This does not implement the GTA renderer or game FPS.
