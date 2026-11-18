# Experimental Glide-to-RasterIX translator

This is a standalone **command encoder and offscreen probe**, not yet a guest Glide driver.
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
Core0 service. It does not launch Core1, change the display or modify firmware.

**Physical progress:** clear, solid red, additive green, red/blue texturing and
source-alpha blending passed guarded readback on the A4000TX. Earlier runs
observed a marker before correct pixels; the delayed readback then passed.
Revised probes pass, but the cause of the earlier failure is not isolated.
See the [hardware report and captures](validation/hardware/README.md).

## Implemented subset

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
2. Guest Glide 2 OVL interception and bounded guest-memory marshalling.
3. Viewport clipping and pixel-centre/edge-rule comparison against Glide.
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
vertex layout is not the DOS Glide 2 binary ABI. This component uses a small
explicit input structure; a future OVL decoder must translate the actual
Glide 2 fields into it.

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
