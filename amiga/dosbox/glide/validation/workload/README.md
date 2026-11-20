# Larger offscreen workload — physical FPS results

At **800 MHz ARM**, three 100-frame runs per resolution produced:

| Resolution | Run A | Run B | Run C | Mean FPS |
| --- | ---: | ---: | ---: | ---: |
| 320×240 | 54.459 | 54.473 | 54.477 | **54.47** |
| 640×480 | 45.569 | 45.564 | 45.587 | **45.57** |

All runs passed pixel verification and clean GPU release. One-frame checks at
both resolutions and the original tiny-scene control also passed. The Amiga
remained responsive. Pre-run temperatures ranged from 54.3 to 55.3°C; these
are not continuous thermal measurements. Each 100-frame interval is roughly
1.84 or 2.19 seconds, so this is not an endurance test.

[Raw logs and structured results](hardware/result.json).

This is a synthetic grid using one 16×16 RGB565 stripe texture. It has no depth
testing, overlapping game geometry, game logic, display presentation or texture
streaming in the timed interval. It demonstrates this rendering path's throughput;
it does not predict GTA FPS or establish a gain over the previous firmware.

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
| Larger-workload physical FPS | Pass: six 100-frame runs plus one-frame checks |

Profile 0 retains the existing 64×72 benchmark. Profile 1 uses 320×240; profile 2
uses 640×480. For example, `ZZGlide-Measure --candidate 1 1` requests one larger
frame. Run only with the matching experimental firmware, an idle GPU/Core1 and the
shared bridge reservation. ABI2 capability and selected-profile checks prevent
silently interpreting an old tiny-scene result as a larger workload. Any fault
stops the session and retains the GPU claim until recovery.

`GRXONE.bin` on the firmware card preserves the previous, tested completion
firmware. `GRXBASE.bin` remains the earlier recovery image. No released player or
launcher was changed. This does not implement the GTA renderer or game FPS.
