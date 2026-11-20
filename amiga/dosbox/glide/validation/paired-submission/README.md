# Paired draw submissions — awaiting hardware comparison

This experiment reduces completion waits from **65 to 33 per frame**. It does
not remove framebuffer commits or claim an FPS gain. The existing baseline is
[54.47 FPS at 320×240 and 45.57 FPS at 640×480](../workload/README.md).

The clear packet remains separate. Each following submission joins two
five-triangle packets into one render envelope, followed by one fence and
completion token. All 320 textured triangles, register writes and 65 colour
commits remain in their original order. Each pair is copied into private
storage before validation; malformed envelopes, privileged operations and
oversized submissions are rejected. The largest workload submission is 451
words including its completion suffix, within the 512-word FIFO.

Profiles 1 and 2 retain the original 65-submission workloads. Profiles 3 and 4
use pairs at the same respective resolutions. Both are in the same firmware
for matched testing. Five original correctness scenes still precede timing,
and the 960 final pixel samples and clean-close requirements are unchanged.

## Verification

- Native address/undefined-behaviour sanitizers: packet bounds, envelope checks,
  command order, retained commits and one completion suffix pass.
- 260 generated workload packets pass the unchanged firmware validator.
- ARM service model: 25 scenarios across all five profiles pass. Successful
  baseline/paired runs have identical render-word counts and payload hashes.
  Busy, mapping, FIFO and pixel failures preserve the existing failure behaviour.
- Full renderer simulation: paired subject/commit plus state/commit fixtures
  produce the expected pixels; delayed memory, cancellation and error checks
  pass. These are small correctness scenes, not simulated gameplay or FPS.
- The command-drain mutation requires the original unaligned DDR-tail fixture;
  the paired packet lengths do not expose that particular missing guard. The
  regression therefore retains the original fixture for that mutation, which
  is detected. Paired-stream rendering uses its own fixtures throughout.
- ARM firmware, 68020 test program and boot payload checks pass. The FPGA,
  bootloader and existing audio/clock services are preserved.

The evidence files contain the model results, renderer logs, source hashes,
firmware build and package records. No hardware result for pairing is claimed.

The firmware writer completed both writes: the proven workload firmware is
preserved as `GRXWORK.bin`, and the paired candidate is staged as `BOOT.bin`.
Uploads passed MD5 checks; no firmware-card readback is claimed. The bridge
responded afterwards and its reservation was released. A cold restart is pending.

## Physical comparison

After the staged firmware has been cold-booted, start with one frame of the
tiny control, then one frame of each larger profile. Continue only after
pixel checks, GPU completion, clean close and bridge responsiveness pass.
Alternate profiles 1/3 and 2/4 at 800 MHz for repeated 100-frame runs.
Retain the baseline if pairing does not improve measured throughput.

These are offscreen synthetic tests. GTA guest execution, texture uploads,
depth/LFB support and presentation are still separate integration work.
