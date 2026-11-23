# Runtime timer — packaged Expanded-core candidate

This pass carries the calibrated presentation/FPS timer into the recorded
Expanded core, preserving its **4 MiB JIT cache, 32 MiB guest-RAM maximum and
64 MiB Exec allocation**. These are the packaged core's values; this is not the
separate 32 MiB JIT experiment. The source player's SHA-256 is
`3ad6328ad6b051fae93dbe0acfba645fa7e5335b1101834ddcbd86c5361823a0`.

The bench entry and direct presenter are rebuilt with `DB_RUNTIME_CLOCK` against
the retained core objects. The 68k host is rebuilt with Expanded support and the
64 MiB allocation. [Build provenance](build.json) records every linked ARM
object, the two replacement objects, source hashes and resulting player hash.

Native calibration, adaptive cadence and FPS tests pass at both timer rates.
The physical tests keep the existing 800 MHz setting and select the measured
400 MHz timer. No firmware, PLL, emulated CPU cycles or audio-rate changes are
part of this candidate.

## Hardware checks

All four Doom demo3 checks completed 20,000 guest milliseconds and 640,000
stereo audio frames, with timer estimates within 6 kHz of 400 MHz. The tested
modes were windowed Normal, windowed Adaptive and fullscreen Adaptive at 8 MiB,
then windowed Adaptive at 32 MiB guest RAM. Every run passed SIMD context,
register/MMU/cache restoration, file closure and owned-memory release.
Audio gaps remain. [Measurements and individual logs](hardware.json).

## Limits before promotion

This is a private development candidate, not a replacement installer. A clean
whole-core rebuild with the pinned DOSBox toolchain remains required. The local
GNU/picolibc combination could not build all current DOSBox sources (newlib/C++
headers and directory API mismatch); the retained packaged core avoids silently
substituting a different core. Other release variants also need calibration and
shutdown checks. Stock-clock hardware has not been tested in this pass.

The results do not establish an FPS gain or uninterrupted audio. Source-frame
and presentation counters are not Doom's internal rendered-game FPS. Descent
and Duke-specific observers retain separate clock assumptions and are outside
this candidate.
