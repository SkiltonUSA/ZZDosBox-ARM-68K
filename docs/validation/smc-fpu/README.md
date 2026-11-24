# Self-modifying code and x87 compatibility

The recorded 1.0.2 Beta Compat ARM payload (4 MiB JIT, 8 MiB guest RAM,
runtime clock calibration and x87) passes two new original DOS fixtures at two
ARM relocation addresses. Synthetic timer frequency: 400 MHz. Two inherited
FPSCR policies are tested; all 32 SIMD registers and supported FPSCR bits restore.

- **SMCALIAS:** locates a MOV instruction at a physical 4 KiB page boundary,
  independent of DOS's load segment. A different segment alias rewrites its
  immediate operand between 0x1234 and 0x5678, for 256 pairs of calls. Both
  relocations pass with actual JIT publication/dispatch counters.
- **FPROUND:** checks positive/negative half-integers under all four x87 rounding
  modes, then FSAVE/FRSTOR preservation of the control word and two stacked
  values. Both relocations pass.
- Existing x87 arithmetic, transcendental, extended-memory round-trip and
  host-context regression also passes at both addresses.

```sh
python3 scripts/test_dosbox_smc_fpu.py .context/release-102/compat
python3 scripts/test_dosbox_x87.py .context/release-102/compat
```

The new runner copies required inputs to a separate output directory. It never
modifies the retained player. Tests require Unicorn, Clang and LLVM lld/nm.

These are instruction-model correctness results, not a paged-JIT speed gain,
complete emulator save-state implementation or physical-cache validation.
No player or installer changed. Physical runs remain pending because the bridge's
Amiga connection was disconnected before any hardware test started.

[Results](results.txt) · [Input and fixture hashes](hashes.json).
