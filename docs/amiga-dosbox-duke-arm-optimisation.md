# Duke Nukem 3D: cycles, display and ARM recompiler experiments

Follow-up to the 1.17 → 4.53 FPS configuration result. These tests use the same
private 1.3D shareware counter instrumentation, Hollywood Holocaust/Piece of Cake,
320×200, default detail, S3/VESA, Adaptive presentation, 486, 8 MiB, EMS and SB16/OPL3.
Guest audio uses eight voices, 22 kHz, 16-bit stereo; host AHI uses 32 kHz stereo.
Each game-frame sample lasts thirty wall seconds after five seconds settling.
The opening contains scripted animation; this is not an identical-frame replay.

## Cycle and display comparison

Baseline runtime, before the guarded inline-read update.

| Cycles | Window FPS | Fullscreen FPS | Window game-clock speed | Fullscreen game-clock speed |
| ---: | ---: | ---: | ---: | ---: |
| 10,000 | 4.53 | 4.60 | 85.2% | 86.1% |
| 12,000 | 5.17 | 5.27 | 75.8% | 76.8% |
| 15,000 | 5.83 | 5.90 | 65.1% | 65.6% |
| 20,000 | 6.57 | 6.67 | 52.7% | 53.5% |


Higher fixed cycle counts increase rendered frames but reduce the speed of the
emulated timer. Guest-time/wall-time matters for gameplay and sound; more FPS
alone does not establish a better configuration. Fullscreen and window modes
use the same source resolution and 640×480 output. Fullscreen was only 1–2%
ahead in these single samples; that small difference is not a robust speed claim.

Every accepted run requires the JIT witness, SIMD-state equality, zero register
and cache-witness mismatches, completed audio delivery and released owned memory.
Completed buffers with no AHI error do not establish continuous or audible sound.
The separate audio build prints the existing late-link counter: the previous AHI
request had completed before its successor was submitted. It measures potential
stream discontinuities, not their duration or perceived quality. These counts
cover the whole session, including startup, rather than just the FPS interval.

## Where the time goes

CPU execution accounts for about 75–85% of the measured interval, while frame
copying accounts for about 2–3%. This limits the immediate value of further NEON
pixel work. The hot guest loops use indexed palette/texture gathers and carry
dependencies; turning those into NEON vectors would require a proven guest-loop
translation, not simply substituting a host framebuffer-copy function. Existing NEON conversion, 4 MiB JIT cache and VFP scheduling remain.
The upstream DOSBox/libretro ARM backend (credited to M-HT in its source)
already avoids identical register moves and replaces eligible
unused-flag arithmetic helpers with ARM instructions. Those are existing upstream
techniques, not new gains from this experiment.

The scoped diagnostic counted 34,029,535 translated-block entries across the
sample, with no table-capacity drops. Counts rank **frequency, not time**. Its
instrumentation adds overhead, so its FPS is excluded from the comparison.

- The busiest sampled block resembles Build's four-column texture loop. Other
  frequent blocks match masked sprite/vertical-column drawing: fixed-point
  stepping, indexed texture and palette reads, transparent-pixel tests and stores.
- Five frequent blocks match the clipping and mixing sequence in Duke's
  `MV_Mix16BitStereo`, each entered roughly 660,000 times. Guest software audio
  mixing is part of the emulation workload, separate from the Amiga AHI worker.
