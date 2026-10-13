# ZZDosBox: larger dynrec cache, NEON pixels and display overlap

13 October 2026. Extends the physically verified ARMv7 dynamic recompiler.
No firmware changes or extra Core1 worker; the 68k/ARM ownership model remains.

## Changes

- Optional 4 MiB translated-code cache, up from 1 MiB. Block metadata grows
  from 8192 to 32768 entries; the heap reserve grows from 4 to 8 MiB. The
  guest still has 8 MiB for the benchmark and the complete Exec allocation
  stays **32 MiB**. The JIT arena moves to owned offsets 0x1b00000–0x1f20000;
  linker and runtime page bounds prevent overlap with data/tables/stack.
- NEON converts RGB565 and copies 32-bit scanlines; 1×/2× scaling and output
  byte swapping use integer SIMD. Eight-bit palette lookup and generic
  arbitrary-ratio scaling retain their scalar paths. No speculative tail reads.
- SIMD availability is checked against firmware policy without changing
  CPACR/FPEXC. Save/restore all 32 D registers and FPSCR around the DOS session,
  including guest errors and cancellation. Host verifies the execution witness
  and restored context. Only the pixel translation unit uses softfp/NEON flags;
  the remaining worker stays soft-float.
- The existing bounded two-grant display pipeline is enabled by default in
  the candidate. This allows writing the next spare buffer while a previous
  PAN acknowledgement is pending. Guest execution already resumes after each
  completed copy in the serial build; the new overlap concerns buffer writes.
  Front/pending buffers remain excluded and OS bitmap locks stay owned until
  work completes. Input gets a release point after at most two grants.
  `--serial-video` remains available for comparison.

## Build and tests

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-pipeline --pc-neon --pc-doom-clock \
  --container-command '["podman","--connection","nuflix-converter-root"]'
.context/amiga/mesen-bench/venv/bin/python tests/amiga/dosbox/check_dynrec_arm.py \
  --out .context/amiga/dosbox-dynrec-4m-neon-pipe
