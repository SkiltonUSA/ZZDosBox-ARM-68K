# First completion-driven GPU measurements

After the firmware restart, the ZZ9000 completed all four offscreen runs at
800 MHz ARM / 100 MHz GPU design clock. Each run passed five correctness scenes,
final pixel readback, timer calibration and clean GPU release. The bridge
responded afterward and no application reservation remained.

| Completed frames | Timer ticks | Offscreen FPS |
| ---: | ---: | ---: |
| 1 | 41,996 | 9,524.927 |
| 10 | 421,032 | 9,500.304 |
| 100 | 4,210,573 | 9,499.805 |
| 1,000 | 43,000,743 | 9,302.194 |

The scene is a **64 × 72 RGB565 solid triangle**. The timer includes Core0 scene
preparation, submission and completion polling. Preflight checks, final readback
and application startup/close are outside the measured interval. The 1,000-frame
batch lasted about 107.5 ms; this is a throughput check, not an endurance test.
Temperature before the runs ranged from 55.4 to 56.3°C.

This establishes normal-path retirement and readback on the matching FPGA image.
It does not establish GTA FPS, full-resolution fill rate, display refresh rate,
physical fault recovery or a speed gain over the earlier firmware. The earlier
ARM/68k readback discrepancy has not been independently resolved by this test,
which verifies pixels on Core0.

Next: connect guest Glide drawing/state/texture calls, implement the required
depth/LFB behaviour and presentation, then measure an actual game workload.

[Structured result](result.json) · [Counter contract](../../../experimental/measurement.md)
