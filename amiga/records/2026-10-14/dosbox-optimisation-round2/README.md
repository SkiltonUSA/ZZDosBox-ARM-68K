# ZZDosBox optimisation round 2

[Report](../../../../docs/amiga-dosbox-optimisation-round2.md).

- `cache-results.json` and six `physical-cache-*.log` files: matched Windows
  startup comparisons; 12,000 guest ms and 772 frames each. Profiling enabled.
- `render-results.json`: complete Doom demo3 comparison. Profiling disabled.
  `render-capture-a.log` is explicitly rejected: flip ACK failure after the
  demo sequence. Other runs completed checked restoration and teardown.
- `midi-runtime.log` / `midi-capture.log`: physical internal CAMD loopback;
  no external MIDI output. Byte/message order and SysEx validated.
- `pit-math-before.log`: expected old scalar-build failure in the instruction
  model, where coprocessor access was disabled. Linked library `fmod` uses VFP.
- `*-simd-test.log`: PIT/library math with hostile inherited FPSCR, exact
  restoration of all SIMD registers/FPSCR, and denied-access handling.
- `*-safe-model.log`, `dynamic-paging-test.log`: linked ARM instruction tests,
  including paging fallback, disk bounds and cancellation. Not hardware timing.
- `cache-all-sizes.log`, `store-final.log`, `developer-final.log`: native
  sanitizer regressions. `present-tests.log` models buffer ownership schedules.
- `windows-safe-boot-model.log` / `wfw-model-interpreter-safe.json`: Windows
  interpreter instruction-model boot, 4,000 guest ms; SIMD state restored.
  Model wall time is not an A9 performance measurement.
- `physical-windows-final-check.log` / `physical-dynamic-final-check.log`:
  final deployment builds, each with 1 MiB cache and mandatory SIMD context
  guard. Both completed checked restoration and teardown. These are validation
  runs, not a controlled interpreter/JIT performance comparison.
- `final-installed-check.log`: installed/backup checksums, unchanged settings,
  idle runtime and responsive bridge/Roadie.
- `artifacts.json`: hashes of deployment outputs; `*-build.json` retain build
  options and the pinned upstream revision. `sources.json` fingerprints current
  adapter, launcher and regression sources. No game or OS binaries here.

The on-demand compatibility file is a generated build artifact. Rebuild the
launcher with `scripts/build_dosbox_launcher.py` and keep its matching
`Compatibility.dat`. Source facts remain under `amiga/dosbox/compatibility/`.

Example checks after building the PC payloads:

```sh
python3 scripts/test_dosbox_disk_cache.py
python3 scripts/test_dosbox_store.py
python3 scripts/test_dosbox_midi_host.py
uv run --with unicorn==2.1.4 python scripts/test_dosbox_simd_context.py .context/amiga/dosbox-pc-cache1024
uv run --with unicorn==2.1.4 python scripts/test_dosbox_paging_cases.py .context/amiga/dosbox-dynrec-4m-cache1024
```

No unguarded paged-JIT payload was deployed. The rendering candidate remains
experimental; the installed DOS-game runtime is unchanged.
