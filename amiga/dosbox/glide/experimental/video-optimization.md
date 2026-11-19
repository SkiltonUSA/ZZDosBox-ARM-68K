# Three follow-up video tests

These experiments build on the [runtime clock audit](video-modes.md). They use
our routed rebuild, not the released firmware binary. No bitstream is produced
or flashed, and no physical ARM clock is changed. Timing results are not emulator
FPS results or complete timing sign-off.

## 1. Legal PLL ratio at the same pixel clock

The 2560 × 1440 preset's `41/2/14` multiplier/input-divider/output-divider ratio
produces a 146.429 MHz pixel clock with a 2,050 MHz VCO. The candidate `41/4/7`
preserves the exact rational output frequency and lowers the VCO to 1,025 MHz.

| Check | Original ratio | Candidate ratio |
| --- | ---: | ---: |
| Derived pixel-clock period | 6.829 ns | 6.829 ns |
| PDRC findings | 1: VCO-range warning | 0 |
| Setup WNS | −1.453 ns | −1.508 ns |
| Hold WHS | +0.010 ns | +0.010 ns |

The original failure reproduced, and restoring the startup PLL properties
reproduced the original baseline timing. Vivado exited 0 in 363.16 seconds.
The range correction succeeds; setup timing remains a failure and is 0.055 ns
worse in this model. The candidate is suitable for further investigation, not
promotion to the runtime table without reconfiguration and display validation.

[Test 1 evidence and reproduction script](../validation/video-optimization/pll-ratio-check/).

## 2. Physical optimisation at 150 MHz

Test 2 starts from the original routed checkpoint and verifies the recorded
150 MHz control. It uses `phys_opt_design -directive AggressiveExplore`, followed
by `route_design -directive Explore`, retaining the source RTL and existing timing
exceptions. AMD documents physical optimisation as a timing-driven operation on
placed or routed logic; it can change the netlist and placement even though the
source RTL is unchanged. See the [Vivado 2025.2 command reference](https://docs.amd.com/r/2025.2-English/ug835-vivado-tcl-commands/phys_opt_design).

The completed run exited 0 in 1,113.39 seconds. After both physical optimisation
and rerouting, setup WNS improved from **−1.621 ns to +0.021 ns**: a 1.642 ns
margin improvement. Hold stayed at +0.010 ns. This clears the checked 150 MHz
setup paths with only a small positive margin; it is not an FPS improvement.

The worst reported video path now runs from `vga_v_rez_reg[4]` to
`dvi_active_video_reg/CE`, with 6.284 ns data delay: 2.539 ns logic and 3.745 ns
routing. Nearby critical paths run from sprite RAM to `dvi_rgb` output registers.
These are concrete targets if more timing margin is needed; the original
sprite-position reset path is no longer the worst reported path.

Vivado reported 29 optimized cells/nets, including register placement, pin swaps
and LUT transformations. The final design has 54,919 routable nets, all routed,
and zero routing errors. LUT use increased from 25,762 to 25,766; registers remain
34,798, block RAM 80.5 tiles and DSPs 22. The full DRC retains the original 52
warnings and one advisory, with no errors.

Before saving the separate candidate checkpoint, the script restored the original
`33/4/11` startup PLL configuration. Its global setup/hold result returned to
+0.156/+0.010 ns. The original baseline checkpoint was not overwritten. No source
pipeline stages were added and no manual Copper or sprite timing change was made.

[Test 2 reports, candidate hash and reproduction script](../validation/video-optimization/video-physopt/).

## 3. Independent candidate regression

Test 3 independently reopens the resulting diagnostic checkpoint and checks
representative display configurations, including the corrected 146.429 MHz PLL
ratio. It checks route status, timing coverage, bus skew, CDC and methodology,
then restores the candidate's initial clock properties. Existing unconstrained
audio and I/O paths remain outside a positive setup/hold result.

| Configuration | Pixel clock | Original setup WNS | Candidate setup WNS |
| --- | ---: | ---: | ---: |
| Startup control | 75 MHz | +0.156 ns | +0.156 ns |
| 640 × 480 preset | 25 MHz | +0.156 ns | +0.156 ns |
| 800 × 600 preset | 40 MHz | +0.156 ns | +0.156 ns |
| 1280 × 1024 preset | 108 MHz | +0.156 ns | +0.156 ns |
| 1920 × 1080 preset | 150 MHz | −1.621 ns | +0.021 ns |
| 2560 × 1440, corrected PLL | 146.429 MHz | −1.508 ns | +0.135 ns |

The corrected-PLL reference is Test 1's result on the original routed design.
All six candidate cases have zero failing setup, hold and pulse-width endpoints
among the constrained paths, +0.010 ns worst hold slack, and zero PDRC findings.
These mode names identify the source clock presets; full display geometry and
physical monitor operation are not tested here.

The independent run exited 0 in 635.38 seconds and restored the initial clock and
timing result. All 54,919 routable nets remain routed with zero routing errors.
CDC summary counts and methodology rule counts match the original baseline;
570 critical methodology warnings and 112 warnings remain. Bus-skew reports
retain both MET and NA results, with no reported violation; NA is not a pass.

Every mode retains 566 clock pins without timing clocks, 1,016 unconstrained
internal endpoints, 76 inputs without input delays and 75 outputs without output
delays. No timing exceptions were added to achieve the positive margins.

[Test 3 reports, six-mode results and reproduction script](../validation/video-optimization/video-candidate-regression/).

## Reproduction and remaining work

Use the pinned source/toolchain in the [rebuild guide](../../../../docs/xacp-firmware-rebuild.md).
Place the archived Tcl scripts together in a working directory containing
`baseline-routed.dcp` with the SHA-256 recorded in each result manifest. Run
`pll-ratio-check.tcl`, then `video-physopt.tcl`, then
`video-candidate-regression.tcl` in Vivado 2025.2 batch mode. The second script
creates `video-physopt-candidate.dcp`; the third reopens it. Reports and scripts
are published with hashes; checkpoint binaries are not part of a firmware release.

These are steady-state configurations, not dynamic PLL-switching tests. The
161.538 MHz preset and its VCO-range issue remain outside the candidate regression.
Before promotion, resolve timing coverage and clock-domain findings, validate
integrated video behaviour and mode transitions, and test the resulting firmware
on hardware. Keep the existing source and deployed firmware as the reference.
