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
