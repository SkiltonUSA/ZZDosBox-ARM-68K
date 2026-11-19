# Runtime display-clock timing experiment

This is a steady-state timing experiment on our rebuilt Xanxi XX19d-RIX routed
baseline. It is not a timing report for the released firmware binary, a physical
board test, or a GPU FPS measurement. No firmware or hardware clock was changed.

The test changes only the in-memory video PLL multiplier and dividers to match
eight representative presets in the pinned ARM `video.c`. Vivado derives the
new output clocks from those properties. Logic, routing, input-clock constraints
and timing exceptions remain fixed. Each derived period must match the requested
ratio, and the unchanged control must reproduce its original setup/hold timing.
The script restores the original PLL properties and checks timing again before
reporting success. The recorded run exited 0 in 893.13 seconds and restored the
original +0.156 ns setup / +0.010 ns hold result. It does not save a checkpoint
or bitstream.

## Results

| Configuration | Pixel clock MHz | Overall setup WNS ns | Hold WHS ns | PLL check |
| --- | ---: | ---: | ---: | --- |
| Routed control | 75 | +0.156 | +0.010 | No PDRC findings |
| 640 × 480 | 25 | +0.156 | +0.010 | No PDRC findings |
| 800 × 600 | 40 | +0.156 | +0.010 | No PDRC findings |
| 1024 × 768 | 65 | +0.156 | +0.010 | No PDRC findings |
| 1280 × 720 | 75 | +0.156 | +0.010 | No PDRC findings |
| 1280 × 1024 | 108 | +0.156 | +0.010 | No PDRC findings |
| 1920 × 1080 | 150 | **−1.621** | +0.010 | No PDRC findings |
| 1600 × 1200 | 161.538 | **−2.088** | +0.010 | VCO outside rated range |
| 2560 × 1440 | 146.429 | **−1.453** | +0.010 | VCO outside rated range |

Positive numbers cover the declared paths only. Existing gaps remain: 566 pins
without a timing clock, 1,016 unconstrained internal endpoints, 76 inputs without
input delay and 75 outputs without output delay. This is not full sign-off.
Other presets, custom timings and mode transitions are not covered by this sweep.
The table does not establish a universal 108 MHz limit. The three failing cases
have 62, 90 and 59 setup-failing endpoints respectively; none of the nine cases
reports a hold or pulse-width failure under the declared constraints.

The control uses multiplier/dividers `33/4/11`. The firmware's 1280 × 720 preset
uses `15/1/20`: the same output frequency, but different PLL operating parameters.
Both cases were checked rather than assuming they have identical uncertainty.

## Concrete timing target

At 150 MHz, a worst video path runs from `counter_y_reg[1]` through the sprite
comparison/reset logic into `sprite_py_reg[0]/R`. Its data delay is 7.778 ns:
1.644 ns in logic and 6.134 ns in routing. The same path has +0.912 ns slack at
108 MHz but −1.621 ns at 150 MHz. This provides a specific target for physical
optimization or carefully tested logic restructuring. It does not establish how
much improvement either approach will achieve.

Keep sprite coordinates, frame wrap and output-pipeline timing equivalent.
Adding a register without compensating the pixel timing could close a path while
moving or corrupting the sprite. A physically optimized candidate must also
retain the 100 MHz GPU/AXI timing result and pass functional rendering checks.

## Separate PLL configuration finding

Vivado reports PDRC-42 for two supplied ratios. With the declared 100 MHz input,
`21/1/13` uses a 2,100 MHz VCO and `41/2/14` uses a 2,050 MHz VCO. The report gives
an 800–1,600 MHz rated VCO range for this device. It labels these warnings because
the dynamic reconfiguration port is active and could program a legal value.
Here, however, these are the very ratios being evaluated from the runtime table.

Therefore a timing pass alone would not establish that either preset is usable.
Select and validate appropriate PLL ratios separately from improving video-path
timing. This experiment does not measure what a particular physical board does
with those presets, and it does not change the runtime table.

## Reproduction and next work

Run `video-mode-timing.tcl` from the [evidence directory](../validation/video-modes/)
beside the recorded `baseline-routed.dcp`, with Vivado 2025.2. The evidence includes
per-mode timing summaries, worst video paths, PDRC reports, a TSV and hashed result
manifest. Host names are removed from published reports.

The next candidate should target the identified sprite/reset path under a valid
high-clock configuration, then repeat the controls and mode sweep. Continue the
integrated producer/consumer tests and sampled-audio/I/O timing work in parallel
with that investigation. The [fetch-consumer tests](crossing-audit.md#fetch-consumer-follow-up)
support keeping the existing line transfer until a replacement demonstrates an
actual benefit and preserves its behaviour. Hardware completion and guest-renderer
integration remain separate requirements before accelerated GTA benchmarking.
