# ZZDosBox Classic scanline copy experiment

13 October 2026. Separate optional build; no canonical installation change.

## Question and scope

Can omitting framebuffer writes to black rows improve real game throughput?
This does not skip guest CPU work or internal VGA rendering. XANI's FPGA
scanline pipeline normally excludes 640x480 RTG without vertical doubling,
so this experiment implements a **software Classic mask** in our final ARM
copy. It neither changes firmware nor depends on the global Workbench
scanline setting. It is not Soft/Gradient mode, which needs nonblack pixels.

Three modes in one executable:

- `--scanline-copy off`: all output rows rendered normally.
- `--scanline-copy full`: even rows rendered, odd rows written black each frame.
- `--scanline-copy skip`: same image, but odd rows initialized once per buffer
  and then preserved. Expected steady-state row writes halve relative to full.

F9 cycles Off -> Full -> Skip -> Off in this experimental build only.
Alt+Return and F10 retain their normal actions. Window mode and FPS-overlay
mode use normal unmasked output; the experiment targets fullscreen 640x480.
The independent Workbench FPGA On/Off controls do not control this mask.

## Ownership and transitions

The bounded three-entry cache identifies framebuffer address, pitch, geometry,
format and an epoch. Changing mask mode or switching display mode invalidates
all entries; first use of each buffer initializes black rows. Disabling the
mask draws all rows of each newly presented frame. No reused front or pending
buffer is touched, and no P96 allocation is invented. Existing nonce checks,
temporary mappings, cancellation and synchronous reset-before-free remain.

The ownership challenge overwrites the last pixel, which is on a black row.
That pixel is explicitly repaired every masked frame, even when the rest of
its row is skipped. Model tests must poison initial black rows to catch missing
initialization; zero-filled model buffers would hide that defect.

Per-frame mode/epoch use reserved handoff words at PC_VIDEO+52/+56, gated by
DB_SCANLINE_TEST on both processors. Diagnostic counters at PC_VIDEO+0xd80
record processed frames, rendered rows, cleared rows, skipped rows and nonce
repairs. Counters are observations, not exclusive pixel-kernel time.

## Reproduction

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-pipeline --pc-neon --pc-doom-clock --pc-scanlines \
  --container-command '["podman","--connection","nuflix-converter-root"]'
.context/amiga/mesen-bench/venv/bin/python tests/amiga/dosbox/check_dynrec_arm.py \
  --out .context/amiga/dosbox-dynrec-4m-neon-pipe-scan
clang++ -std=c++17 -O2 -fsanitize=address,undefined -Iamiga/dosbox/pc \
  tests/amiga/dosbox/scanline_plan.cpp -o .context/amiga/dosbox-scanlines/plan
.context/amiga/dosbox-scanlines/plan
```

Compare the same Doom 1.9 shareware Demo3, fresh defaults, 8 MiB guest RAM,
10000 cycles/ms, fullscreen 640x480, no overlay/audio. One owned Core1 job at
a time. Do not inject input or capture screenshots during measured runs.
Use the signature-checked A9 timer observer and complete all 2134 gametics;
Doom's guest-clock FPS and our VGA-presentation counter are not real game FPS.
Run the interactive transition check separately at 3000 cycles/ms.

## Results

Physical A4000TX / ZZ9000, same settings and executable for all three runs:

| Output | Real seconds | Real Doom FPS | Change vs normal |
| --- | ---: | ---: | ---: |
| Normal | 291.206 | 7.3281 | — |
| Classic mask, rewrite black rows | 286.792 | 7.4409 | +1.54% |
| Classic mask, preserve black rows | 280.901 | 7.5970 | +3.67% |

Preserving rows was 2.10% faster than rewriting the same mask. These are
single runs per mode, not a repeatability study. Treat the small differences
as preliminary. The scope remains final display conversion/copy, not guest
rendering or FPGA scanout. No claim of a general emulator speedup or doubled
FPS follows from halving output-row writes.

The skip run cleared exactly 720 rows (240 for each of three buffers), then
skipped 3,121,920 black-row writes. Its 13,011 frames each repaired the final
nonce pixel. All timed runs completed all 2134 gametics with clean SIMD,
MMU/cache-witness restoration and owned-memory release. `analyse.py` validates
the raw logs and reproduces `results.json`.

Separate physical checks passed:

- EGA: 17 frames, three initialized buffers, correct counters and clean exit.
- Interactive Doom: F9 changed Full -> Skip -> Off. Readback captured all
  240 odd rows black in both masked modes and none black after disabling it.
  Captures are sequential live images, not identical game frames; see
  `screenshot-check.json`. The user was not watching; this is screenshot
  verification, not user confirmation or proof of tear-free monitor scanout.
- F10 cancelled cleanly; `physical-visual.log` records restoration and release.

Native ASan/UBSan slot-planning tests, ARM-model poisoned-buffer transitions,
both pixel byte orders, pixel guards, cancellation, JIT tests and SIMD context
restoration passed. See `model.log`, `pixels.log`, `transitions.log` and the
verification-source hashes. These supplement the physical results.

## Installation state

Experimental executable: `RAM:ZZScanBench` (lost on reboot), 1,076,456 bytes,
SHA256 `02442db69194befb55f8644c61badc01655bcc493d63adc11a3051ef75d89144`.
Build recipe and exact identity are in `build.json`. Normal runtime MD5 remains
`62a29c0ece11553121368b219c286741`, launcher settings remain
`297dddafcdd1fa80cf87d4aeaca3361d`; neither was replaced. Core1 was left idle
on Workbench, global FPGA scanlines Off. No firmware, game or OS media included.
