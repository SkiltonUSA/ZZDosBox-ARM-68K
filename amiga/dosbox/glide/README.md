# Experimental Glide-to-RasterIX translator

This is an experimental **command encoder, offscreen probe and diagnostic guest gateway**,
not yet a working guest Glide renderer.
It translates a small, explicit subset of Glide window-space rendering into
RasterIX packets for the XACP Core0 submission interface. The released
ZZDosBox player and launcher do not use it. No GPU throughput or game FPS
has been measured for this component.

## Architecture

The intended path is a guest `GLIDE2X.OVL` call, a DOSBox interception hook,
the ARM translator, XACP Core0 submission, then FPGA rasterisation. Core1
continues PC emulation. Core0 submits GPU commands; it does not perform the
triangle rasterisation in software.

The encoder itself has no MMIO, guest memory dereferences or display takeover.
A separate 68k probe submits pre-encoded fixtures through the firmware's validated
Core0 service. The standalone probe does not launch Core1. A separate diagnostic
build adds a read-only Core1 witness; neither build changes the display or firmware.

**Physical progress:** clear, solid red, additive green, red/blue texturing and
source-alpha blending passed guarded readback on the A4000TX. Earlier runs
observed a marker before correct pixels; the delayed readback then passed.
Revised probes pass, but the cause of the earlier failure is not isolated.
See the [hardware report and captures](validation/hardware/README.md).
The [marker-isolation follow-up](validation/marker-isolation/README.md) reproduces
the failure without a pre-submit marker read. Fresh colours and triangle markers
also fail in that mode; sixteen prepared runs pass. The underlying cause remains
unresolved, so no production fence or guest-game support is claimed.
The [Core1 witness comparison](validation/core1-witness/README.md) records one
scene with correct ARM readback and 35 incorrect 68k pixels. Idle and delayed
observer controls also pass, so visibility and timing still need isolation.
The [preparation controls and first-read trace](validation/preparation-controls/README.md)
add 80 physical runs: unrelated reads still fail, while wait controls pass.
Traced incorrect pixels retain previous-scene colours. No production wait or
completion-fence fix has been promoted.
The [ordered-observer tests](validation/observer-order/README.md) add 45 completed
runs and one interrupted run. Waiting for ARM verification passed 12/12 runs;
concurrent and reverse readback still fail intermittently. Hardware testing is
paused after the interrupted run; its cleanup was unverified until reboot.
After recovery, [early pixel sampling](validation/early-samples/README.md) captured
new ARM pixels alongside previous-scene host pixels in two of four concurrent
runs. Four ordered controls passed. All eight runs cleaned up successfully;
this remains diagnostic evidence, not a production fix.
[Short ARM acknowledgements](validation/early-gates/README.md) also passed their
completed controls, including publication before interior pixel reads. A later
run lost connectivity. Reboot recovery and a single direct-output check passed,
but repeated-run stability remains unresolved.

The [upstream fence audit](validation/fence-audit/README.md) identifies a newer
RasterIX command that waits for framebuffer and texture transactions. Its parser
passes simulation checks, but our inspected XACP ABI rejects that opcode. It is
an integration candidate, not an installed fix or an explanation of the freezes.

## Implemented subset

- Diagnostic DOS/4G `GLIDE2X.OVL`, with bounded guest argument decoding.
- Optional Glide 2 vertex decoder and perspective-preserving viewport clipper.
- In-bounds window-space triangles, either winding; degenerate triangles skipped.
- Upper-left/lower-left origins, interpolated RGB and alpha.
- One RGB565 texture mip level, nearest filtering, texture × vertex colour.
- Perspective-weighted TMU0 coordinates; rectangular texture aspect ratios.
- Opaque, source-alpha and additive colour blending state.
- Big-endian packets, limited to 248 words including the envelope and commit.
- One state per batch; triangles remain in their original order.
- Explicit unsupported-depth and needs-clipping results, with no silent fallback.
- Bounds checks against the published GPU pool; texture/framebuffer overlap rejected.

