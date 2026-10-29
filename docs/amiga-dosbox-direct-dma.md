# ZZDosBox direct DMA investigation

Status: **program builder tested; read-only hardware survey passed. No DMA
transfers submitted and no game-performance gain claimed.** The normal player
and its ARM/NEON rendering path are unchanged.

## Implemented

- Standalone `ZZDosBox-DMA-Probe`, using the existing runtime mapping check,
  Exec-owned allocation, single-Core1 port and reset-before-free lifecycle.
  It reads the Zynq DMA reset/security configuration, identification, manager,
  interrupt, fault and eight channel-status registers. No DMAC/SLCR writes.
- A bounded PL330 program builder for separate source/destination spans, up to
  4 MiB, with 8-byte-aligned starts, 128-byte bursts and finite nested loops.
  It supports raw bytes and swapping bytes within each 32-bit pixel. Short
  tails use byte transfers, or whole 32-bit transfers for swapped pixels.
  DMAWMB precedes DMAEND. The generated program is at most 48 bytes.
- An independent subset interpreter checks 328 copy/conversion cases, source
  preservation, destination guards, loop boundaries, exact tails and rejection
  of overlapping, unaligned, empty, oversized or out-of-span requests.
  Address/undefined-behaviour sanitizers pass; the builder also compiles as
  freestanding Cortex-A9 C. Model tests do not prove physical DMA or coherency.

The builder only creates bytes in ordinary memory. It has no submit function,
does not reserve a channel and is not connected to the player. Caller-supplied
spans describe an allocation; the builder cannot establish actual ownership.

## A4000TX survey — 28 October 2026

| Reading | Result |
| --- | --- |
| Cortex-A9 Core1 identity | MIDR `413fc090`, MPIDR `80000001` |
| DMA reset | Deasserted |
| Manager security | Secure, register interface `f8003000` |
| Identification | Peripheral `30 13 24 00`; PrimeCell `0d f0 05 b1` |
| Configuration CR0 / CRD | `001e3071` / `07ff7f73` |
| Manager / debug state | Stopped / idle |
| Channels 0–7 | All stopped at the sample |
| Faults / pending events / interrupts enabled | Zero |
| Exit | RET1 received, launcher exit 0, owned memory released |
| DMA submissions | **0** |

This establishes register access and sampled state, **not an exclusive channel
reservation**. It does not establish DMA throughput. The full local run log is
kept with the build; a compact receipt is in
[`amiga/records/2026-10-28/dosbox/dma-survey.json`](../amiga/records/2026-10-28/dosbox/dma-survey.json).

## Which DMA?

The display's AXI VDMA feeds scanout and remains in use. It is not a spare copy
engine. The firmware's `REG_ZZ_BLITTER_DMA_OP` dispatches ARM drawing functions;
its name does not make it an asynchronous PS DMA copy service.

The separate **Zynq PS PL330** is the candidate investigated here. The reviewed
XACP 1.7 SDK does not expose an application DMA acquire/submit/cancel/release
interface. A stopped-channel snapshot cannot replace that agreement with the
firmware. This work concerns local ZZ9000 DDR; it does not establish bus-master
access to TF4060 RAM, Amiga Chip RAM, disk controllers or Roadshow buffers.

## Where it could help ZZDosBox

| Path | Assessment |
| --- | --- |
| Same-size 32-bit frame, format 3 | Candidate for a contiguous copy when pitch and ownership permit |
| Same-size 32-bit frame, format 4 | Candidate for PL330 32-bit byte swapping; no extra CPU conversion buffer required in principle |
| Scaled output / software scanlines | Current NEON path also scales/dims; this copy builder cannot replace that work |
| Audio and file queues | Investigate large local DDR copies only after measuring; device/68k addressability is separate |
| Guest Sound Blaster DMA / guest memory semantics | Remain emulated; physical DMA is not a substitute for their timing and callbacks |

Rendering directly into the granted bitmap remains preferable to adding a full
staging copy solely to make DMA usable. Hardware byte swapping does not provide
palette lookup, RGB565 expansion or arbitrary scaling. Pitched/multirow programs
are not yet implemented.

## Remaining integration work

1. Establish a documented exclusive channel/manager-command contract with the
   installed firmware. Prefer a firmware-mediated lease; confirm how it is
   revoked on application exit or Core1 reset. Do not reset the whole controller
   or change TrustZone, clocks or global interrupt configuration to claim it.
2. Benchmark within proven application-owned DDR: guard bytes, full comparison,
   source/program cache clean, destination handoff and completion visibility.
   The current private inner-WB/outer-NC mapping needs explicit L1 maintenance;
   a barrier alone is insufficient. Shared PL310 global maintenance is forbidden.
3. Implement bounded completion and channel-specific cancellation. **Resetting
   Core1 does not stop an independent DMA engine.** A timeout must retain the
   allocation and bitmap lock until the engine is demonstrably quiescent.
   The current CPU-worker reset-before-free path cannot be reused unchanged.
4. Compare DMA against existing NEON at 4 KiB, 64 KiB, 320×200×4 and 640×480×4,
   including setup, cache maintenance and wait time. Test useful ARM work during
   transfer and resulting contention, not merely enqueue time.
5. Only then connect eligible transfers to `pc_flow`: copying remains set until
   DMA completion; no buffer flip, unmap, reuse or HUD writes before completion.
   Preserve CPU fallback before submission; never race fallback against an
   uncertain in-flight DMA. Repeat game FPS/audio/exit tests with a fresh baseline.

No performance percentage from earlier runs is reused as evidence for this work.

## Reproduce

```sh
python3 scripts/test_dosbox_dma.py
python3 scripts/build_dosbox_dma_probe.py
```

The probe is a developer diagnostic, not a normal launcher option. On the
verified XX19c target, with other ARM applications closed, run the generated
`ZZDosBox-DMA-Probe` from a Shell with `Stack 65536`. It does not change display
preferences or the installed player. Its bootstrap briefly allocates/locks a
P96 bitmap to prove the runtime mapping.

## References

- [AMD UG585 DMA programming guide](https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/Programming-Guide-for-DMA-Controller)
- [AMD DMA manager and security interface](https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/DMA-Manager)
- [AMD channel control and endian swap](https://docs.amd.com/r/en-US/ug585-zynq-7000-SoC-TRM/Register-XDmaPs_CC_n_OFFSET_5-Details)
- [Xilinx DMA driver](https://github.com/Xilinx/embeddedsw/tree/master/XilinxProcessorIPLib/drivers/dmaps): instruction encodings and CCR layout checked against the BSP's v2.4 source and current upstream; no upstream controller-initialization routine invoked.
- [Xanxi XACP source and SDK](https://github.com/Xanxi-Amiga/XACP-ZZ9000/tree/824f13a7db0eb98b82b8ef4e1a891db9386227d2): XX19c Core1/display paths and published API review.
- [ZZ9000 firmware reference](https://github.com/BlitterStudio/zz9000-firmware/tree/1790ad0f3d5a5d146f757f7a8e8806e44fcc506f): BSP addresses and `dma_rtg.c` dispatch. This separate reference is not a claim that its newer firmware is installed.
