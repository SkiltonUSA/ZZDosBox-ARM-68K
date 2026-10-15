# ZZDosBox benchmark results

Recorded physical A4000TX / ZZ9000 Cortex-A9 results. Launcher screenshots use
0.19.8-dev; launcher version changes do not imply a new ARM benchmark run.

## Doom 1.9 shareware: Demo3

`DOOM.EXE -timedemo demo3`, unchanged game defaults, 2,134 game tics,
8 MiB guest RAM and 10,000 cycles. FPS is game tics divided by the measured
A9 timer interval. The DOS guest clock and display-overlay counter are not
used for this score.

| Recorded run / build | Game FPS | Evidence |
|---|---:|---|
| Original interpreter | 3.10 | [Run](../amiga/records/2026-10-13/dosbox-doom/result.json) |
| ARMv7 JIT, 1 MiB code cache | 6.94 | [Run](../amiga/records/2026-10-13/dosbox-dynrec/result.json) |
| JIT, 4 MiB cache, overlap and NEON pixels | 7.29 | [Runs](../amiga/records/2026-10-13/dosbox-tuning/results.json) |
| VFP scheduler, accepted Direct build | **7.64** | [Two-run mean](../amiga/records/2026-10-14/dosbox-pic-vfp/physical-results.json) |
| Latest repeated baseline check | **7.49** | [7.4799 / 7.5049 FPS](../amiga/records/2026-10-14/dosbox-optimisation-round2/render-results.json) |
| Latest pixel-capture candidate | 7.49 | Same record; no established improvement, not promoted |

The VFP comparison and later baseline check are separate measurement sessions.
The later accepted candidate scored 7.4887 FPS; its first run was rejected after
a display acknowledgement failure and is excluded. Normal presentation remains
the default. The separate Adaptive experiment reached 8.25 FPS by displaying
fewer frames; see [the presentation report](../amiga/records/2026-10-13/dosbox-presentation/results.json).

### Historical PC comparison

| System / configuration | Doom Demo3 FPS |
|---|---:|
| Intel 386DX-25, OPTi | 4.58 |
| AMD 386DX-33, ETEQ + 387-40 | 6.89 |
| AMD 386DX-33, ETEQ | 7.47 |
| **ZZDosBox, latest baseline check** | **7.49** |
| **ZZDosBox, accepted VFP comparison** | **7.64** |

PC results: [Thandor's Doom 1.9 shareware benchmark](https://thandor.net/benchmark/32),
reviewed 15 October 2026. These put ZZDosBox broadly in the **386DX-25/33 class**
for this workload, closest to the recorded **386DX-33** results. Graphics cards,
memory systems and timing methods differ; this comparison is workload-specific
and is not a general MHz rating or an official submission to the reference list.

## Ziff-Davis WinBench 3.11

Latest completed recorded run: **13 October 2026**. Graphics WinMark:
**1,923,752 pixels/sec**, running Windows for Workgroups 3.11 with standard
VGA 640×480, 16 colours, 8 MiB guest RAM and 3,000 cycles in a Workbench window.

| Subtest | Reported pixels/sec |
|---|---:|
| MS PATCOPY | 3,231,315 |
| ExtTextOut, 16 pt System | 1,933,861 |
| ExtTextOut, 21 pt New Tms Rmn | 1,447,575 |
| MS PATINVERT | 690,661 |
| Polygon alternate fill | 522,477 |
| Single horizontal | 1,695,090 |
| Single vertical | 1,085,509 |
| Single diagonal | 221,382 |
| MS SRCCOPY | 1,633,974 |
| Poly mixed | 505,642 |

[Recorded result](../amiga/records/2026-10-13/dosbox-wfw/benchmark-result.json) ·
[Hardware findings](../amiga/records/2026-10-13/dosbox-wfw/findings.md)

WinBench uses the emulated guest clock. Its displayed “37 MHz 80386” estimate
and built-in Compaq reference do not establish equivalent physical CPU speed;
the reference also uses a different graphics mode. The Doom comparison above
uses independently measured A9 elapsed time.

Later Windows work measured startup/disk caching, not a new WinMark run:
the 1 MiB cache reduced the matched startup workload from 77.479 to 66.410
seconds (14.29% less elapsed time). See [the cache results](amiga-dosbox-optimisation-round2.md).
No fresh WinBench or Doom run was performed for this documentation update.
