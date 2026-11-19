# Video crossings and sampled audio clock

This review uses unchanged Xanxi XX19d-RIX source at
`6e59edbaf24e0d8d2f29cdd31667a10e16c5b283` and the recorded routed baseline.
It adds diagnostics and a digital simulation; it changes no firmware or constraints.

## Routed crossing review

Vivado's detailed CDC report completed with exit 0. Its rule counts are:

| Rule | Count | Finding |
| --- | ---: | --- |
| CDC-1 | 62 | Unknown single-bit crossing |
| CDC-2 | 5 | Synchronizer without ASYNC_REG |
| CDC-3 | 25 | Synchronizer with ASYNC_REG |
| CDC-4 | 12 | Unknown multi-bit crossing |
| CDC-9 | 3 | Synchronized asynchronous reset |
| CDC-13 | 23 | Crossing on a non-FD primitive |
| CDC-15 | 36 | Clock-enable-controlled crossing |

These are detailed rule counts, not interchangeable with the earlier clock-pair
report's unsafe/unknown endpoint totals. A report warning alone does not prove
a visible hardware defect. Conversely, an existing false path does not prove
that a crossing is functionally safe.

The source and report agree on specific video crossings:

- `need_frame_sync` goes from the video clock into a single AXI-clock register.
- The 12-bit binary `need_line_fetch` counter crosses into the AXI clock. The
  following register shifts it by the vertical scaling value; this is not an
  atomic multi-bit transfer protocol. Independently synchronizing its bits would
  not establish coherency either.
- Screen dimensions, sync values and other display controls are sampled in the
  video domain. Sprite coordinates are latched at a frame boundary. Configuration
  stability and mode-switch ordering need separate review from active counters.

These are in `imports/April/video_formatter.v`, particularly the AXI-clock block
around line 145 and video-clock block around line 325, under the pinned FPGA
project's `sources_1` tree. The detailed report identifies the synthesized source
and destination registers. Preserve line-buffer ownership and frame/line ordering
when testing a replacement handshake.

## Runtime display clocks

The routed timing result uses a 75 MHz video clock. ARM `src/video.c` writes the
clock wizard's multiplier and dividers in `pixelclock_init_2()` at runtime. Examples
from its preset table, calculated from the 100 MHz input, are:

| Display | Multiplier / input divider / output divider | Calculated MHz |
| --- | --- | ---: |
| 640 × 480 | 15 / 1 / 60 | 25 |
| 800 × 600 | 14 / 1 / 35 | 40 |
| 1024 × 768 | 13 / 1 / 20 | 65 |
| 1280 × 720 | 15 / 1 / 20 | 75 |
| 1280 × 1024 | 54 / 5 / 10 | 108 |
| 1920 × 1080 | 15 / 1 / 10 | 150 |
| 1600 × 1200 | 21 / 1 / 13 | 161.538 |
| 2560 × 1440 | 41 / 2 / 14 | 146.429 |

These are programmed ratios, not measured frequencies or refresh rates. Some
table metadata differs from the divisor-derived frequency. The 75 MHz timing
pass does not establish timing closure for these other configurations. Future
analysis needs mode-specific clock relationships and uncertainty, while retaining
justified crossing exceptions; simply declaring a faster primary clock is not
a complete runtime-mode timing model.

## Audio-clock simulation

The upstream `audio_clock.v` samples its external input with a three-bit shift
register on FCLK0. Its final sampled bit is `mclk_out`; falling-edge detection
advances a counter whose top bit is `bclk_out` (divide by eight).

Nine digital tests passed: three input periods (80, 81.38 and 82 ns), each with
three starting phases, sampled at 100 MHz. Each test preserved 200 sampled rising
edges and produced the expected 25 divided rising edges. Sampled-edge delay was
20–29.86 ns. The non-integral periods produced sampled output periods of 80 or
90 ns. This quantisation follows from sampling on the 10 ns grid.

Two deliberately broken versions were rejected at simulation time: removing a
sampling stage and choosing the wrong divider bit. Compilation failure does not
count as mutation detection. The test explicitly initializes both registers to
zero; it does not test startup or reset behaviour.

Routed `clkbuf` bits all have `INIT=0`, but no ASYNC_REG property. Bits 0 and 1
occupy `SLICE_X109Y59`; bit 2 occupies `SLICE_X108Y46`. Vivado reports the external
input crossing as CDC-2, with a two-stage recognized synchronization structure.
These placement facts do not establish a metastability failure or its likelihood.

The simulation cannot model metastability, physical clock routing, setup/hold
failures or analog audio quality. The periods are test stimuli, not a measured
board clock. This is **not evidence that emulator crackling originates here**.
Do not assign an ideal uniform generated clock to the sampled output merely to
remove unconstrained endpoints: that would omit the demonstrated quantisation.

## Reproduction and next gate

Run from the repository root with Icarus Verilog 13.0 installed:

```sh
python3 scripts/test_xacp_audio_clock.py /path/to/upstream/audio_clock.v \
  --output audio-clock-simulation.json
```

The runner also accepts `--iverilog`, `--vvp` and `--ivl-dir` for local toolchains.
For the routed review, put `crossing-details.tcl` alongside the recorded
`baseline-routed.dcp` and run it with Vivado 2025.2. It only writes a report and
prints register properties. See the [evidence](../validation/crossing-audit/).

Next, validate runtime video modes and simulate the active frame/line handoff
before proposing crossing changes. Complete sampled-clock and external I/O
timing coverage remains a separate requirement. The known-working firmware stays
the reference; no FPGA image or performance improvement is claimed by this audit.
