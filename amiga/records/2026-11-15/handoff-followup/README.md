# Display handoff follow-up

Private 68k diagnostic hosts retained the recorded release ARM payload and
4 MiB JIT cache. No installed player, firmware, AHI preference or game profile
changed. Tests use Doom demo playback, not timedemo gameplay FPS.

- Ordinary publication/readback run: 1,399 VGA updates, 1,399 marker checks,
  no changed markers; 640,000 queued/submitted/completed PCM frames.
- Actual Intuition screen-depth stress: screen behind Workbench for 500 ms,
  then six window/fullscreen transitions. 1,400 updates, 985 fullscreen marker
  checks, zero changed markers, 265 foreground waits. All 640,000 PCM frames
  completed. Register/MMU/cache/SIMD restoration and release passed.
- Deliberately corrupting one bit of the last marker at frame 100 triggered
  error 7 and exit 20. The new production diagnostic reported the exact changed
  bit. The worker stopped, restoration checks passed and memory was released.
  Challenge rejection was not bypassed or repaired.

The production diagnostic records the granted sequence, mapped address, pitch,
format and, for error 7, expected/observed endpoint markers while the bitmap
remains locked. These are **host observations after ARM completion**, not a
record of the values ARM saw earlier. No diagnostic reads are added on success.
Both windowed and fullscreen error paths use the same helper.

The original intermittent nonce failure has not been reproduced by the first
two tests, so its cause remains unresolved. Device gaps remain recorded;
completed PCM delivery does not prove clean audible output. The first final stress run stopped progressing at 68 updates / 1,002 guest
milliseconds because its custom screen was behind Workbench before the planned
frame-80 test. It accumulated 175,552 foreground waits. A targeted Ctrl-C to
its identified CLI process stopped it normally; every restore/release check
passed. This is a **cancelled incomplete run**, despite the shutdown logger's
`SHELL PASS` line. It is not a completed 20-second playback or a speed result.
The private retry explicitly requests foreground once if this happens before
its scheduled screen-depth test; production never forces a background screen
forward.

`build-handoff.py` captures the private instrumentation and requires archived
release inputs in the development workspace. `DOS_VIEW_STRESS=1` enables real
screen-depth and mode switches; `DOS_BAD_NONCE=1` enables the deliberate marker
fault. Neither injection is part of production code. The native host-review
suite also passes: console spans, presentation schedules, triple-slot handoffs
and oversized network packet handling.

## Diagnostic-enabled final run

The retry completed 20,000 guest milliseconds / 1,400 VGA updates in 55.256 s.
It performed the planned 500-ms screen-depth change and all six view switches.
Its one-time initial-foreground recovery was not needed. There were 270
foreground waits, 982 fullscreen marker checks and no changed markers.
All 640,000 PCM frames completed; register/MMU/cache/SIMD checks and exit 0
with memory release passed. The recorded 623 audio gaps remain unresolved.
This verifies the added error reporting does not break the exercised normal
window/fullscreen paths; it does not resolve the original intermittent error 7.
