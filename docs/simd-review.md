# Simd NEON reference

Reviewed [ermig1979/Simd](https://github.com/ermig1979/Simd) at
`69a630c5569e13a3fed3533f80e323186c904a64`. No library code has been integrated
and no performance gain is claimed.

The project provides image conversion, resizing and filtering with an ARM32
NEON backend. It is a useful reference for future ZZ9000 pixel work, rather
than a replacement for the x86 core or PIC scheduler.
[Project overview](https://github.com/ermig1979/Simd/blob/69a630c5569e13a3fed3533f80e323186c904a64/README.md).

- **Conversions:** its NEON BGR/RGB implementation demonstrates vector loads,
  channel rearrangement and stores, with aligned/unaligned paths. Our runtime
  already has NEON RGB565 conversion, 32-bit copy, byte swapping, 1×/2× scaling
  and dimming; a replacement must beat those kernels on the physical A9.
  [Conversion source](https://github.com/ermig1979/Simd/blob/69a630c5569e13a3fed3533f80e323186c904a64/src/Simd/SimdNeonBgrToRgb.cpp).
- **Scaling:** the NEON dispatcher chooses the scalar Base resizer for nearest
  neighbour. It offers NEON paths for certain bilinear/area cases. That gives
  no automatic speedup for our existing nearest-neighbour display path.
  [Resizer dispatcher](https://github.com/ermig1979/Simd/blob/69a630c5569e13a3fed3533f80e323186c904a64/src/Simd/SimdNeonResizer.cpp).
- **Porting:** ARM32 CPU detection uses operating-system capability information;
  our Core1 payload instead needs its existing XACP-aware SIMD guard. The CMake
  ARM settings also enable FP16 flags, so selected code must be built explicitly
  for Cortex-A9 and only the instructions available under its firmware policy.
  Whole-library Linux build settings cannot be reused unchanged.
  [CPU detection](https://github.com/ermig1979/Simd/blob/69a630c5569e13a3fed3533f80e323186c904a64/src/Simd/SimdNeonCpu.cpp),
  [ARM build settings](https://github.com/ermig1979/Simd/blob/69a630c5569e13a3fed3533f80e323186c904a64/prj/cmake/arm.cmake).

Recommended experiment: compare one needed pixel conversion against the current
kernel, including row alignment, short widths, tails and both framebuffer byte
orders. Keep scalar fallback, bounded owned memory and SIMD/FPSCR restoration.
Measure isolated kernel time and then complete game FPS; do not infer end-to-end
speed from a SIMD label. Bilinear filtering would be a separate visual option.
Any reused source must retain its copyright and licence notices.
