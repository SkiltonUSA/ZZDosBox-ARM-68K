# TinyGL on ZZ9000 Core1 — 13 October 2026

Scope: an optional host renderer for ZZDosBox, selected in MUI. It executes a
partial OpenGL fixed-function pixel API in software on the Cortex-A9. It does
not expose OpenGL to the emulated PC, implement GLSL, accelerate guest 3D games,
or use an FPGA 3D engine. All existing Core1 ownership, MMU and RTG grant rules
remain mandatory. Direct remains the default renderer.

Pinned TinyGL revision and the patches are documented in
`amiga/dosbox/opengl/README.md` and the generated `tinygl-provenance.json`.
The original upstream pixel loop omitted source row/column zero and did not
support our top-down negative zoom. The bounded replacement uses destination
pixel centres. Initial code divided for every output pixel. A second version
precomputes 640 horizontal indices once per frame, retaining identical pixels.
All eight input geometries pass an independent integer-centre sampling oracle
under ASan/UBSan, with close/reopen and invalid-geometry checks. The private
patch is restricted to the 640-pixel output path, not a general GL library.

ARM model tests cover EGA, Hercules/amber, the maximum indexed S3 source at
1024×768, two allocation bases and output byte orders, frame cancellation,
firmware denial of SIMD access, all SIMD registers/FPSCR, framebuffer mapping
revocation and a GL-context-closed report. The S3 case peaks at 8,204,032 heap
bytes. The experiment reserves a 10 MiB heap inside the same 32 MiB allocation;
Direct reserves 8 MiB. Linker and mapping checks retain the 4 MiB JIT cache,
stack and page-table separation. No hard-coded DDR allocation is introduced.

Physical baseline TinyGL runs: Hercules 11 frames, 28.458 ms per GL pass;
S3 20 frames, 28.548 ms per GL pass. This is the added software rendering stage,
not whole-frame time or game FPS. Both returned guest success, preserved the
register/MMU/cache/NEON witnesses and released owned memory. A MUI Hercules/
amber OpenGL launch also passed in a 320×240 Workbench window.

The final optimized runs and matched Doom comparison are recorded in results.
Doom uses the existing supplied shareware 1.9 Demo3, 8 MiB guest RAM, 10000
cycles/ms, 4 MiB ARM JIT, NEON, pipelined handoff, Normal presentation, no
scanlines or FPS overlay, and profiling enabled for both renderers. Fresh
DEFAULT.CFG absence is checked. Compare game-tic deltas against the A9 global
timer, not VGA frame counts. One matched pair establishes an observation,
not a general game-performance guarantee. Only closed logs are retrieved.

Horizontal-index precomputation passed the same native/ARM-model checks. Its
physical Hercules run measured 10.824 ms per GL pass (11 frames); S3 measured
12.026 ms (19 frames). These are short mixed text/graphics sessions, so their
averages are not steady-state kernel microbenchmarks. The comparison shows
less work in this version, not a guarantee of the same gain in every game.

Additional ARM-model checks passed window resizing, scanline transitions with
presentation divisor 2, mouse and MPU-401 MIDI. Input-only fixtures complete
before a VGA frame; the initial added GL-report assertion wrongly required a
frame and was corrected to compare render and grant counts, allowing the
zero-frame case and one rendered frame cancelled before its grant. This was
a harness correction, not an emulator failure. The input tests passed again.

## Matched physical Doom result

| Renderer | Game interval | Actual game FPS |
|---|---:|---:|
| Direct | 304.759 s | 7.0023 |
| TinyGL software OpenGL | 451.684 s | 4.7245 |

Both completed Demo3 (2134 game tics), with clean guest exit, JIT witness,
SIMD/register/MMU/cache restoration and owned-memory release. OpenGL was
32.53% slower in this one matched pair. Its GL pass averaged 10.127 ms over
13,008 presented VGA frames and consumed 131.733 s across the whole session;
that session includes startup/exit, so it is not identical to the game interval.
The GL heap peak was 6,082,048 bytes for Doom. There is no performance case for
turning it on for Doom; Direct remains default. This supplies a working bounded
fixed-function rendering foundation, not a game-speed gain or GLSL support.
