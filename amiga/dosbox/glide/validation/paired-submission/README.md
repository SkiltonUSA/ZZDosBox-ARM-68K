# Paired draw submissions — physical comparison

Pairing reduces completion waits from **65 to 33 per frame**, improving measured
offscreen throughput without removing framebuffer commits. At 800 MHz ARM,
three alternating 100-frame runs per path produced:

| Resolution | Baseline FPS | Paired FPS | Throughput gain |
| --- | ---: | ---: | ---: |
| 320×240 | 54.396 | **56.876** | **4.56%** |
| 640×480 | 45.516 | **47.239** | **3.79%** |

These are arithmetic means from the same firmware and scene settings. A longer
640×480 check, 1,000 frames per path, gave **45.506 → 47.235 FPS (+3.80%)**.
The timed intervals were roughly 22 and 21 seconds; this is not an endurance
test. Pre-run temperatures ranged from 54.6 to 56.5°C, not continuous maxima.
All runs passed their pixel checks, completion count and clean GPU close.

[Raw logs and structured results](hardware/result.json). The
[earlier baseline runs](../workload/README.md) remain recorded separately.
This is synthetic offscreen throughput, **not GTA gameplay or display FPS**.

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
firmware build, package records and physical measurements.

The firmware writer completed both writes: the proven workload firmware is
preserved as `GRXWORK.bin`, and the paired candidate was staged as `BOOT.bin`.
Uploads passed MD5 checks; no firmware-card readback is claimed. The bridge
responded afterwards and its reservation was released. The subsequent read-only
status query confirmed the running firmware's five-profile capability mask
(`0x1f`) and idle GPU before tests began.

## Physical comparison

One-frame checks passed for the tiny control and all four larger profiles.
The 100-frame comparison alternated profiles 1/3 and 2/4; the middle round
reversed each pair's order. A final 1,000-frame run of profiles 2 and 4 confirmed
the VGA gain. Profiles 3/4 are now preferred for this experiment; 1/2 remain
available for regression comparisons. The released DOSBox player is unchanged.

The baseline 1,000-frame run exited successfully, but transferring its full
progress log exceeded the bridge script timeout. The final result was recovered
with a filtered log read; status confirmed DONE, error zero and no retained GPU
claim before another test ran. Subsequent retrieval transfers only summary
lines. The partial output and recovery evidence are included in the raw logs.

`ZZGlide-Measure --status` queries the private service without submitting GPU
work. It reports capability bits, state, error, claim and selected profile.

These are offscreen synthetic tests. GTA guest execution, texture uploads,
depth/LFB support and presentation are still separate integration work.
