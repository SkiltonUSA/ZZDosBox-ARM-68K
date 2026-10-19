# Duke ARM optimisation evidence

Thirty-second opening-level samples after five seconds settling. Same Duke 1.3D
shareware counter instrumentation, 320×200/default detail, S3/Adaptive, 486,
8 MiB, EMS, SB16/OPL3; both displays use 640×480 output. These are rendered game
frames, not VGA refresh counts. Game-clock progress is reported separately.

- `w10a/w12/w15/w20`, `f10/f12/f15/f20`: baseline cycle/display comparison.
- `audio8/audio10/audio20`: existing late AHI-link counter made visible.
- `candidate10/candidate20`: read-value ABI experiment, not promoted.
- `reuse10/reuse20`: corrected register forwarding, not promoted.
- `inline10/inline20`, `baseline10b/inline10b`: guarded inline reads and repeat.
- `inline60/inline60full`: guarded inline reads at 60,000 cycles, window/fullscreen.
- `inline80/inline80full`: same runtime at 80,000 cycles, window/fullscreen.
- `inline100/inline100full`: same runtime at 100,000 cycles, window/fullscreen.
- `hot`: gameplay-scoped block entry counts; instrumented FPS excluded.
- `controls-*`: original, unmodified game; physical interactive/exit checks.
- `model-*`: linked ARM instruction-model tests; not physical execution proof.
- `test-*`: native regression suites.

Run `python3 verify_results.py` to recompute FPS and check accepted teardown logs.
`results.json` records guest-time progress and exclusive profile percentages.
`runtime-hashes.json` identifies private diagnostic builds and the normal player.
No game executables, assets, personal configuration or guest-memory dumps are
included. Diagnostic binaries are excluded; the normal player is under
`artifacts/development/expanded-0.22.5`.

AHI completions and zero error codes do not establish continuous sound. `gaps`
counts successors submitted after their predecessor completed, across the whole
session. Audible quality was not verified. See the
[report](../../../../docs/amiga-dosbox-duke-arm-optimisation.md).