Texture bytes must already be uploaded into contiguous 2 KiB pages in the
published GPU pool. The encoder neither allocates nor uploads these pages.
It rejects non-power-of-two textures, dimensions above 256 and aspect ratios
outside 8:1–1:8. The initial viewport limits are 1024×768 and all vertex
positions must lie in the pixel-centre range 0..width−1, 0..height−1.

`grx_begin()` starts a packet; repeated `grx_triangle()` calls append draws;
`grx_end()` writes the envelope and colour commit. `GRX_FULL` means finish
the current packet and retry that triangle in a new batch. Other rejected
triangles leave the packet unchanged. Callers must handle every result.

## Limits that matter for games

The inspected **Core0 submit ABI 1.0** deliberately rejects depth/stencil
registers and the depth-test enable bit. RasterIX hardware support and direct
Core1 library support do not imply permission through this interface. A
depth-capable Core0 path needs a separately reviewed firmware API extension,
or a different supported submission route. This encoder never bypasses it.

The ABI pool is `0x06600000..0x067F0000` (exclusive end), separate from
staging. A future transport must query capabilities, obtain exclusive GPU
use and manage the actual target/texture lifetimes. These constants are the
published GPU aperture, not invented shared Exec allocations.

**XACP DONE/FIFO transmission completion is not GPU retirement.** Colour
COMMIT alone is not a CPU-visible completion fence either. The transport
must implement and verify a bounded completion-marker/fence sequence before
CPU readback, resource replacement or display presentation. Two trailing NOPs
follow the reference FIFO pattern; they do not establish completion.

Further work before launching a game:

1. Resolve the earlier completion-marker failure; extend offscreen tests to
   repeated batches, texture replacement, shared edges and cancellation.
2. Connect the diagnostic OVL gateway to a complete rendering/state transport.
3. Compare the clipper's pixel-centre and edge rules against Glide rendering.
4. Depth and W-buffer semantics, texture formats/palettes/mipmaps, additional
   combiners, alpha test, fog, culling and linear-framebuffer locks/readback.
5. A suitable Glide game's startup, frame correctness, sound and clean exit;
   then matched performance measurements.

The descriptor retains librixgpu's unbiased edge rule: pixels exactly on a
shared edge can be drawn by both triangles. Blended shared edges therefore
need physical validation before promotion. No conformant Glide coverage rule
is claimed. General OpenGL and software-rendered DOS games are not accelerated
by this encoder.

## Reference guide and credits

