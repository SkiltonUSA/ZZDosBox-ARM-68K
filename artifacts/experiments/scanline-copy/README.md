# Experimental Classic scanline copy

Separate test executable; not promoted to the normal launcher.
`pc/ZZDosBox-Dynrec --scanline-copy off|full|skip` selects normal output,
black rows rewritten every frame, or black rows initialized once per buffer.
F9 cycles these modes. Fullscreen 640x480 only; window/overlay output is unmasked.
This is a software mask, independent of the Workbench FPGA scanline controls.

Doom Demo3: 7.33 / 7.44 / 7.60 real FPS. Single runs; preliminary +3.7% vs normal.
[Method, tests and limitations](../../../amiga/records/2026-10-13/dosbox-scanlines/method.md)
[Build instructions](../../../docs/build-and-restore.md)
