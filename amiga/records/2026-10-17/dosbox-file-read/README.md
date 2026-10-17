# File-read and scheduling validation

[Report](../../../../docs/amiga-dosbox-file-read-optimisation.md) · [Exact build artifacts](../../../../artifacts/development/runtime-0.22.2)

`physical-results.json` retains the measured counters and pass markers from 18
completed hardware runs, with hashes of the original private logs. It excludes
user media, network endpoints and machine-specific paths. Native sanitizer and
ARM instruction-model tests separately cover cache coherence, mouse transitions
and paging faults/aliases. Hardware results do not establish physical serial
transfer, interactive joystick behaviour or a game-FPS improvement.