The supplied [3Dfx Glide 3.0 Programming Guide, June 1998](http://bitsavers.informatik.uni-stuttgart.de/components/3dfx/Glide_Programming_Guide_3.0_199806.pdf)
is useful for state, coordinate and blending semantics. Its configurable
vertex layout is not the DOS Glide 2 binary ABI. The guest decoder translates
the 60-byte Glide 2 vertex into the encoder's explicit input structure.
Global `oow` is used by default; the TMU0 W-difference hint selects its own
`oow`. The caller must supply that state correctly.

Window-space TMU input is `s/w`, `t/w`, `1/w`. The longest texture axis has
coordinate extent 256 and the shorter axis scales with aspect ratio. The
encoder normalises the preweighted coordinates directly, avoiding a divide
and multiply by `1/w` at each vertex. Colours are normalised from 0..255.

Triangle setup is derived from **Xanxi's 0BSD librixgpu `rixdesc.c`**, retaining
its S.5 edge calculations and 39-word descriptor. The adaptation supplies
geometry explicitly, removes mutable globals and accepts Glide preweighted
texture coordinates. The original file's SHA-256 is
`5aaef9675b2218c201a5846c316d5e5f2e6d6001db84d230e475d197895e4c68`.

References:

- [Xanxi's librixgpu](https://github.com/Xanxi-Amiga/XACP-ZZ9000/tree/main/applications/games/ZZQuakeGL/source/ZZQuakeGL/librixgpu)
- [XACP GPU submit header](https://github.com/Xanxi-Amiga/XACP-ZZ9000/blob/main/sdk/zz_rasterix_submit.h)
- [Inspected firmware source](https://github.com/Xanxi-Amiga/XACP-ZZ9000/blob/5516effbad59acf6bef44ea1cc3ca4393c8ec9e0/firmware/source/XX19d-RIX/arm/ZZ9000_proto.sdk/ZZ9000OS/src/main.c)
- [RasterIX](https://github.com/ToNi3141/RasterIX)
- [DOSBox-X Glide interception](https://github.com/joncampbell123/dosbox-x/blob/master/src/hardware/glide.cpp)
- [OpenGlide](https://github.com/voyageur/openglide), a useful translation reference;
  no OpenGlide code is copied into this prototype.

## Validation

```sh
python3 scripts/test_dosbox_glide.py --arm --firmware /path/to/XACP/firmware/source/XX19d-RIX/arm/ZZ9000_proto.sdk/ZZ9000OS/src/main.c
# Optional, using Python with Unicorn installed:
python3 tests/amiga/dosbox/glide/arm_model.py
python3 scripts/test_dosbox_glide_guest.py
python3 tests/amiga/dosbox/glide/clip_model.py
```

Native tests use Clang with AddressSanitizer and UndefinedBehaviorSanitizer.
The ARM compile step needs `arm-none-eabi-gcc`; the instruction model also
needs its `nm` and `objcopy` tools. Generated files stay under `.context/`.

The firmware test extracts and compiles the **unchanged upstream validator**
from the supplied source, rather than duplicating its checks. The reference
results are in [validation/](validation/). These checks do not establish
physical GPU behaviour or instruction timing.

To build the separate 68k hardware probe (does not install or run it):

```sh
python3 scripts/build_dosbox_glide_probe.py
```

This uses the pinned Amiga cross-toolchain container and native Clang to generate
wire fixtures, so the 68k executable needs no FPU. The build record includes
source, fixture and executable hashes. Hardware execution requires exclusive GPU
access, an idle Core1 and the verified XACP GPU submit ABI. Read the report before
running it; a timeout leaves completion unknown and must stop further GPU tests.

## Additional reference

[Glidulator review and GTA archive findings](glidulator-review.md): ABI,
texture lifetime and capture/replay ideas. The MIT ABI table is vendored;
its renderer and worker implementation are not imported.

## Diagnostic guest gateway

The [GTA ECTS loader test](validation/guest-loader/README.md) reaches our OVL's
`grGlideInit`, `grSstQueryHardware` and shutdown entry points. The query returns
false deliberately: no GPU is advertised. The result proves DOS/4G loading and
argument delivery in the native harness, not playable GTA or ARM performance.
The [timed original-overlay comparison](validation/timed-gta/README.md) reaches
the same no-board failure and credits with both overlays. Normal DOS exit is
still unverified; bounded harness termination must not be called a clean game exit.

```sh
python3 scripts/build_dosbox_glide_ovl.py --watcom /path/to/openwatcom
python3 tests/amiga/dosbox/glide/ovl_model.py
python3 scripts/build_dosbox_pc.py --compat --x87 --expanded --glide-diagnostic --game fixture
```

Open Watcom's Linux x64 `wasm` and `wlink` run inside the pinned build container.
The generated OVL and call table remain under `.context/amiga/glide/ovl`.
Use a separate copy of game files when replacing an OVL. The diagnostic module
exports 146 entry points; that count is ABI coverage, **not implemented Glide
functionality**. The bridge uses private emulated ports, not physical host I/O.
It is separate from DOSBox-X's Glide protocol.

`grx_clip_triangle()` rejects fully outside triangles and copies fully inside
triangles without running the four clipping passes. Crossing triangles use
double intermediates to avoid overflow with extreme finite guest coordinates.
Clipping interpolates `s/w`, `t/w` and `1/w` directly. These design choices avoid
unnecessary work, but there is no measured Cortex-A9 speedup yet.
