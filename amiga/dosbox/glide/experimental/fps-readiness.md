# GTA FPS readiness

**No accelerated GTA gameplay FPS has been measured yet.** The fixed-scene
hardware tests are successful, and the live GTA subset now builds. The next
gate is physical validation of the new streaming firmware and player.

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

Cold-boot the streaming firmware, verify channel lifecycle, render GTA gameplay
and inspect it, then time completed presentations. Other Glide programs may need
additional state, texture, depth and framebuffer behaviour.

Compare the same game build, scene, resolution, cycle setting, audio mode and
ARM clock. Record player, overlay, firmware and FPGA identities. Count completed
game presentations after warm-up; VGA refreshes and repeated scans are not game
frames. Record timer calibration and clean teardown for every measured run.

Native runtime and RTL simulation speed cannot establish Cortex-A9 game FPS.
