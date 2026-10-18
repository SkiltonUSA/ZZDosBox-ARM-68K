# Descent rendered-frame samples

See the [method, settings and interpretation](../../../../docs/amiga-dosbox-descent-performance.md).
Accepted physical samples: w6000 (window VGA), f6000b (fullscreen VGA),
s36000 (S3), s310kadapt (S3/adaptive/10000), b64s310k (same, experimental
64-instruction JIT), low-old and low-new (existing custom detail before/after).
All are single 30-second stationary samples, sound off. Default blocks remain 32.

results.json reports completed game frames, wall seconds, FPS, guest/wall time
and exclusive profiler percentages [other, CPU, events, capture, grant, copy].
The first window sample predates exclusive profiler reporting.

Build JSON records diagnostic executable hashes. These are isolated benchmark
builds, not replacement production players. Build with --pc-descent-clock on
the expanded runtime. The source observer does not modify guest game memory.
Private ROM/game files, pilot data, RAM snapshots and personal library are excluded.
Intermediate overlapping-report attempts are excluded; only corrected extended
reports (e00) and the earlier unaffected short window report are accepted.

Reproduce the 32-instruction diagnostic build in the standalone repository:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
  --pc-pic-vfp --pc-compat --pc-x87 --pc-expanded --pc-file-readahead \
  --pc-descent-clock
```

Use a private copy of Descent shareware 1.4 with DOS32A 7.35. Start level 1,
Trainee, and skip the briefing; do not send input or take screenshots during
the automatic 35-second settle/sample interval. Runtime `--profile` enables
exclusive stage accounting. S3 and adaptive use `--machine s3 --presentation
adaptive`; select 6000/10000 cycles and window/fullscreen in the session.
The 64-instruction experiment additionally applies block64.patch to the generated
core_dynrec.cpp before compiling. It is not the recommended player configuration.
