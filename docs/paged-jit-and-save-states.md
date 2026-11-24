# Paged JIT and save-state development

The released player remains **1.0.2 Beta**. These are isolated experiments;
full emulator save states and effective paged-JIT acceleration remain unfinished.

## Paged JIT

The existing opt-in paged JIT translates one register-only instruction at a
time. Memory access, branches, stack operations, prefixes and x87 instructions
continue through the paging-safe interpreter. This preserves its page-fault
restart behavior, but repeatedly switching execution paths costs time.

The new `--pc-paged-blocks` build option translates up to 32 consecutive eligible
instructions. Classification stops at the current page and 16-bit segment
boundary, an incomplete instruction or an unsupported opcode. Page presence,
physical RAM and CPU-specific user/supervisor permissions are checked first.
Code invalidation and paging-mode cache flushes remain active; paged block
linking remains disabled.

`--pc-paged-burst 32|128|256` controls the experimental interpreter fallback
budget. It consumes at most the cycles already available before the next
scheduled event. The default remains 32; this does not increase guest CPU speed
or permit a batch to run past the scheduler's allocated cycle budget.

Both options require an isolated Expanded build. For example:

```sh
python3 scripts/build_dosbox_dos_arm.py \
  --base <recorded-expanded-build> \
  --pc-paged-blocks --pc-paged-burst 256 \
  --output .context/paged-register-candidate
```

Use `--paged-jit on` only with the experimental player. The released paging
fallback remains the default. Native bounds tests include a protected host page;
the ARM instruction model checks remapping, self-modifying grouped code,
instruction-fetch faults, read/modify/write faults, nested faults and 386/486
privilege behavior at two runtime relocation addresses.

Hardware measurements and exact build identities are recorded in
[the validation record](validation/paged-jit-checkpoints/README.md).

## Save states: first CPU component

`amiga/dosbox/state/cpu_checkpoint.cpp` implements a **development-only real-mode
integer CPU checkpoint**. It is not linked into the production player and does
not enable a Save State button.

The component records fixed little-endian fields: integer registers, instruction
pointer, pending arithmetic flags, segment state, relevant real-mode control
state and cycle counters. A schema, architecture identifier, exact length and
CRC32 are checked before restoration changes the CPU. Host pointers, translated
ARM instructions and cache entries are never stored. Protected/paged execution,
trap decoders and other unsupported contexts are rejected.

The actual pinned normal interpreter was resumed at 200 instruction boundaries.
The test covers carry-dependent arithmetic, stack flags, loops and REP memory
operations. CPU state and the complete diagnostic RAM image match uninterrupted
execution. A separate test writes CPU/RAM fixture data and resumes it in a fresh
process. Every single-byte corruption, truncation, extra trailing data and an
architecture mismatch are rejected. AddressSanitizer/UndefinedBehaviorSanitizer
pass. The component also compiles for Cortex-A9 ARM32; it has not yet been used
for a physical game-state restoration.

Validation also rejects 20 deliberately malformed records with recalculated,
valid CRCs. Checks now cover selector/descriptor widths, lazy carry, 16-bit stack
masks and the relationship between DF and string direction. Rejection leaves the
CPU unchanged. Both forward and reverse string direction remain restorable.
The existing 200 continuation points and fresh-process CPU/RAM replay still pass
under ASan/UBSan. This is correctness work, not a JIT speed improvement.

```sh
python3 scripts/test_dosbox_cpu_checkpoint.py --output .context/checkpoint-validation
```

The private CPU/RAM test file is **not** a supported game-state format. The
current component intentionally does not promise hidden protected-mode state,
x87 state, a running DOS kernel or device restoration.

## Work needed before game save/load

1. Add protected-mode CPU/TSS, paging and x87 components; rebuild mappings and
   invalidate translated code when loading.
2. Capture the full scheduler, event queue and interrupt state using stable
   callback identifiers instead of function addresses.
3. Serialize VGA/video memory, DMA, timers, keyboard/mouse and sound devices.
   Discard queued host audio after restoration and refill it from restored
   emulation state.
4. Restore DOS allocation/kernel state and reopen guest files at their saved
   positions. Define consistent handling of changed disk images and external
   serial/network/MIDI devices.
5. Add a complete machine/build/media manifest and prevalidate every component
   before applying a state. Write through a temporary file and retain the
   previous valid save if writing fails.
6. Add launcher slots only after fresh-process game continuation, damaged-file
   rejection, disk consistency and clean-exit tests pass on the Amiga.

DOSBox Pure's `DBPSerialize_CPU` and broader serialization subsystem informed
the component inventory. The new restricted checkpoint code is original; its
format is not compatible with DOSBox Pure or DOSBox-X snapshots.
