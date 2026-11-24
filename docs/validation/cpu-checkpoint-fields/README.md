# CPU checkpoint field validation

Development-only real-mode integer CPU component. Native ASan/UBSan tests:

- 200 instruction-boundary restores reproduce CPU and diagnostic RAM.
- CPU/RAM fixture resumes in a fresh process.
- 20 valid-CRC malformed records reject before changing live state.
- Both DF/string-direction configurations restore correctly.
- Existing byte corruption, truncation, extra-data, architecture and unsupported-context checks pass.

[Results](results.txt). No production player, installer, firmware or hardware test
changed. This does not implement complete game save states or paged-JIT acceleration.
