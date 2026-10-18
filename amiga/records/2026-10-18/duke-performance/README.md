# Duke Nukem 3D rendered-frame comparison

[Method and interpretation](../../../../docs/amiga-dosbox-duke-performance.md).
base = VGA/6000/normal; s3only = S3/6000/normal; tuned = S3/10000/adaptive.
All runs use the same window, 486 model, 8 MiB RAM, EMS, SB16/OPL3 and game detail.
results.json records actual game-frame deltas and wall time, guest-clock progress,
and exclusive profiler percentages. Game binaries and personal libraries are excluded.

instrumentation.json records the private EXE's original/diagnostic hashes and the
two-byte change enabling the existing FPS counter. The original installation is
unchanged. No instrumented game executable is published. The linked runtime is
an isolated measurement build, not a replacement release player.

Reproduce the diagnostic runtime:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-neon --pc-pipeline --pc-scanlines --pc-presentation --pc-adapters \
  --pc-pic-vfp --pc-compat --pc-x87 --pc-expanded --pc-file-readahead \
  --pc-duke-clock
```

Use a private copy of the matching shareware executable, verify the original
hash/bytes before enabling its counter, and use DUKE3D.EXE /v1 /l1 /s1.
Restore the same game configuration for each run. Runtime --cpu 486 --ems on
--sound sb16 --profile; add --presentation adaptive for tuned. Select adapter,
6000/10000 cycles and window mode in the session. Allow automatic shutdown;
no input/screenshots during sampling. Every accepted log must have a valid
DUKECLOCK record, JIT witness, SIMD context equality, zero register/cache witness
mismatches and normal owned-memory release.
