# NEON row-dispatch experiment

Moving invariant format/scaling choices outside the NEON pixel loop did **not**
improve measured Doom Demo3 speed. The installed PIC/VFP runtime is retained.
The experiment remains opt-in with `--pc-neon-dispatch`; it is not a new default.
Empire is outside the active optimisation test set.

| Direct / Normal, 10000 cycles | Run 1 | Run 2 | Mean FPS |
| --- | ---: | ---: | ---: |
| Installed VFP baseline | 7.63547 | 7.62858 | 7.63203 |
| NEON row dispatch | 7.63037 | 7.63209 | 7.63123 |

The mean difference is **−0.0104%**, effectively unchanged in these four runs.
Order was baseline/candidate/candidate/baseline. All used the supplied Doom 1.9
shareware Demo3, 8 MiB guest RAM, Direct fullscreen, Normal presentation,
profiling/trace/overlay/scanlines off. FPS covers 2134 game tics measured with
the A9 global timer; it is not the VGA refresh rate. No frames were skipped.
All completed with checked SIMD/register/cache restoration and memory release.

## Why test this loop?

A separate instrumented run of the installed VFP build assigned 54.331% to CPU
execution, 19.137% to events, 5.443% to capture, 5.595% to grants, 9.547% to
copying and 5.946% to other work. The copy bucket includes mapping and rendering;
it is not pure kernel time. Instrumentation changes performance, so its FPS is
not compared directly with the uninstrumented runs above.

The original scaler checks byte order and 1×/2× output on every four-pixel
iteration. The candidate chooses one of four specialised loops per row. It
preserves the vector footprint, exact byte values and scalar tails. Dimmed
scanline processing and arbitrary-ratio fallback are unchanged. No Simd
library code was imported.

## Cached-RAM kernel measurement

A separate diagnostic build runs the old and new functions in the same checked
SIMD context. Each case is warmed and measured in ABBA order, 2048 rows per
function, using private cached source/destination arrays. These timings are
**not framebuffer bandwidth**.

| Pixels | Format | Baseline µs/row | Candidate µs/row | Throughput change |
| --- | --- | ---: | ---: | ---: |
| 320 → 320 | Native words | 1.292 | 1.167 | +10.67% |
| 320 → 640 | Native words | 3.042 | 2.672 | +13.87% |
| 640 → 640 | Native words | 2.552 | 2.307 | +10.60% |
| 640 → 1280 | Native words | 6.042 | 5.322 | +13.53% |
| 320 → 320 | Byte-swapped | 1.173 | 1.188 | −1.26% |
| 320 → 640 | Byte-swapped | 2.801 | 2.789 | +0.43% |
| 640 → 640 | Byte-swapped | 2.313 | 2.328 | −0.65% |
| 640 → 1280 | Byte-swapped | 5.561 | 5.559 | +0.03% |

Faster cached-RAM arithmetic does not establish faster mapped framebuffer
writes. The end-to-end result shows no useful gain in this workload. The
profile continues to point mainly at CPU execution and event handling.

## Correctness and evidence

ARM instruction-model checks cover all widths 1–1024, both byte orders, 1×/2×
scaling, alignment variants, row-end bounds and guard bytes. Full scaler/style
checks include arbitrary ratios and dimming. Linked-image checks matched
baseline scheduler decisions, guest output/time and exact pixels, and tested
resize, cancellation and denied SIMD access. Physical EGA/PIT and the kernel
diagnostic passed before Doom. These correctness tests are separate from
physical speed measurements.

[Raw measurements and analysis](../amiga/records/2026-10-14/dosbox-pixel-dispatch/results.json)
retain the individual results. `analyse.py` reproduces the game FPS and
cached-kernel calculations from the closed logs.
