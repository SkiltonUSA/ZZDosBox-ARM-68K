# Marker isolation follow-up

The earlier completion failure is now reproducible. **A pre-submit marker read
changes the result; cache maintenance alone does not prevent the failure.** This
narrows the investigation, but does not establish its hardware cause or justify
promoting the marker to a general rendering fence.

## Controlled comparisons

Every run uses the same five scenes and checks the firmware packet CRC, pixel
oracle, buffer guards and texture contents. Each series stops on its first
failure. A failed first read remains a failure even when the delayed read passes.

| Preparation before submit | Marker | Runs | Result |
| --- | --- | --- | --- |
| Cache maintenance + marker read | Reused colours, memset | 4, interleaved | All pass |
| Neither | Reused colours, memset | 4, interleaved | Fourth run fails: 35 solid pixels wrong |
| Neither | Fresh colours, memset | 1 | Fails: 153 alpha pixels wrong |
| Neither | Triangle marker; fresh initial clear marker | 1 | Fails: 946 additive pixels wrong |
| Cache maintenance + marker read | Fresh colours, memset | 16 | All 80 scene checks pass |
| Cache maintenance only | Fresh colours, memset | 3 | Third run fails: 3 alpha pixels wrong |
| Marker read only | Fresh colours, memset | 8 | All 40 scene checks pass |

All four failing runs had intact guards and correct delayed pixel readback.
No GPU timeout or bridge failure occurred. The A4000TX returned to Workbench
after each run; the existing 800 MHz setting remained unchanged.

The first eight runs alternate preparation on/off in one executable. The fresh
memset failure and the sixteen fully prepared runs also share one executable,
as do the cache-only and read-only comparisons. Build records confirm the hashes.
The triangle-marker test uses the 72-row allocation; the other comparisons use
65 rows. Therefore the triangle versus memset comparison is not geometry-matched.

Representative failures:

- [No preparation, reused marker](cold-7.log).
- [No preparation, fresh marker](fresh-0.log).
- [No preparation, triangle marker](triangle-cold-0.log).
- [Cache maintenance only](preclear-2.log).

Pass series: `warm-fresh-0.log` through `warm-fresh-15.log`, and
`read-only-0.log` through `read-only-7.log`. See [results.json](results.json)
for every recorded scene and [manifest.json](manifest.json) for file hashes.

## What this rules out—and what it does not

- Reusing a previous run's marker colour is **not a sufficient explanation**:
  fresh values still fail, with matching firmware CRCs for the retagged packets.
- Changing from a scissored memset to the diagnostic triangle marker is **not
  sufficient**: that path also fails when the preparatory read is absent.
- The extra pre-submit `CacheClearE` call alone is **not sufficient**.
- Reading the old marker before submission correlates with all passing series
  in this comparison. That read also checks the expected new value is absent.

The read could affect host-memory visibility, bus sequencing or timing. These
tests do not distinguish those causes; there is no equal-duration delay control
or independent Core1 readback witness yet. We have not proven a firmware or FPGA
defect. Inspection of the 68k disassembly confirms that the polling loop loads
the mapped marker words and compares them against the expected value; it is not
just polling a C variable cached in a register.

**Keep both cache maintenance and the pre-submit read as the probe default.**
The negative modes exist for diagnosis, not as faster production settings.
No player, installer, firmware or release was changed. Guest Glide integration
remains gated on reliable completion and the separate depth-support limitation.

## Reproducing the comparison

```sh
python3 scripts/build_dosbox_glide_probe.py --height 65
```

On an idle, exclusively reserved target, with a 64 KiB stack:

```text
ZZGlide-Probe --marker-base 0xA200
ZZGlide-Probe --no-marker-preread --marker-base 0xA210
ZZGlide-Probe --marker-preclear-only --marker-base 0xA220
ZZGlide-Probe --marker-preread-only --marker-base 0xA230
```

Use a new base for each run. Valid bases are 1 through 65531; five consecutive
16-bit values are used. `--triangle-marker` needs a default 72-row build and only
uses the custom base for its initial clear; subsequent triangle-marker colours
are fixture-specific. Fresh-colour conclusions above concern the memset path.

`prepare=0/1/2/3` means neither/cache-only/read-only/both. `before=10000` is a
sentinel meaning **not sampled**, not a framebuffer value. All modes still perform
the original cache maintenance inside marker polling and before pixel verification.

Only the final clear-colour payload is retagged. CRC32 is recomputed on the exact
packet bytes and checked against the firmware result. Native tests cover the
standard CRC32 check vector, payload-only retagging and rejection of a triangle
tail. The original generated fixtures retain their independent Python CRC check.

`split-build.json` and `read-only-build.json` describe the current source in this
commit. Earlier build records describe intermediate probe variants; their complete
source snapshots were not retained. None of the logs should be presented as a
game benchmark.

## Next isolation step

Compare the 68k observation with an independent, correctly mapped Core1 witness
of the same GPU work. That can distinguish a host-side observation issue from
pixels becoming visible after the marker in DDR. Keep tests offscreen and stop
on any unknown completion state. Do not paper over this with a fixed sleep.
