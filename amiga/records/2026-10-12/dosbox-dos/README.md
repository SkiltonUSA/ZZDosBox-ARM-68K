# DOS kernel hardware test — 12 October 2026

ZZDosBox 0.2.0-dev passed on the A4000TX / ZZ9000 Core1 using XX19c/XACP.
External original COM and MZ EXE fixtures exercised real DOSBox EXEC, file
handles/read/write/seek/EOF, relocation and DOS memory allocation. The 256-byte
output retrieved from the SD card matches the input exactly. Both guests
exited 0; cancellation, state restoration and memory release passed.

`hardware.log` is the physical report. `result.json` records the independent
output comparison; `build.json` identifies the binary and build sources.
`native-asan.log` and `arm-model.log` cover local checks, not physical speed.
`postflight.log` shows the bridge and Workbench after exit. Its first log pull
timed out; the subsequent CRC-verified retry is recorded in `log-retry.log`.
No FPS or PC-equivalent speed measurement is claimed.
