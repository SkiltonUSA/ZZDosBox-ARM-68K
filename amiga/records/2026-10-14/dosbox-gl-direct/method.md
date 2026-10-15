# TinyGL direct-target experiment

The optional software-GL renderer now binds the validated XACP 640×480 RTG
back buffer directly when no scanline mask is active. It applies byte order
and row stride during `glDrawPixels`, avoiding the previous private colour
buffer write followed by a second full-frame copy. All other target geometries
and scanline masks retain the existing two-stage pixel path. Direct remains
the default renderer and its rebuilt executable has the same SHA256 as before.

The allocation and firmware contracts are unchanged: prepare the context
before requesting the bitmap grant; validate geometry/aperture, mapping and
both nonce endpoints; borrow the target only during rendering; restore the
private pointer before unmapping and acknowledging completion. Cancellation
is checked each row and unwinds only after unmapping. TinyGL never programs
scanout registers. The FPS overlay follows rendering. The private fallback
buffer remains allocated, so no reduction in the 10 MiB GL heap is claimed.

## Verification

- Native ASan/UBSan: eight source geometries, independent integer pixel-centre
  oracle, both RTG byte orders, tight and padded pitch, sentinels/padding,
  invalid dimensions/formats/stride, cancellation and close/reopen.
- ARM instruction model: Hercules/amber, S3 1024×768 indexed, EGA; relocation
  at two allocation bases, SIMD/FPSCR restoration, direct-write cancellation
  and firmware-denied SIMD. Additional checks cover window resizing with both
  direct/fallback counters, rejected nonce with zero renders, scanline
  transitions across three slots and cancellation on a skipped presentation.
- Physical A4000TX: Hercules and S3 fullscreen reported 11 and 20 direct
  frames respectively, with zero fallback frames. A small Workbench window
  and gradient scanlines each reported 11 fallback frames and zero direct.
  All completed with guest success, context/MMU/register/cache witnesses
  restored and owned memory released.

The model is not a performance measurement and the lifecycle checks do not
establish visual smoothness or tear-free scanout.

## Timedemo protocol

Use the supplied Doom 1.9 shareware Demo3, 8 MiB guest RAM, 10000 cycles/ms,
4 MiB JIT cache, NEON, pipelined display, Normal presentation, profiling on,
fullscreen, no scanlines/overlay, and verify DEFAULT.CFG absent. Compare the
2134-game-tic interval against the A9 global timer, not the VGA frame count or
the guest's emulated timer. Only read logs after the process completion marker.

Candidate is staged separately in RAM. The matched reference is the existing
installed Direct binary. Historical two-stage GL data is in the previous
day's record; comparisons to it must be labelled historical rather than a new
same-session pair. Results and installation status are recorded after the run.

## Results and held refinement

| Renderer | Game interval | Game FPS |
|---|---:|---:|
| Current Direct, this session | 307.435 s | 6.9413 |
| First direct-target TinyGL, this session | 421.420 s | 5.0638 |
| Previous two-stage TinyGL, historical | 451.684 s | 4.7245 |

The first direct-target build is about 7.18% faster than the historical GL run,
but still 27.05% slower than the current Direct run. These are single runs.
All 13,008 GL presentations used the direct target, with zero fallback frames.
Both this-session timedemos completed with clean teardown. Direct remains default.

A subsequent refinement separated the private/depth-aware loop from a dedicated
volatile external-store loop. It passed native and ARM-model checks, but its
first physical Hercules test lost bridge connectivity at about 10:35:52 UTC.
The owner confirmed a frozen Amiga and manually restarted it. No completion log
was retrieved, so the exact failing phase and cause are unknown. Do not infer
that the store loop itself caused the failure solely from the timing.

That refinement is withdrawn. Its binary SHA256 is
`51bb21250b830a8fd07ae631109128a996e131e31def38f5f5b8326f60a22987`;
its failed run transcript, native/model evidence and disassembly are retained.
`build-final.json` and `tinygl-final-provenance.json` describe that withdrawn
candidate, despite their chronological filenames. It never reached a Doom run.

Source was restored to the first tested variant and rebuilt to the exact
previous SHA256 `15e5d7fbcb224b0ae555f06b024208c66c8b0ee72932483d569c18ecaa72be3a`.
This candidate is also **not installed**. All testing used separately staged RAM
executables. The installed 0.11.0-dev launcher, Direct and two-stage OpenGL
runtimes were never overwritten. Settings still offers Direct / OpenGL
(experimental). A physical failure outweighs the model passes; no automatic
retry or promotion is scheduled.

Build/test commands:

```sh
python3 scripts/test_dosbox_opengl.py
python3 scripts/build_dosbox_dos_arm.py \
  --container-command '["podman","--connection","nuflix-converter-root"]' \
  --pc --pc-dynrec --pc-jit-mib 4 --pc-neon --pc-pipeline \
  --pc-scanlines --pc-presentation --pc-adapters --pc-opengl --pc-doom-clock
.context/amiga/mesen-bench/venv/bin/python tests/amiga/dosbox/check_opengl_arm.py \
  --out .context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-gl
```

After restart, the bridge reconnected; Workbench/Roadie/bridge ports were present
and no Core1 owner remained. Remote checksums verified unchanged installed
Direct (`D1EBF6F8`), OpenGL (`DF7E7D19`) and launcher (`A86FCE99`). No further
application test was launched. See `recovery-confirmed.log`.


## Offline follow-up: Developer controls

The next local build adds trace records and cooperative before/after-frame
checkpoints, plus a MUI Developer tab. No new hardware run or installation
was performed. The failed refinement remains withdrawn. Evidence is in
[the Developer record](../dosbox-developer/validation.json); implementation,
profile reanalysis and the SIMD paper review are documented in
[the reference notes](../../../../docs/amiga-tinygl-rasterisation.md).
