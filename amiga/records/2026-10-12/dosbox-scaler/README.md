# ZZDosBox ARM scaler specialisation

A4000TX / ZZ9000 XX19c, 2026-10-12. Keeps the 1 ms host wait, 640×480
output, 8,000 emulated ms and 558 generated frames of the previous build.

Changes: common 320-pixel input rows use paired stores; 640-pixel rows use
straight copies; pixel format is selected outside the inner loop. Other
widths retain the exact nearest-neighbour rule. Scalar integer code only;
no VFP/NEON state changes, cache-policy changes or altered framebuffer ownership.

- `row-native.log`: ASan/UBSan, all 1,024 widths and both formats, exact pixels
  and destination guards; exact-size source allocations catch read overruns.
- `row-arm.log`: production row helper compiled for Cortex-A9, all widths and
  both formats; source ends on an unmapped page boundary, output guards checked.
- `full-arm.log`: linked DOSBox EGA/PIT/direct-framebuffer test at two bases,
  both formats, plus rejected nonce and revoked temporary mapping.
- `build.json`, `before-build.json`: candidate and baseline provenance.
- `*-before.log`, `*-after.log`: physical comparisons, without screenshots.

COPY measures host-observed grant-to-completion, including mapping, scaling,
copy, unmapping and polling delay. It is not an isolated kernel benchmark.
The FPS figures count generated frames over startup, not steady gameplay.

See `docs/amiga-dosbox-feasibility.md` for results and interpretation.

Fresh physical results: Keen 29.07 -> 33.25 FPS; Vette 22.01 -> 24.20 FPS.
Observed copy phase approximately 8.4 -> 4.2 ms per frame. Both keep 558
frames, restore registers/cache witness and release owned memory. Game file
writes are zero. EGA/PIT and input-wait cancellation also pass.

Final executable: 695,244 bytes, SHA256
`68faf71c85135dc1e915141a82d821a522a7769598b82596d0c6077ec4ed4b4b`.
ARM ELF SHA256
`c800d2e1361dec8f82556810e6ff1b64d040e062d4ec665ae543021af4027af2`.
`promotion-events.json` records installation and preservation of the previous
binary. `fixture-1-default.log` checks the normal installed path.
