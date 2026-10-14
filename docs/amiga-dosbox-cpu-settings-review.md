# DOSBox-X CPU settings and ZZDosBox

Reference: [DOSBox-X CPU settings guide](https://github.com/joncampbell123/dosbox-x/wiki/Guide%3ACPU-settings-in-DOSBox%E2%80%90X).

The guide's cycle setting is an emulation work budget, not a literal MHz rating.
Its auto/max policies and CPU-model estimates should not be transferred to this
port without measurement. Excessive requested work can cause lag. Its dynamic
translation-block setting is distinct from total JIT-cache capacity.

## Verified local implementation

- `amiga/dosbox/pc/adapter.cpp` defaults to 3,000 cycles and sets the guest CPU
  architecture to `CPU_ARCHTYPE_386SLOW`. A dynrec build selects
  `CPU_Core_Dynrec_Run`; the interpreter build selects `CPU_Core_Normal_Run`.
- `amiga/dosbox/pc/devices.cpp` initialises `CPU_CycleMax` and `CPU_CycleLeft`
  from the selected fixed value.
- `amiga/dosbox/pc/bench.cpp` validates live requests between 500 and 10,000
  and updates `CPU_CycleMax`; the launcher session validation uses the same range.
- The launcher exposes fixed CPU-cycle choices. It does not implement the
  DOSBox-X auto/max policy, dynamic CPU-model selection or its translation-block
  tuning control.
- The 4 MiB JIT allocation is a total cache budget. Increasing it is not the
  same operation as increasing translated instructions per block.
- The native TinyGL diagnostic executes before the guest fixture. Changing
  guest cycles cannot accelerate its native ARM triangle routines.

## Proposed next CPU experiment

Run the same guest benchmark at 3,000, 5,000, 7,500 and 10,000 fixed cycles with
the same core, display and renderer. Record both throughput and emulated-time
progress/pacing; a speed-sensitive game can run faster than intended without
becoming a more accurate emulation. Retain a low fixed setting per game when
needed. Only consider a bounded auto policy after measuring available A9 time,
frame delivery and cancellation responsiveness.

CPU settings and profiles were not changed during the TinyGL follow-up. No new
cycle ceiling, automatic-speed capability or historical-PC ranking is claimed.
