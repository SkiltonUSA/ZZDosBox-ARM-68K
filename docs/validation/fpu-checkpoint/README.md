# Portable x87 checkpoint validation

Development-only addition after source `ae71be2`. Public/installed **1.0.2 Beta**
is unchanged. [Exact source and test build identities](identities.json).

## Checks

- First run with an unimplemented serializer fails at the first save assertion.
- The actual pinned normal interpreter and portable FPU then pass 200 CPU/FPU/RAM
  restore-and-continue points: four rounding modes, FXCH, arithmetic, square root,
  memory conversions and guest FSAVE/FRSTOR. Output integer conversions are also
  checked against explicit expected values, not just a second run.
- Fresh-process CPU/FPU/RAM replay matches uninterrupted execution.
- Nine register payloads preserve signed zero, subnormal, infinity and both NaN
  payload cases. Working TOP and status TOP can differ; derived rounding and
  masked control state are rebuilt.
- Every single-byte corruption, truncated record, trailing byte and 16 malformed
  records with recalculated valid CRCs are rejected without changing FPU state.
- Native tests pass ASan/UBSan. The Cortex-A9 model loads native-generated records,
  runs the actual portable FPU's FRNDINT/FADD/FCHS/FSQRT paths in four rounding
  modes and matches the complete expected records. It also rejects 560 corrupt
  records atomically. ARM uses VFPv3 with contraction disabled, as the player does.

[Selected results](results.txt).

```sh
python3 scripts/test_dosbox_fpu_checkpoint.py --output .context/checkpoint-fpu
python3 scripts/test_dosbox_fpu_checkpoint_arm.py \
  --prepared .context/checkpoint-fpu --output .context/checkpoint-fpu-arm \
  --compiler <complete-arm-none-eabi-toolchain>/bin/arm-none-eabi-g++ \
  --libc .context/amiga/dosbox-libc/libc.a
```

The ARM runner requires Unicorn, LLVM tools and the existing PIC libc archive.
The bare-metal fixture excludes unused host directory declarations from `cross.h`
while preserving its GNU `LONGTYPE` macro; no FPU instruction logic is replaced.

## Limits

This is a component checkpoint, not a game save format. CPU and FPU records subsequently gained
[all-or-nothing processor prevalidation](../processor-checkpoint/README.md);
a full machine transaction remains unfinished. Devices,
scheduler, DOS/files, paging and media identities are not captured by this test.
The private fixture file is not distributed as a supported save state.

No player was rebuilt, installed or released with this component. There is no
physical restored-game or FPS claim. Native/ARM agreement does not improve the
portable backend's existing double precision or establish full x87 conformance.
