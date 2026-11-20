# GTA FPS readiness

**Latest result: 4.50 FPS**, compared with 3.96 FPS in the reference profiled run
(+13.6%). Reusing a valid CPU framebuffer avoids redundant GPU readback.
[Implementation and comparison limits](../validation/gta-live/frame-cache/README.md).

**First accelerated GTA gameplay baseline: 3.68 FPS**, at 800 MHz ARM,
640×480 windowed, 3,000 DOS cycles and sound off. The street scene is captured,
and 481 completed-frame intervals took 130.81 seconds after warm-up. Capture
and transfer activity occurred during this sample. See the
[screenshot, raw logs and limitations](../validation/gta-live/README.md#first-measured-gameplay). The
GPU-close correction passes physical startup/exit checks; see the
[recorded cause and fix](../validation/gta-live/README.md#first-physical-lifecycle-and-close-correction).

## Completed

- The carry-free FPGA candidate meets its declared timing constraints and runs
  the offscreen completion tests. Existing CDC and unconstrained-path findings
  are not a full-board timing sign-off.
- Guarded MMIO, tagged retirement, pixel readback and clean close pass physical
  fixed-scene tests at 800 MHz.
- Paired submissions improve the synthetic VGA workload by approximately 3.8%.
  This is [offscreen throughput](../validation/paired-submission/README.md),
  not a GTA result.
- GTA's observed texture, draw, alpha-cutout and framebuffer path passes captured
  call replay and validator checks. The bounded Core1/Core0 channel passes ARM
  instruction-model tests. See the [live candidate](../validation/gta-live/README.md).

## Remaining

Repeat a controlled gameplay interval without transfers, compare rendered pixels
with a reference, and profile request batching, framebuffer edits/readback and
x86 execution. Startup, visible gameplay, completed-frame measurement and clean
close have now been demonstrated. Other Glide programs may need additional state,
texture, depth and framebuffer behaviour.

Compare the same game build, scene, resolution, cycle setting, audio mode and
ARM clock. Record player, overlay, firmware and FPGA identities. Count completed
game presentations after warm-up; VGA refreshes and repeated scans are not game
frames. Record timer calibration and clean teardown for every measured run.

Native runtime and RTL simulation speed cannot establish Cortex-A9 game FPS.
