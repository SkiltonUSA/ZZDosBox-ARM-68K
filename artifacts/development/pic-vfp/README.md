# Direct runtime with A9 PIC floating point

The measured and installed **0.11.0-dev-pic-vfp** runtime. The executable is
`pc/ZZDosBox-Dynrec`; an existing launcher uses it under the name `ZZDosBox`.
This is a runtime update, not a new MUI or Windows package. Keep a copy of the
previous runtime when updating. `baseline/` preserves the measured control.

Doom Demo3: **7.29 → 7.64 FPS (+4.68%)**, two runs per build. No frame skipping.
Requires the existing ZZ9000/XACP environment; tested with XX19c.

[Report and reproduction](../../../docs/performance-pic-vfp.md)

Executable SHA256:
`cccfd75208ea662f0ce8036d8ab1cbccfde8ac2846715ddfd0d338272d5a6aa0`.
No game data, Microsoft software or guest disk images are included. The boot
image is an original regression fixture, not an operating-system installation.
