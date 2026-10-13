# Backup validation — 13 October 2026

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
