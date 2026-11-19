# Copper drawing feasibility: resolved-image spans

This offline experiment checks a possible input representation for RasterIX.
It does not execute Copper instructions, modify firmware or submit GPU work.
The input captures contain **final pixels**, not timed register writes or DMA
events. Guest timing must still be resolved by ZZUAE.

The planner converts a capture to RGB565, groups identical horizontal pixels,
and merges matching runs on consecutive rows into disjoint rectangles. An
independent replay checks every RGB565 pixel and rejects overlapping coverage.
The drawing estimate uses the existing opaque-clear packet layout: ten setup
words, then seven words per rectangle (scissor bounds, colour and clear).
It excludes completion commands, alignment, uploads and presentation. The
32,768-word budget is an offline drawing budget, not a hardware allocation or
permission to submit an oversized packet. A rejected plan exposes no partial
rectangle list.

## Results

All inputs are 640x256. The reference is their explicit RGB565 conversion;
this is not a claim of lossless RGB888 conversion.

| Existing capture | Horizontal runs | Merged rectangles | Drawing bytes | Fits 128 KiB drawing budget |
| --- | ---: | ---: | ---: | --- |
| Acid in the House menu | 12,473 | 8,622 | 241,456 | No |
| Banshee power-up briefing | 10,578 | 7,623 | 213,484 | No |
| State of the Art silhouette | 3,205 | 1,840 | 51,560 | Yes |

Reconstruction was exact for all three RGB565 images. Raw RGB565 pixels occupy
327,680 bytes each. These byte counts are **not transfer times, GPU timings,
FPS measurements or Copper execution savings**. The Banshee image is a briefing,
not active gameplay; the silhouette does not establish coverage of the earlier
dense-Copper slowdown. There is no fresh Amiga run in this experiment.

**Decision:** do not implement whole-frame solid-rectangle rendering as a
universal replacement for ZZUAE's cached ARM renderer. The representation is
scene-dependent and exceeds this budget for two of three captures. Even the
smaller list may lose to cached CPU rendering; the existing synchronous/readback
experiments already showed that command preparation and synchronization matter.
The next useful drawing test should select genuinely large resolved regions,
retain the CPU fallback, and measure the complete presentation path. It must
not first render an entire CPU frame merely to rediscover these rectangles.
An actual event/span capture from the renderer is needed before live integration.

## Selective large-region follow-up

The follow-up partitions the image into GPU candidates and a CPU remainder.
It selects the largest disjoint rectangles first, with deterministic ties,
a minimum pixel area and the same drawing budget. A zero or insufficient
budget leaves the entire image on the CPU. Independent reconstruction checks
both partitions together, so omitted or overlapping pixels fail the test.

At a **256-pixel minimum area**:

| Capture | GPU candidate rectangles | Pixel coverage | Drawing bytes |
| --- | ---: | ---: | ---: |
| Acid menu | 57 | 37.34% | 1,636 |
| Banshee briefing | 123 | 76.83% | 3,484 |
| State of the Art silhouette | 209 | 63.20% | 5,892 |

All three fit the offline budget and reconstruct exactly with the CPU remainder.
The evidence also includes thresholds of 64 and 1,024 pixels. These are
exploratory thresholds, not tuned defaults. Coverage includes static backgrounds;
it does not measure Copper workload removed, elapsed time or FPS. The CPU
remainder rectangles describe coverage, not a proposed CPU drawing algorithm.

**Decision:** retain selective regions as an experiment. The smaller command
lists justify measuring event-derived regions, but do not justify enabling GPU
rendering. Live work still needs cache ownership, completion and presentation
costs measured against the current cached ARM renderer. Discovering regions by
scanning an already rendered frame would add work rather than remove it.

Eight tests pass, including 80 randomized selective partitions, threshold and
budget boundaries, deterministic selection and full fallback. Mutations that
omit the CPU remainder, ignore the budget or prefer the smallest regions all
fail. See `selective-results.json`, `selective-tests.json` and
`selective-environment.json` in the evidence directory; the earlier full-frame
records remain unchanged.

## Reproduce

Python 3 is required. Image analysis additionally uses Pillow (tested with the
version recorded in `validation/copper-spans/environment.json`); it is not an
Amiga runtime dependency.

```sh
python3 tests/amiga/dosbox/glide/test_copper_spans.py
python3 scripts/analyze_uae_copper_spans.py \
  amiga/dosbox/glide/validation/copper-spans/acid-menu.png \
  amiga/dosbox/glide/validation/copper-spans/banshee-briefing.png \
  amiga/dosbox/glide/validation/copper-spans/state-of-art-silhouette.png
```

The five tests cover full coverage, odd dimensions, palette changes, alternating
pixels, 80 deterministic random frames, exact budget boundaries and invalid
input. Three deliberate mutations (last-row loss, colour-insensitive merging,
incorrect exact-capacity rejection) were detected. Input hashes and results are
in [the evidence directory](../validation/copper-spans/).

Work sequence: inspected previous ZZUAE tests; identified the failed readback
path; wrote and ran failing planner tests; implemented grouping and bounded
fallback; passed tests and mutation controls; replayed the three captures.
No emulator defaults, clocks, firmware or hardware settings were changed.
