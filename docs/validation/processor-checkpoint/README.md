# Coordinated CPU/x87 checkpoint

Development-only; public/installed 1.0.2 Beta is unchanged.

The 420-byte processor record contains the existing real-mode CPU and portable
x87 components. Saving stages both before publishing any bytes. Loading copies
the record to private storage, validates both components, then restores them
with guest execution stopped and no intervening callbacks. A damaged x87 record
cannot leave the CPU partially restored. Component formats remain unchanged.

The test-first sequential loader fails exactly at the unchanged-CPU assertion
when the FPU record is corrupt. The corrected transaction passes ASan/UBSan:

- 200 actual interpreter continuation boundaries and fresh-process CPU/FPU/test
  RAM replay match uninterrupted execution.
- Every one of the 420 byte corruptions, every truncated length, a trailing byte,
  null input, unsupported CPU mode and valid-CRC semantic corruption in either
  component is refused without changing CPU or FPU state.
- Failure to capture the FPU leaves the caller's destination unchanged.
- All three components compile for Cortex-A9 ARM32 with the player ABI.
- Existing integer-component regression and Cortex-A9 x87 continuation tests pass.
  The latter tests the FPU component; it does not establish ARM execution of the
  new combined transaction or physical game restoration.

[Native results](native.txt), [CPU regression](cpu-regression.txt),
[ARM component regression](arm-regression.txt), [identities](identities.json).

Run `python3 scripts/test_dosbox_fpu_checkpoint.py` to reproduce the transaction
and continuation tests. Devices, RAM, scheduler, DOS/files, protected mode and
storage still need whole-machine coordination. This is not a usable game-state
format and does not enable launcher save slots.

## Coordinated ARM32 continuation

The same pinned normal interpreter now runs in a Cortex-A9 instruction model
from five CPU/x87 records produced natively. Each restored point continues the
x86 program to its common endpoint, matching the entire processor record and a
hash of all 2 MiB of diagnostic RAM. The fixture exercises four rounding modes,
FSAVE/FRSTOR and arithmetic. All 2,100 single-byte corruptions reject without
changing either CPU component. The 200-boundary native sanitizer and fresh-process
regressions still pass after sharing their fixture setup.

The model runs the actual interpreter, not a stub CPU. Host I/O remains the
existing diagnostic adapter; RAM is restored by the test harness, not by a new
production component. Static C++ constructors run before the model's main;
fixture-only assertion/exit/delete traps fail the probe if unexpectedly reached.
This is not physical hardware, an FPS result or a complete PC snapshot.

```sh
python3 scripts/test_dosbox_processor_checkpoint_arm.py \
  --prepared .context/checkpoint-fpu --output .context/checkpoint-processor-a9
```

Requires the pinned interpreter preparation, Unicorn, the ARM toolchain and the
existing bare-metal libc archive, as for the earlier x87 model test.
[Results](arm-interpreter.txt), [input identities](arm-identities.json).