- These identifications compare runtime x86 bytes with the released
  [Build assembly](https://github.com/videogamepreservation/dukenukem3d/blob/master/SRC/A.ASM)
  and [audio mixer](https://github.com/videogamepreservation/dukenukem3d/blob/master/audiolib/SOURCE/MV_MIX.ASM).
  Source and tested binary versions differ, so symbol assignments are inferred.

Flags need particular care here: the sprite loop deliberately combines carry
from ADD with zero from DEC before JA, and ADC uses carry for texture stepping.
Removing those flag dependencies would corrupt rendering. Existing upstream
unused-flag elimination remains enabled; no speculative cross-block flag removal
is introduced. A wider persistent-register allocator would also have to preserve
exception, device-handler and self-modifying-code boundaries.

The first diagnostic accidentally included startup. It was rejected, the reset
was corrected and tested, and only the rerun with `scoped=1` is used here.

## Checked-read register ABI experiment

The experimental `--pc-jit-value` build changes the ARMv7 read-helper interface:
return the value in r0 and the exception flag in r1. A direct TLB hit no longer
stores into `core_dynrec.readdata` only for generated code to reload it. The
emitted exception check uses r1; the destination receives r0 directly. This is
an improvement to generated ARM code and register use, not a rewrite in assembly.

Device handlers, unlinked/high TLB banks, permission failures and cross-page
reads retain the checked upstream fallback. Writes and self-modification tracking
are unchanged. It does not bypass guest memory protection or alter the game.

The value-register ABI measured 4.53 FPS at 10k and 6.67 FPS at 20k, compared
with the audio-instrumented baseline's 4.53 and 6.60 FPS. The 10k result is
unchanged; the small 20k difference does not justify promotion. Both passed
physical teardown, but no production runtime is replaced with this experiment.

The separate 8k diagnostic reached 3.77 FPS and 97.6% emulated-time progress.
Its whole-session late-link counter was 510, compared with 1,236 at 10k and 1,264
at 20k. All submitted frames completed with AHI error=0. Different session
lengths mean raw gap counts are not directly comparable; normalised to 1024-frame
requests, approximately 19%, 56% and 100% of links were late, respectively.
No capture of analogue/HDMI audio was available; audible quality is not certified.

### Original-game controls

The installed game initially ignored movement/fire while Escape still worked.
Its minimal CFG had no `KeyDefinitions`. Duke's
[configuration reader](https://github.com/videogamepreservation/dukenukem3d/blob/master/source/CONFIG.C)
clears control assignments and then reads that section. After backing up the CFG,
keyboard bindings were added without changing display/sound settings or the game
executable. Forward movement, turning and fire then worked: the view changed and
pistol ammunition decreased from 48 to 37. F10 restored Workbench cleanly, with
SIMD/register/cache witnesses intact and owned memory released. This was a game
configuration fix, separate from the recompiler experiments. Fresh shareware
installs should run the supplied SETUP.EXE to configure controls and sound.


## Register reuse and inline-read candidates

`--pc-jit-reuse` replaces an immediately adjacent ARM store/load of the same
emulated CPU register with MOV (or no instruction when registers already match).
It applies only to unconditional word accesses through the resident CPU-register
base and scratch r0–r3. Block entries and branch-patch boundaries reset the
opportunity. The optimisation runs while translating, not on each guest load.

`--pc-jit-inline` emits a direct ARM load only for an aligned, currently linked
low-bank TLB read entry. Other reads retain the existing checked helper: high
banks, device/unlinked entries and unaligned wide accesses. Writes are unchanged,
including self-modification tracking. The fast path preserves the address and
cached address registers needed by the slow path and caller.

| Build | 10k game FPS | 20k game FPS | Decision |
| --- | ---: | ---: | --- |
| Baseline with audio diagnostics | 4.53 | 6.60 | Reference |
| Read value returned in ARM registers | 4.53 | 6.67 | No meaningful gain; research only |
| Adjacent CPU-register store/load forwarding | 4.50 | 6.60 | No gain; research only |
| Guarded inline ARM reads | 4.67 | 6.77 | Retained in Expanded 0.22.5-dev |

Repeating the 10k pair gave 4.53333 and 4.66662 FPS again: **+2.94%**.
Emulated-time progress improved from 84.76% to 87.32%. The initial inline sample
was 4.66665 FPS / 87.49%. These are modest, repeatable improvements in this
opening-level workload, not an emulator-wide performance guarantee.

The first register-forwarding overlay missed the direct CPU-register emitter.
It was corrected before final assessment. The linked regression now requires
actual replacements at both allocation bases, and the physical 10k run recorded
4,065 translation-time replacements. Those replacements did not improve FPS;
they are excluded from the normal player.

The two inline 10k runs recorded 987 and 1,002 late audio links (about 44–45% of
requests), against 1,236 and 1,458 (56–66%) in the paired baseline runs. This is
an improvement in delivery timing, **not stable audio**. Whole-session counts
include startup. No subjective listening result is claimed.

## Normal player validation and installation

The normal 0.22.5-dev-inline-read player passed the linked read/CPU/paging/SMC
suite at two allocation bases, speculative-read regressions and the 16 MiB x87
fixture. It contains no Duke counter instrumentation or timed auto-exit.

The original game reached the first level. Forward movement, turning and fire
worked in a window (ammunition 48 → 37), and after switching to fullscreen
(ammunition 37 → 30 on return). Alt+Return switched both ways; F10 exited cleanly.
The log records 9,727 VGA frames over 191 seconds, intact JIT/SIMD witnesses,
zero register/cache mismatches, and released owned memory. Those VGA frames are
not the rendered-game FPS used in the benchmark tables. All 4,442,112 queued
AHI frames completed with error=0; 2,207 late links still prevent an audio-stability
claim. The first session was stopped during loading and is retained separately;
it is not counted as a gameplay check.

The tested normal Expanded runtime replaced the installed executable after a
backup and checksum verification. The launcher, other runtimes and saved
10,000-cycle profile remain unchanged. `make build-expanded` now includes the
inline-read flag; register forwarding and the read-value ABI remain optional
experiments. [Development executable](../artifacts/development/expanded-0.22.5) ·
[Logs, model checks and recalculation script](../amiga/records/2026-10-19/duke-optimisation).

## Follow-up: 60,000 cycles

The same guarded-inline-read build was tested at 60,000 cycles, with the same
opening scene, source resolution, sound, CPU and presentation settings. No code
or guest executable changes were made between these runs and the inline 10k/20k
samples. These are DOSBox emulation cycles, not an ARM clock-frequency change.

| Cycles | Display | Game FPS | Emulated-time progress |
| ---: | --- | ---: | ---: |
| 10,000 | Window | 4.67 | 87.3% |
| 20,000 | Window | 6.77 | 54.1% |
| 60,000 | Window | 7.97 | 22.0% |
| 60,000 | Fullscreen | 8.13 | 22.3% |

Windowed 60k rendered 239 frames in 30.003 seconds; fullscreen rendered 244 in
30.003 seconds. Emulated time advanced only 6.599 and 6.705 seconds, respectively.
The extra rendered FPS therefore accompanies roughly 4.5× slower emulated-time
progress, rather than establishing a better gameplay configuration. The saved
10k profile is unchanged.

Across the whole sessions, 535/537 windowed and 539/541 fullscreen AHI links were
late (about 99.6%). All queued frames completed with error=0, but that does not
establish continuous sound. No listening result is claimed. Both runs passed the
JIT/SIMD witnesses, register/cache restoration and owned-memory release checks.
Accepted evidence is `inline60.log` and `inline60full.log` in the linked evidence
folder; `results.json` and `verify_results.py` include both samples.

## Follow-up: 80,000 cycles

Same guarded-inline-read runtime and benchmark settings; only the cycle value
changed. Loading is excluded from the thirty-second measurement.

| Cycles | Window FPS | Fullscreen FPS | Window game-clock speed | Fullscreen game-clock speed |
| ---: | ---: | ---: | ---: | ---: |
| 60,000 | 7.97 | 8.13 | 22.0% | 22.3% |
| 80,000 | 8.10 | 8.23 | 17.6% | 17.9% |

Windowed 80k rendered 243 frames in 30.000103 seconds; fullscreen rendered 247 in
30.005704 seconds. Emulated time advanced only 5.286 and 5.369 seconds. Compared
with 60k, the single samples added roughly 1–2% FPS while game-clock progress
fell about 20%. These small FPS differences are not evidence of a worthwhile
configuration improvement. The saved 10k profile remains unchanged.

Whole-session late AHI links were 435/437 requests in the window and 438/440
fullscreen (about 99.5%). All submitted frames completed, error=0; audible quality
was not checked. Both runs restored JIT/SIMD/register/cache witnesses and released
owned memory. `inline80.log`, `inline80full.log` and the updated results JSON retain
the evidence; the verification script recalculates both measurements.

## Follow-up: 100,000 cycles and launcher presets

Same inline-read runtime, scene and settings. Windowed rendered 248 frames in
30.007058 seconds (**8.26 FPS**); fullscreen rendered 253 in 30.001601 seconds
(**8.43 FPS**). Game-clock progress was **14.9% / 15.2%** of real time, respectively.
This is another small FPS increase alongside slower emulated-time progression.
CPU execution occupied about 92% of the measured interval.

Whole-session late AHI links were 376/378 requests windowed and 378/380 fullscreen
(about 99.5%). All queued frames completed with error=0; no listening result is
claimed. JIT/SIMD witnesses, register/cache restoration and owned-memory release
passed. Evidence: `inline100.log`, `inline100full.log` and the results verifier.

Launcher **0.22.6-dev** adds saved **60,000 and 100,000** choices to CPU & memory.
Select Fixed cycle control to retain the chosen budget; Auto and Max adjust it.
These choices automatically select Expanded, and dosbox.conf overrides retain
precedence. Existing saved cycle values are unchanged. The tooltip explains the
timing/audio trade-off; these are emulation budgets, not ARM clock settings.

## Reproducing the diagnostics

Build the normal Expanded tuning configuration with `--pc-duke-clock`; add just
one of `--pc-jit-value`, `--pc-jit-reuse` or `--pc-jit-inline` for an isolated
candidate, or `--pc-jit-hot` for block-frequency diagnostics. Each has a separate
output directory. These are opt-in research builds; the game-clock build stops
automatically after its sample and is unsuitable as an ordinary player.

The existing compiler-cache digest covers the generated headers. Build records
now also identify experiment flags, preparer sources and generated patch hashes.

Run `scripts/test_dosbox_jit_value.py BUILD` for byte/word/dword reads across all
4096 page alignments, partial registers, carry, video-memory handlers, CPU,
self-modification, paging and high-bank bounds. The fixture and framebuffer run
at two allocation bases. `scripts/test_dosbox_jit_speculation.py BUILD` also checks
invalid speculative bytes and self-modification at both bases.
`scripts/test_dosbox_jit_reuse.py` compares ARM register-forwarding replacements
against the original instructions in 320 cases; `test_dosbox_jit_hot.py` tests
counter reset, ranking and capacity. Instruction-model results precede, and do
not replace, physical tests.

No game executable, assets, save files, private guest-memory dump or local library
configuration is part of this report's source/evidence bundle.
