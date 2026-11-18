# Offscreen RasterIX checks — 18 November 2026

**The GPU rendered the five fixtures correctly. Completion ordering remains
experimental.** Three final runs passed packet CRC, pixel and guard checks;
earlier runs sometimes observed the marker before correct pixel readback.
The revised probe has not established the cause of those earlier failures.
No guest Glide game, throughput improvement or FPS gain is claimed.

**Follow-up:** the failure is now reproducible without the pre-submit marker
read. Cache maintenance alone is insufficient. See the
[controlled comparison](../marker-isolation/README.md); the original results below
remain unchanged.

## Final checks

| Scene | Operation | Result |
| --- | --- | --- |
| 0 | Clear to black | 4,096 checked pixels correct |
| 1 | Solid red triangle | 3,611 checked pixels correct |
| 2 | Additive green over red | Yellow; 3,611 checked pixels correct |
| 3 | RGB565 red/blue texture × white vertex colour | 3,464 checked pixels correct |
| 4 | Red, alpha 128, over the textured triangle | Red/purple; 3,464 checked pixels correct |

Every final scene preserved the framebuffer guards, texture guards and uploaded
texture page. RGB565 alpha rounding is checked within one quantisation step.
Triangle edges and the texture split are deliberately excluded from the pixel
oracle; these results do not prove Glide's shared-edge or filtering rules.

| Solid | Additive | Texture | Alpha |
| --- | --- | --- | --- |
| ![Solid red](scene-1.png) | ![Yellow additive](scene-2.png) | ![Red blue texture](scene-3.png) | ![Red purple blend](scene-4.png) |

These are actual 64×64 readbacks from `final-triangle.log`, converted losslessly
from PPM to PNG. They are small offscreen test scenes, not game screenshots.

Final executable: 29,976 bytes, SHA-256
`66939c70329fb3e72829fdcf8812a8f72b17e9eaa4a570414035d40803fa2fa5`.
[Build inputs](build.json), [evidence hashes](manifest.json).

- [Triangle marker and capture](final-triangle.log): all five pass.
- [Memset marker](final-memset.log): all five pass.
- [Triangle marker, repeated without captures](final-triangle-repeat.log): all five pass.

## Method

The Amiga's 68k stages a big-endian packet and calls XACP `0x0603`. Core0
validates and sends it to the FPGA. The probe checks the returned word count
and firmware CRC against CRCs independently calculated with Python's `zlib`.
It then polls the framebuffer marker and checks pixels immediately, without
logging or a fixed sleep between submission, polling and first readback.

Hardware query returned `RIX1`, version `0x00020000`, capabilities `0x0b` and
submit ABI `0x00010000`. The existing ARM setting remained 800 MHz. No firmware,
clock, display mode, installed player or release asset changed.

The final buffer is 64×72 RGB565, with eight bottom GL rows reserved for
diagnostic markers. Its address and resolution stay fixed throughout each run.
The published GPU sandbox is used, separately from command staging. Framebuffer
and texture each have 128-byte guards at both ends. The 68k performs explicit
`CacheClearE` maintenance around shared accesses. The probe claims the existing
cooperative ARM-use port before touching the GPU and refuses an occupied target.
This is not a system-wide lock against unrelated GPU applications.

Two completion candidates are retained:

1. **Scissored memset:** a 64-pixel marker at GL row zero, preserving the draw's
   texture-enable and blend-factor state. The ABI requires the full-frame count
   even though scissoring restricts the writes.
2. **Triangle marker:** a second triangle in the reserved area, using the same
   draw state and no intervening feature, blend or scissor changes. Its expected
   colour changes between stages. This is fixture-specific, not a general fence
   for arbitrary texture, blend or channel-mask state. Scene zero still uses the
   clear/memset marker.

The test checks that the next expected marker is absent before submission.
The final probe strips COMMIT from its test packets; the standalone encoder's
`grx_end()` retains its original COMMIT behavior. Neither COMMIT nor XACP DONE
is treated as GPU retirement.

