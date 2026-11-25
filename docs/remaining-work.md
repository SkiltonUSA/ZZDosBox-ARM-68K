# ZZDosBox remaining work

Current release: **1.0.2 Beta**. [Release notes](release-notes.md).

## Active work

- Complete whole-machine save states. Coordinated real-mode integer CPU/x87
  checkpoints pass development tests; protected/paged CPU state, RAM, scheduler,
  devices, DOS file handles and storage consistency still need integration.
  Add launcher slots after physical fresh-process game restoration and failure
  recovery pass. [Implementation plan](paged-jit-and-save-states.md).
- Find a useful paged-JIT acceleration path. Bounded grouped-register candidates
  pass correctness checks, but no released paged-JIT speed gain is established.
  [Measurements](validation/paged-jit-checkpoints/README.md).
- Broaden protected-mode, self-modifying-code and x87 game coverage beyond the
  passing alias/rounding fixtures. [Existing checks](validation/smc-fpu/README.md).
- Extend physical serial, USB serial, MIDI, network and controller interoperability
  tests. A selectable option is not proof of every attached device working.
- Validate operation on a 68030; the compiler target is not hardware evidence.

## Completed or retained

1.0.2 runtime timing calibration and installer work are complete for the players
listed in the README. CPU/x87 development components are not production save slots.
Legacy silent/scanline and network-interpreter timing restrictions remain documented.

## Parked

Glide-to-RasterIX/GTA acceleration and Empire-specific investigation. Retain the
experimental measurements; do not describe them as released capabilities or
as the next development priority.
