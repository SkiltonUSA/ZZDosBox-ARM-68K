# Elite DOS startup probe

User-supplied Elite DOS executable, 74,576 bytes. Initial direct launch
reported `Packed file is corrupt` and generated no frames. Using the existing
LOADFIX helper with the unchanged game executable removes that message.

On A4000TX / ZZ9000 with the installed 0.5.0-dev prototype:

- `loadfix elite.exe`: 559 frames / 8,000 emulated ms in 12.314 s, **45.40 FPS**.
- All 559 display acknowledgements received; no file writes.
- Checked register/cache restoration and owned-memory release passed.

This is startup throughput, not gameplay speed. The native diagnostic core
reaches the Elite title, ship/dashboard screen and “Press any key to
start game”, at 320×200 / 8 bpp. It matches the hardware slice count (414,840),
file reads (74,362 bytes) and frame count. Native rendering does not establish
physical scanout correctness.

Two separate physical screenshot attempts captured black/cursor content.
The bridge screenshot code reads `scr->RastPort.BitMap->Planes[0]`, while the
player writes P96-locked framebuffer memory and uses XACP PAN. The logged
bitmap-plane and locked addresses differ. These captures therefore do not
establish that the physical monitor is black, nor prove the title is visible.
Visual confirmation on the Amiga is pending. Screenshot-run timings are
excluded from the FPS figure.

Native reproduction: stage the user's executable as
`.context/amiga/dosbox-shell/Games/ELITE/ELITE.EXE`, then run
`python3 scripts/build_dosbox_pc.py --game elite`. This produces the captured
native frame under `.context/`; game data is not included here.

`result.json` records hashes and measurements; physical logs retain startup,
file-service and cleanup evidence. Installed emulator binary is unchanged.
Final state: Workbench, Roadie and bridge available, no Core1 owner.