Recorded marker waits vary from hundreds of microseconds to around 16 ms.
They include host polling, scheduling and the test sequence. They are not GPU
triangle timings and must not be converted into an advertised game FPS figure.

## What failed, and what changed

The initial assumption was that a trailing scissored memset proved all preceding
triangle pixels were visible to the 68k. That assumption did not survive repeats.

| Iteration | Observation |
| --- | --- |
| [Initial full run](full.log) | Solid/additive passed; resetting texture enable in the marker tail left 799 texture pixels wrong. |
| [Preserve texture enable](preserve-texture.log) | Texture passed; alpha still had 153 wrong pixels. |
| [Preserve blend factors too](preserve-pipeline.log) | All five passed once. This did not establish reliability. |
| [Repeat](repeat.log) | Solid draw had 35 wrong pixels after marker observation. |
| [Remove COMMIT](no-commit.log) | Alpha had 153 wrong pixels initially, then zero after a two-tick delayed reread. Run still failed. |
| [Larger buffer, triangle marker](triangle-marker.log) | All five passed, including repeated runs. |
| [Matched marker comparisons](matched-0.log) | Four alternating triangle/memset runs passed: [1](matched-1.log), [2](matched-2.log), [3](matched-3.log). |
| [Original 65-row buffer](memset-65-no-log.log) | All five passed with the revised probe. Geometry alone is not an established explanation. |
| [Logging before polling](logging-65.log) | Also passed. Moving logging alone is not an established explanation. |

The two-tick delay is **diagnostic only**: a mismatching first read returns
failure even if the later read is correct. No production sleep was added.
The late success is evidence of an ordering/visibility problem, but does not
isolate FPGA pipeline ordering, shared-memory visibility or probe sequencing.
Preserving state is useful; calling it a complete fence fix would overclaim.

Earlier logs are retained as development evidence. They used earlier probe
variants, principally a 65-row framebuffer; their binaries and complete source
snapshots were not retained. Only the three `final-*` logs and PNG captures are
tied to the exact final source/binary hashes in `build.json`.

## Reproduce the isolated probe

```sh
python3 scripts/test_dosbox_glide.py --arm --firmware /path/to/XACP/main.c
python3 scripts/build_dosbox_glide_probe.py
```

After confirming the physical machine is idle and obtaining exclusive access,
copy `.context/amiga/glide/ZZGlide-Probe` to the Amiga. Use a 64 KiB stack:

```text
Stack 65536
ZZGlide-Probe --triangle-marker
ZZGlide-Probe
ZZGlide-Probe --triangle-marker --capture RAM:Glide-
```

`--clear` and `--solid` stop after the corresponding stage. `--log-before-read`
reintroduces diagnostic ACK logging before marker polling. A build with
`--height 65` reproduces the earlier geometry for memset tests; triangle-marker
mode requires the default 72 rows. The program writes only the published GPU
scratch area and requested capture files. A timeout or cancellation during GPU
work leaves completion unknown: stop testing and establish quiescence before
running another GPU client. Mid-submit cancellation was not physically tested.

The final runs returned to Workbench, released the cooperative port and passed
bridge liveness checks. The shared bridge reservation was then released.

## Remaining gates

- Isolate the earlier marker/readback failure with a controlled reproduction.
- Validate repeated batches, texture replacement, shared edges and cancellation.
- Add bounded guest Glide 2 OVL interception and marshalling.
- Resolve the public Core0 ABI's depth/stencil restriction before claiming
  general 3D game support.
- Test a real Glide workload and only then measure matched game performance.

The marker experiments draw on **Xanxi's librixgpu** and its stable-address
scissor marker, plus the existing ZZUAE Core0 clear probes. Triangle setup also
derives from Xanxi's 0BSD `rixdesc.c`; see the [component credits](../../README.md#reference-guide-and-credits).
