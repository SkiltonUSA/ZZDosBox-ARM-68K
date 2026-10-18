# Descent performance tuning

Descent shareware 1.4 was measured on the A4000TX/ZZ9000 using its actual
completed-3D-frame counter. S3/VESA emulation, adaptive presentation and 10,000
cycles substantially improve the starting-room result without reducing detail.
The normal 32-instruction ARM JIT block limit is retained.

## Controlled maximum-detail comparison

Thirty-second stationary samples in level 1, Trainee difficulty, default
cockpit, maximum detail, sound off, DOS/32A 7.35, 16 MiB guest RAM and EMS on.
The game executable, scene and detail settings are identical between rows.

| Configuration | Rendered FPS | Guest time / wall time |
| --- | ---: | ---: |
| VGA, 6,000 cycles, normal presentation, window | 3.10 | 72.0% |
| VGA, 6,000 cycles, normal presentation, fullscreen | 3.13 | 73.3% |
| S3/VESA, 6,000 cycles, normal presentation, fullscreen | 3.57 | 83.8% |
| S3/VESA, 10,000 cycles, adaptive presentation, fullscreen | 7.10 | 97.1% |
| Same settings, experimental 64-instruction JIT blocks | 7.17 | 97.7% |

The accepted tuning improves the fullscreen maximum-detail sample by **126.6%**.
Window versus fullscreen alone differed by only about 1% in the baseline.
The larger JIT blocks changed the result by under 1%; that is insufficient
benefit to promote a broader recompiler change. These are individual controlled
samples, not a whole-game average or a CPU-equivalence rating. Audio-on results
are not established by these sound-off measurements.

## Existing custom-detail profile

The original installed Descent profile already used adaptive presentation and
custom detail `5,0,0,0,0,0,0`. This needs a separate comparison; the 126.6% figure
above is not presented as its before/after improvement.

| Configuration, fullscreen and adaptive | Rendered FPS | Guest time / wall time |
| --- | ---: | ---: |
| Existing VGA and 6,000 cycles | 4.47 | 87.1% |
| S3/VESA and 10,000 cycles | 8.63 | 97.8% |

This is **93.3% higher rendered FPS** at the existing detail level. Only the
original Descent profile’s adapter and saved cycles were changed; its fullscreen,
adaptive presentation, sound and game-detail choices were retained.

## Where the improvement comes from

- **S3/VESA graphics emulation:** this core's plain VGA path captures Descent
  as 320×400; S3 captures 320×200 with vertical scaling. Its batched VGA drawing
  reduces event dispatch and duplicate scanline work. At 6,000 cycles, measured
  gameplay event time fell from 23.2% to 1.6%. This is the emulated PC graphics
  adapter, not a new ZZ9000 GPU driver or a game modification.
- **Adaptive presentation:** reduces intermediate VGA presentations while
  continuing guest execution. Descent renders far fewer 3D frames than its VGA
  refresh rate. Reducing display handoffs frees time for x86 execution; the
  final adaptive divisor was two in the tuned maximum-detail sample.
- **10,000 guest cycles:** uses the recovered execution headroom. Measuring guest
  milliseconds alongside wall time prevents interpreting slow-motion emulation
  as a successful FPS improvement. The tuned maximum-detail sample advanced
  29.128 guest seconds in 30.000 wall seconds.
- **JIT block experiment:** 32 versus 64 instructions retained the same 4 MiB
  code cache and paging guard. Correctness tests passed, but the observed gain
  was too small to warrant changing the normal runtime.

No new NEON kernel, TinyGL transformation, x87 optimisation, firmware or game
patch is responsible for these results. They use the existing accelerated core
and graphics paths more effectively.

## Measurement and validation

The [original Descent renderer](https://github.com/videogamepreservation/descent/blob/master/MAIN/RENDER.C)
increments `FrameCount` after `render_mine()` and `g3_end_frame()`. Its debug FPS
display is absent in release builds. Native guest-RAM snapshots and x86
disassembly identified the corresponding function in the supplied executable.
Both installed EXE filenames have the same MD5: `d1de6a993724d7b2ab4939a66f15f1e5`.

The isolated `--pc-descent-clock` build recognizes the render function's code,
reads its relocated counter operand and never writes guest RAM or registers.
It settles for five wall seconds, samples for thirty, then requests normal
shutdown. Timing uses the existing A9 global-timer convention of 333,333,333 Hz.
The profiler samples exclusive gameplay CPU, event, capture, grant and copy
costs across that same interval. The on-screen VGA-refresh FPS counter is not
used as the game's FPS measurement.

Native tests cover recognition, relocation, counter resets, altered code,
ambiguous matches and memory bounds. Linked ARM tests cover self-modifying JIT
lookahead, EGA/PIT output, two allocation addresses across the test set, profile
publication and SIMD restoration. Accepted physical logs require intact JIT and
SIMD reports, zero register/cache-witness mismatches and released owned memory.

An intermediate expanded-profile build overlapped its timing fields with the
JIT diagnostic report. Its fullscreen/12,000-cycle attempts are excluded. The
corrected report uses exclusive space at `PC_VIDEO + 0xe00`; ARM tests caught
and then verified the correction. The original 48-byte windowed baseline was
unaffected. Diagnostic builds do not replace the normal player.

## Launcher

Launcher **0.22.4-dev** adds 10,000 to the per-program CPU-cycle choices while
retaining the previous 1,000/3,000/6,000 values. A global `dosbox.conf` cycles
override still takes precedence. The live ZZ9000 temperature remains beside
the version. Game data and private pilot/configuration files are not included
in this repository.

## Evidence

[Accepted hardware logs, build hashes and test records](../amiga/records/2026-10-18/descent-performance/)
include the original-detail and maximum-detail comparisons. The standalone
launcher build passes warnings-as-errors and profile/session round-trip tests.
The installed launcher and library were checksum-verified, with rollback copies
retained. Runtime executables, other game profiles and game files were unchanged.

The installed production player was launched from the updated MUI library,
reached the first mine and exited with F10. Its log confirmed S3/10,000 cycles,
intact JIT/SIMD diagnostics, restored registers/cache and released owned memory.
The launcher returned normally.
