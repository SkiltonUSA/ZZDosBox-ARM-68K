# Duke Nukem 3D performance tuning

The Descent configuration approach also helps Duke Nukem 3D 1.3D shareware on
the A4000TX / ZZ9000. S3/VESA, 10,000 cycles and adaptive presentation increased
measured game rendering from **1.17 to 4.53 FPS (+288.6%, 3.89×)**. Sound and
music remained enabled. This is a substantial improvement, but still below
smooth gameplay; the tuned guest clock advanced at about 85% of real time.

## Physical results

Each row is one 30-second stationary opening-level sample after five wall
seconds of settling. Hollywood Holocaust, Piece of Cake, 320×200 game resolution,
unchanged default detail, Workbench window, 486 CPU model, 8 MiB guest RAM,
EMS, SB16 and OPL3. No input or screenshots during measurement.

| Configuration | Completed game frames | Game FPS | Guest time / wall time |
| --- | ---: | ---: | ---: |
| VGA, 6,000 cycles, normal presentation | 35 | 1.17 | 57.4% |
| S3/VESA, 6,000 cycles, normal presentation | 49 | 1.63 | 74.9% |
| S3/VESA, 10,000 cycles, adaptive presentation | 136 | 4.53 | 84.8% |

S3 alone improved rendering by **40.0%**. The remaining gain combines higher
cycles and adaptive presentation; their separate contributions were not isolated.
These single samples cover the opening rooftop scene, including its initial
scripted animation. They are not whole-game averages, a fixed identical-frame
replay or an Intel CPU-equivalence rating.

## What changed

- **Graphics-event batching:** the VGA path captured 320×400, while S3 captured
  320×200 for the same game resolution. Exclusive PIC-event time (graphics, timers and audio) fell from
  23.4% to 8.0% of the measured interval in the S3-only comparison.
- **Adaptive presentation and higher cycles:** skipping intermediate display
  refreshes reduced host handoff/copy work. In the combined configuration, CPU
  execution occupied 74.8% of the interval, compared with 40.7% in the baseline.
  The final adaptive presentation divisor was four; that is not a 4-FPS limit.
- **Existing acceleration retained:** the same ARM JIT, 4 MiB code cache,
  NEON pixel routines and VFP scheduler were used throughout. There is no new
  firmware, recompiler optimisation or reduced game detail in this result.

## How game FPS was measured

Duke's built-in FPS text uses its emulated 120 Hz clock. It cannot establish
real elapsed game FPS when emulation runs slower than real time. The isolated
`--pc-duke-clock` build instead observes the game's completed-render history
index and divides the accumulated count by A9 wall time (333,333,333 Hz).

The original [Duke source, GAME.C](https://github.com/videogamepreservation/dukenukem3d/blob/master/source/GAME.C)
shows `displayrest()` calling `tics()` when DNRATE is enabled. The released 1.5
source uses a 16-entry history; disassembly of the tested 1.3D executable confirms
64 entries. The observer recognizes relocated code and matching counter operands,
excludes the unrelocated EXE buffer, and unwraps the index each guest millisecond.
At the measured rates, the index cannot wrap 64 renders between observations.

All three benchmark configurations use the same **private instrumented game copy**:
a two-byte conditional branch becomes two NOPs to enable the existing FPS
counter automatically. This is equivalent to leaving DNRATE enabled and adds
its overlay overhead to every row. It is measurement instrumentation, not a
performance patch. The original installed game and its configuration are unchanged;
the instrumented game binary is not distributed. The runtime observer only reads
guest memory. It automatically requests normal shutdown after sampling.

## Validation and installation

Native tests cover relocation, ring wraparound, sample duration, invalid counter
values, altered code, ambiguous signatures and memory bounds. Linked ARM tests
at two allocation bases pass JIT lookahead, profile publication, framebuffer and
SIMD-state checks. The host suite and Amiga/ARM protocol suites pass.

All three physical runs completed AHI submission/playback accounting with no
reported error, restored registers/cache witnesses and released owned memory.
This is not a listening test or a full-game compatibility guarantee.

The saved Duke profile now selects S3/VESA, 10,000 fixed cycles and Adaptive.
Its 486 model, RAM, sound, window mode and launch command remain unchanged.
Other program profiles are unchanged. The existing launcher/player are retained;
this research does not create a new runtime release.

The normal, unmodified player launched the original game and reached its attract
demo. Interactive keyboard/exit verification remains incomplete: a subsequent
remote title-bar click stalled bridge input pending physical mouse release. This
occurred after the three diagnostic runs had finished and is not evidence of a
Duke runtime crash or clean normal-player exit.

[Raw logs, hashes and test evidence](../amiga/records/2026-10-18/duke-performance/).
