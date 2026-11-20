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

## Deployment status

The candidate boot image and isolated player have been staged on the test Amiga.
Upload hashes and successful firmware-writer receipts were checked; this is not
boot-card readback verification. The proven paired firmware was preserved as
`GRXPAIR.bin`. The candidate requires a cold power cycle before testing. The
bridge was responsive and unreserved after staging; no live GTA GPU run has
been attempted on it yet.
