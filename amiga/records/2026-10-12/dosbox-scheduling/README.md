# ZZDosBox host wait comparison

Physical A4000TX / ZZ9000 XX19c, 2026-10-12. All game runs use the same
8-second emulated window, 3,000 cycles/ms, 558 generated frames and direct
640×480 output. No screenshots were requested during measurements.

- `comparison-build.json`: binary used for the 20/4/1 ms comparisons.
- `comparison.json`: extracted counts and actual run durations.
- `keen-20.log`, `keen-4.log`, `keen-1.log`: controlled Keen comparisons.
- `vette-20.log`, `vette-1.log`: controlled Vette comparisons.
- `fixture-1.log`, `cancel-1.log`: EGA/PIT and input-wait cancellation.
- `build.json`: final host build, adding observed copy latency.
- `*-1-profile.log`: repeats using that final build and default 1 ms timer.
- `*-events.json`: deployment/test-tool evidence.

All builds use an identical ARM ELF, SHA256
`25f08068b30229464a6f57225d20eea822120a71f0b2a4f5a30254b56b96366f`.
The 20 ms option retains DOS `Delay(1)`; it is a tick-based wait whose measured
average here is about 32 ms. The 1 and 4 ms options use timer.device DoIO and
yield the host task. Polling/wait durations overlap ARM work; they are not
additive CPU-time measurements. COPY timings include host observation delay.

These are whole-startup-run throughput results, not real-time gameplay or
steady-state benchmarks. Game files are not included. See
`docs/amiga-dosbox-feasibility.md` for interpretation and remaining work.

Final installed binary is 695,036 bytes, SHA256
`11e71fdbfa8f9e561fb7ea8f0c09843986bd0dfaa015210f8cfd14dd42832627`.
`promotion-events.json` records the local Amiga copy and preservation of the
previous executable. `fixture-1-default.log` verifies the normal installed
path with no polling override. `final-state.log` records the final Workbench/
bridge state and absence of a Core1 owner.
