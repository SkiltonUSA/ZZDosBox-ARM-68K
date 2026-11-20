# GTA ECTS live-renderer candidate

The private GTA player now builds for the Cortex-A9 and Amiga. **Hardware GTA
FPS is not measured yet.** It requires the new streaming firmware; the released
DOSBox player remains unchanged.

The captured gameplay path uses texture decals with ARGB1555 cutouts, square
single-level textures, bilinear filtering and RGB555 linear-framebuffer writes.
It did not request a depth buffer. The candidate implements this observed
subset and stops on unsupported calls. This is not general Glide compatibility.

- 41,997 captured calls replay successfully: 27,300 triangles, 61 swaps,
  552 texture allocations and 2,082,048 texture bytes. All 5,100 translated
  packets pass the firmware validator. This replay does not rasterize or
  establish FPS; the supplied game and capture remain private.
- Full RasterIX simulation passes transparent/opaque cutout checks with nearest
  and bilinear filtering, preserving the background behind transparent texels.
- Core0 owns a bounded request channel. Core1 translates commands, uploads
  textures and requests readback. Each GPU submission must retire before memory
  is reused. Errors retain the GPU lease until recovery.
- Linear-framebuffer writes preserve untouched pixels. Presentation reads back
  RGB565, converts it on ARM and uses the existing Workbench/fullscreen path.
  This initial route includes copy and synchronization costs.
- The frame counter waits for a window paint or fullscreen PAN acknowledgement.
  It records completed swaps and raw timer intervals after ten warm-up swaps.
  A gameplay screenshot, stable timer calibration and clean teardown are still
  required before reporting gameplay FPS.

## Validation

`test_glide_gta.py` checks mapping boundaries, channel retirement, texture
encoding, framebuffer edits and terminal unsupported calls. Its optional
`--trace` replays a local GTA capture; `--firmware` checks every submitted packet
against the actual validator. AddressSanitizer and UndefinedBehaviorSanitizer
are enabled.

`test_glide_measure_firmware.py` executes the ARM service with modeled I/O:
streaming/benchmark exclusion, upload, readback, delayed completion, clean close,
nonce reuse and failed-lease retention. The existing 25 benchmark scenarios
also pass. `test_glide_gta_pixels.py` verifies the complete RTL renderer's cutout
results using synthetic pixels. These tests do not replace physical execution.

## Next hardware checks

1. Cold-boot the streaming candidate and query its identity and idle state.
2. Verify an empty DOS session opens and closes the channel cleanly.
3. Run GTA, inspect rendered gameplay and capture the display.
4. Measure matched gameplay intervals, retaining frame counts, timer calibration,
   CPU cycles, resolution, clock, audio mode and all build identities.
5. Verify Core1 restoration and GPU close after every run. Stop after any
   unverified cleanup; do not retry a possibly active GPU client.

The FPGA is unchanged from the tested carry-free texture-address build. Existing
board timing/CDC limitations still apply. The new Core0 service is experimental
and has not yet passed physical validation.

## First physical lifecycle and close correction

The initial streaming firmware booted at 800 MHz; global-timer calibration was
399,995,327 Hz. An empty DOS session completed its command and restored Core1
registers/MMU/cache state, but returned RC20 because GPU close failed. The
channel remained active with no requests processed, no work pending and no GPU
error. No GTA gameplay or additional GPU test followed.

The cause was the close precondition: `zc_idle()` required no GPU reservation,
including the very reservation close was meant to release. The correction
separates worker-idle from clock-transition eligibility. Close still checks
Core1, AY, Paula and Copper workers, the matching nonce, outstanding requests
and failed GPU work. Clock changes remain blocked while the GPU is reserved.

The earlier ARM test stub returned idle without considering GPU ownership.
The revised fixture executes the actual generated firmware idle predicates.
It reproduced the physical failure before the fix. Empty-session close, busy
workers, wrong nonce, delayed retirement, failed-lease retention and all 25
existing measurement scenarios now pass in the ARM model.

[Close-fix evidence](close-fix/) records the physical failure, corrected build
and tests. The corrected boot image has been staged, with upload checksum and
writer RC0 verified; boot-card readback has not been performed. The FPGA and isolated DOS player are unchanged. Physical validation
of the correction remains pending a cold restart; **there is still no GTA FPS
result**. The proven paired firmware remains available as `GRXPAIR.bin`.
