# Backup validation — 13 October 2026

## Latest dynrec / NEON / scanline backup

The exact installed DOS executable and separate scanline candidate are saved
with build metadata and ELF symbols. All backup hashes verified. Exported host
tests and sanitizer checks passed; the restored scanline ELF passed the ARM
model Full/Skip/Off transitions across three buffers with poisoned rows, pixel
guards, nonce repair and SIMD restoration. The raw benchmark analyser reproduced
7.3281 / 7.4409 / 7.5970 real FPS. No new hardware run was needed for this backup.

Validation logs: `amiga/records/2026-10-13/dosbox-scanlines/backup-*.log`.
Current runtime SHA256:
`55085f15f23bc3fc077498e2b2cd9bf3770558a9706147ece163240ac73dcb7d`.
Scanline experiment SHA256:
`02442db69194befb55f8644c61badc01655bcc493d63adc11a3051ef75d89144`.

## Earlier baseline backup


The independent export restored its pinned upstream Git bundle and saved build
inputs without using the source workspace checkout. Host tests passed for
profiles/AUTOEXEC, frame ownership, framebuffer mapping, profiling, capture and
scaling. The native AddressSanitizer EGA/PIT/DOS fixture passed. The archived
ARM ELF passed relocation, CPU/DOS, display, cancellation, guest bounds, BIOS
boot and hard-disk instruction-model tests. Logs are in `backup-checks/`.
These checks did not connect to the Amiga. Physical evidence remains in the
dated records; the saved current executable matches the installed baseline.

SHA256: `65b433b4902c7fe495bdf562129cff912fc7a5eecf4917b7e3837edfb1c561f5`

A restore-script omission of the original native C-drive fixtures was found
and fixed before this backup. Container commands are configurable in the
export. Runtime source and current executables were not changed for export.

## TinyGL research backup

The standalone checkout passed `make test-host` and the O3-vector native
TinyGL ASan/UBSan/exact-output suite. The launcher header's optional monochrome
helper is now `static inline` so strict standalone builds do not report it as
unused; behaviour is unchanged. Native logs are in `docs/backup-checks/`.

The backup retains the previously completed physical and linked-ARM evidence,
including nine fresh compiler-comparison runs, three runs per compiler profile.
The backup operation did not rerun hardware workloads or replace the installed
Direct executable. Exact diagnostic binaries and original source snapshots are
preserved under `amiga/records/2026-10-14/`; the export build script retains its
configurable `DOSBOX_ARM_COMMAND` adaptation. No release tag was created.

## TinyGL rejection and packing follow-up

Standalone `make test-host` and dispatch ASan/UBSan/exact-image tests passed.
Four dedicated builds passed sixteen linked ARM lifecycle checks and twelve
physical runs; only their ztriangle ARM object differs. Every physical scene
matches its native colour/depth hash. The separate earlier rejection experiment
and its nine physical runs are also preserved. These are native private-buffer
results, not DOS FPS gains. The installed executable remains unchanged.

Exact sources, binaries, tests and checksums are in the dated rejection/packing
records. Local workspace prefixes in text evidence are normalised; binary bytes
and hardware report values are unchanged. No release tag was created.