.context/amiga/mesen-bench/venv/bin/python tests/amiga/dosbox/check_capture_arm.py --neon
.context/amiga/mesen-bench/venv/bin/python tests/amiga/dosbox/check_scale_arm.py --neon
```

Omit `--pc-neon` for the scalar comparison. Outputs are separate directories;
no build command deploys or starts hardware. Repeat builds produced identical
executable SHA256 values, retained in the build records.

Pixel-model tests cover all 65,536 RGB565 colours, source widths 1–1024,
unaligned inputs, 8/16/32-bit capture, four target widths, both output byte
orders and row guards. The larger JIT arena passed ASan/UBSan bounds tests;
MMU tests passed 16,384 allocation alignments. The display event model passed
98 latency schedules / 168,000 frames, ACK wrap, slot exclusivity and stop
lock retention. These are correctness tests, not physical FPS measurements.

Both linked ARM builds passed CPU/files/MZ, self-modification, paging, EGA,
copy-time cancellation, input/read cancellation, BIOS boot/HDD, mouse and
relocation checks. The NEON model also verifies seeded D0–D31/FPSCR restoration
and denied SIMD access. Unicorn's A9 model reads CPACR as zero through MRC
despite its register API being set; that one hardware-register read is explicitly
modelled. All subsequent policy checks and SIMD instructions execute normally.
This limitation is why the real hardware context check is required separately.

Physical EGA/PIT tests passed for both candidates. NEON hardware reported
`witness=42 context_equal=1`, CPACR 00f00000 / FPEXC 40000000, checked ARM/MMU
registers restored, cache witness intact and owned allocation released.

## Benchmark method

Same supplied Doom 1.9 shareware Demo3, 2134 gametics, 8 MiB guest, VGA,
10000 cycles/ms, fresh defaults, fullscreen 640×480, no overlay or PCM audio.
No game input/screenshots during timing. The read-only signature-checked
observer uses the nominal 333333333 Hz A9 timer, with roughly one-frame
endpoint uncertainty. Guest-clock FPS is not the physical score.

`analyse.py` validates completed logs and creates `results.json`. Comparisons:
initial 1 MiB serial build; same build with overlap; 4 MiB scalar with overlap;
then 4 MiB NEON with overlap. Single-run small differences are not established
gains. Display wait/copy counters include observation latency and overlapping
activity; do not add them as exclusive CPU time.

The first overlap run completed at **6.96 FPS versus 6.94 FPS**, effectively
unchanged. It recorded 21 overlapping grants and clean teardown. The owner
reported no visible tearing during that run. That observation is specific to
this test and does not prove every game/mode is tear-free.

Game/OS media and guest RAM dumps are excluded from these records.

The 4 MiB scalar/overlap run completed at **6.97 FPS** (306.280 seconds),
versus 6.96 FPS with the smaller cache and overlap. This is not a meaningful
speed improvement in a single run. Code-publication commits fell from 12,365
in the initial run to 9,670, but total game throughput barely changed. It
recorded 18 overlapping grants, all 13,011 flips completed and clean teardown.
The 4 MiB metadata peak was 3,906,944 bytes within its 8 MiB heap reserve.

The NEON/4 MiB/overlap run completed at **7.29 FPS** (292.742 seconds),
about **5.0% above the initial 6.94 FPS dynrec run** and **4.6% above the
4 MiB scalar/overlap run**. This remains a single-run comparison, not an
across-game guarantee. The same demo and settings completed all 2134 gametics.
It recorded 104 overlapping grants, 13,008 completed flips, 9,644 code commits,
NEON witness 42 / restored context equal, clean MMU/cache checks and memory
release. The host-observed copy counter did not fall; its polling latency and
overlap make it unsuitable as an exclusive pixel-kernel timing measure.
Doom's eight-bit palette conversion remains scalar; its NEON path accelerates
32-bit output scaling/byte ordering. RGB565 conversion is implemented and
model-validated, but its separate physical throughput was not measured here.

| Build | Approximate real Doom FPS |
| --- | ---: |
| 1 MiB dynrec, scalar pixels, serial display | 6.94 |
| 1 MiB dynrec, scalar pixels, overlap | 6.96 |
| 4 MiB dynrec, scalar pixels, overlap | 6.97 |
| 4 MiB dynrec, NEON pixels, overlap | 7.29 |

## PC comparison and interactive follow-up

[Thandor's same Doom 1.9 shareware Demo3 benchmark](https://thandor.net/benchmark/32)
lists 386SX-25 at 3.40 FPS, two 386DX-33 configurations at 6.89 and 7.47 FPS,
and 386DX-40 at 8.24 FPS. Our 7.29 FPS falls within those DX-33 results and is
about 2.14 times the SX-25 score. This is a Doom workload comparison, not a
general CPU-equivalence claim; physical VGA/memory configurations differ.

The separate interactive check used 3000 cycles/ms, unlike all four benchmark
runs, which used 10000. Fullscreen-to-window succeeded at frame 2022. Injected
return-to-fullscreen and F10 did not take effect. The game continued rendering
to frame 18363, and the owner confirmed Workbench remained responsive. After
a bridge reconnect, a normal CLI Ctrl-C signal stopped this test: NEON context,
MMU/cache witness and memory release all passed. No hard reset was needed.
This does not yet establish the cause of the input problem or a working reverse
mode switch. A second interactive check with physical keys subsequently passed, as below.

## Final controls and installation

The owner confirmed both Alt+Return switches and F10 exit. The second log
records window mode at frame 3399, fullscreen at frame 4114, NEON context
restoration, zero MMU/register/cache-witness mismatches and owned-memory
release. The injected-input failure above was not reproduced with physical
keys; its cause remains unconfirmed.

Installed this exact tested executable as `SD032G:Dev/ZZDosBox/PC/ZZDosBox`,
using a verified staging copy and rename with rollback on failure. Preserved
the initial dynrec as `ZZDosBox-1M` and existing `ZZDosBox-Interpreter`. The
normal MUI launcher uses the new executable on its next launch. No game was
started after installation. Launcher.cfg's before/after MD5 matched
`297dddafcdd1fa80cf87d4aeaca3361d`; profiles and the separate Windows runtime
were not changed. Exact installation checksums are in `installation.json`.
