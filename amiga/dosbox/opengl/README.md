# A9 software OpenGL experiment

The optional **OpenGL (experimental)** renderer uses a pinned
[TinyGL](https://github.com/jserv/tinygl) fixed-function API subset on the
ZZ9000 Cortex-A9. **Direct** remains the default renderer.
Both choices remain available in Settings and are saved per program.

**Current source experiment, not installed:** direct-target TinyGL measured
5.06 FPS against 6.94 FPS Direct in this session. During a later loop refinement's
first hardware test the Amiga froze, despite that build passing model checks;
it was withdrawn and source restored to the first tested variant. The exact
failure cause is unresolved. The installed two-stage renderer is unchanged.
[Results and failure record](../../records/2026-10-14/dosbox-gl-direct/method.md).

This is host-side presentation, not a DOS/Windows OpenGL driver, a 3dfx/Voodoo
emulator or an FPGA-accelerated graphics driver. TinyGL is not a conformant
complete OpenGL implementation. It has no GLSL compiler, programmable shaders,
or support for copying DOSBox Staging's shader files into this application.

Selected frames pass through `glViewport`, projection/model-view setup,
`glRasterPos2f`, `glPixelZoom` and `glDrawPixels`. The current source adds a
direct target for a validated 640×480 RTG grant with scanlines disabled.
TinyGL writes into that temporary mapping, handling pitch and pixel byte order,
then restores its private buffer pointer before the caller unmaps or unwinds.
The FPS overlay is drawn afterward. Context allocation happens before requesting
a grant; direct rendering polls the shared stop word once per row. No firmware
display register is written by TinyGL.

Resized windows and active scanline masks retain the private 640×480 buffer and
existing ARM copy. This preserves their previous scaling/mask semantics.
Window scaling and XACP buffer flipping are retained.
No 68K round trip for pixels and no firmware modifications are introduced.
The additional colour/depth buffers cost about 1.76 MiB plus context allocations;
they remain inside the bounded 10 MiB experimental worker heap inside the same 32 MiB allocation
(Direct retains its 8 MiB heap). The linker still enforces separation from
the 4 MiB JIT cache and stack.
It is software work on the same A9 that emulates the PC, so performance must be
measured; selecting OpenGL does not imply a speed improvement.

The private buffer remains allocated for fallback, so this change removes a
render/copy pass on eligible frames, not the heap reservation. The shutdown
report counts direct and fallback attempts separately. Cancellation during a
direct write detaches the borrowed pointer and revokes the mapping before the
normal cooperative unwind. Native tests cover both RTG byte orders, padded
pitch, guard bytes and cancellation; ARM tests cover invalid nonce rejection,
relocation, resize transitions, scanlines and mapping revocation.

`prepare_dosbox_opengl.py` stages revision
`c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a` and its license. Generated upstream
sources stay outside the repository. Our private build replaces TinyGL's
`glDrawPixels` loop to include source row/column zero, support negative zoom
for top-down frames and sample downscaled images without overlapping writes. Horizontal source
indices are calculated once per frame, avoiding per-pixel floating division.
It also fixes an undefined signed shift in the upstream compatibility check.
Custom allocation uses the owned heap and the existing failure unwind; GL
cleanup runs before the firmware SIMD/FPSCR state is restored. OpenMP is off.
This patch has only been tested for our bounded packed-RGB pixel path, not as
a general-purpose replacement OpenGL library.

Run `python3 scripts/test_dosbox_opengl.py` for native sanitizer/pixel checks.
The ARM build adds `--pc-opengl` to the 4 MiB JIT, NEON, pipeline, scanline,
presentation and adapters build. Build metadata records the pinned source.

The [Staging shader guide](https://www.dosbox-staging.org/0.83/manual/graphics/rendering/shaders/)
requires OpenGL output. TinyGL provides a starting point for testing software
rendering costs, but cannot run those GLSL effects. Full shaders would require
another renderer/compiler integration and separate memory/performance work.

Before the direct-target experiment, a matched Doom Demo3 run measured **7.00 FPS Direct** and
**4.72 FPS TinyGL**, with Normal presentation and 10000 cycles/ms. Both exited
cleanly. Keep Direct for speed; the software OpenGL path is for development.
[Measurements and method](../../records/2026-10-13/dosbox-opengl/method.md).

The [triangle rasterisation review](../../../docs/amiga-tinygl-rasterisation.md)
distinguishes possible future geometry work from this completed-image pixel path.
