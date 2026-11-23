# Runtime ARM timer calibration — experimental

The FPS overlay and adaptive presentation previously assumed a 333.333 MHz
ARM global timer. At an 800 MHz CPU setting, the timer runs at 400 MHz. A fixed
frequency changes both the overlay calculation and adaptive frame budgets.

The opt-in `--pc-runtime-clock` build measures two existing Core1 clock replies
against AmigaOS timer.device over roughly two seconds. It validates the reply
sequence/token and accepts measured ranges around the supported 666.7/800 MHz
CPU settings. It publishes the selected timer frequency and an ABI marker to
the worker before emulation. Unknown rates or an invalid exchange stop startup.
No PLL, firmware, audio rate or emulated CPU cycle setting is changed.

FPS and adaptive timing use the same selected frequency. Presentation state is
reset if its rate changes. The build switch requires a separate output folder;
it remains off in normal builds pending integration across release variants.

## Checks

Native ASan/UBSan tests cover interval/rate bounds, identical real-time adaptive
cadence at both clock rates, and FPS conversion. The existing presentation
regressions pass. Run `python3 scripts/test_dosbox_runtime_clock.py`.

Physical Doom demo checks at the standing 800 MHz setting passed windowed Normal,
windowed Adaptive and fullscreen Adaptive. Each ran 20,000 guest milliseconds,
produced 1,400 source frames and completed all 640,000 stereo output frames.
Measured timer frequencies were within 6 kHz of 400 MHz. All passed SIMD witness
42/context equality, register/MMU/cache restoration and memory release.
[Measurements](hardware.json) · [Private build identity](build.json).

These checks used a retained 4 MiB JIT/8 MiB guest-RAM core with an isolated
host and clock-aware presenter. They do not benchmark the latest release or
prove an FPS gain. Audio gap events remain. Hardware at 666.7 MHz was not tested
in this pass; the existing 800 MHz setting was retained as requested.
The older Descent/Duke-specific diagnostic observers still have their own fixed
clock assumptions and are outside this change.

## Test-build corrections

An initial private build selected a generic MMU header before the recorded
runtime header. The existing mapping guard refused the first frame (error 6),
and the launcher restored its state and freed memory. Include order was fixed.
A subsequent host build incorrectly enabled the separate startup SIMD witness
layout; Doom completed but that unrelated zero witness failed final validation.
The final build uses the DOS runtime's own SIMD report, as the normal builder
does. Both rejected builds remain recorded locally and were never installed.

## Expanded-core follow-up

The same timer integration now has a separate candidate using the retained
packaged Expanded core (4 MiB JIT, 32 MiB guest maximum, 64 MiB allocation).
See [provenance, hardware checks and promotion limits](expanded/README.md).
