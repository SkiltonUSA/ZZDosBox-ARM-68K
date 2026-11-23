# Full-core runtime timer rebuild

The Expanded runtime now builds from the current DOSBox source using a matching
Debian GCC 15.3.1, target libstdc++ and picolibc 1.8.12 toolchain. All 108 ARM
objects were compiled in a new output directory; no retained DOSBox object was
linked. The existing pinned PIC libc archive remains a build input.

The earlier failure came from mixing the Arm download's newlib-based C++
headers with Debian picolibc. Installing `libstdc++-arm-none-eabi-common` alone
also omits the target `bits/c++config.h`. The full `libstdc++-arm-none-eabi`
package supplies it. [Recipe and setup](../../build-and-restore.md#matching-arm-compiler-and-c-headers).

[Build identity and source hashes](build.json) · [Toolchain versions](toolchain.txt).
The candidate retains 4 MiB JIT, 32 MiB guest maximum and 64 MiB Exec allocation,
with calibrated FPS/adaptive timing. Default installers remain unchanged.

## Validation

Four physical Doom demo3 checks passed: windowed Normal, windowed Adaptive and
fullscreen Adaptive with 8 MiB guest RAM, then windowed Adaptive with 32 MiB.
Each completed 20,000 guest milliseconds and all 640,000 stereo output frames.
Timer estimates were within 9 kHz of 400 MHz. SIMD context, register/MMU/cache
restoration, file closure and owned-memory release passed in every run.
[Measurements and logs](hardware.json).

Native tests passed for runtime clock calibration/cadence/FPS, presentation,
console ring transfers, double/triple-buffer handoffs, oversized network receive
and file read-ahead. The physical runs still recorded audio gaps; these results
are not a listening-quality or game-FPS claim.

## Remaining promotion gates

This closes the clean Expanded-core rebuild gap. Later reports cover
[standard/memory-debug](../variants/README.md) and
[network, Windows and software OpenGL](../remaining-variants/README.md). Hardware remains at the requested 800 MHz KEEP; 666.7 MHz has
native clock/cadence tests but no hardware run in this pass. Audio gaps remain,
and these checks do not establish an FPS gain. No firmware is changed.
