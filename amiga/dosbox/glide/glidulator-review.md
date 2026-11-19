# Glidulator reference review

Reviewed [ElTentakel/Glidulator](https://github.com/ElTentakel/Glidulator/tree/8629ed8b6559654ddd0bfb3df2f2ed9e8ec8bcfc)
at commit `8629ed8b6559654ddd0bfb3df2f2ed9e8ec8bcfc`. The MIT ABI metadata is
now vendored in [abi/](abi/); no renderer or worker implementation is imported.

Useful candidates, in implementation order:

1. **Glide 2 ABI and OVL entry points.** Its generated call tables and structure
   definitions help define the guest boundary. The ABI attribution credits
   The Nulph's MIT-licensed MGA-Glide reconstruction; implementation coverage
   must be checked independently of export names.
2. **Capture/replay and image fixtures.** Recorded calls can exercise state,
   textures and framebuffer access before attempting a complete guest game.
3. **Texture lifetime.** Cache decoded textures, invalidate overlapping uploads,
   account for palette/NCC changes and wait before reusing in-flight entries.
4. **Explicit synchronization points.** Its software workers drain at swaps,
   clears, framebuffer locks/writes and some texture reuse. These are useful
   semantic boundaries, but its CPU-worker completion is not a ZZ9000 GPU fence.

The rasterizer derives from Sarah Walker's PCem and the 86Box contributors
under GPL-2.0-or-later. Its x86/SSE2 pixel JIT and DOS AP/Windows worker startup
are not drop-in Cortex-A9 code. A software second TMU does not provide a second
FPGA TMU. Its PC performance figures cannot predict ZZDosBox frame rates.
Our submit-ABI depth rejection and unresolved completion/readback behaviour
remain integration gates.

## First game candidate: GTA 3dfx demo

The upstream [compatibility table](https://github.com/ElTentakel/Glidulator/blob/8629ed8b6559654ddd0bfb3df2f2ed9e8ec8bcfc/docs/compatibility.md)
reports `DEMOFX.EXE` with `guTex*` and LFB locks. This makes GTA a useful
future integration target after a small controlled Glide fixture. It is not
currently verified in ZZDosBox through RasterIX.

The supplied `gta8.zip` contains `GTADOS/DEMO8.EXE`, `K.EXE`, DOS/4GW,
Miles sound drivers and game data. It contains no `DEMOFX.EXE` or `.OVL` file.
Its README identifies the 8-bit/24-bit demo variants; the shared help file
also discusses a 3dfx edition. Some 3dfx error strings in DEMO8 alone do not
establish an accessible dynamically linked Glide renderer. Keep this archive
as the software-rendered comparison.

The subsequently supplied **GTA ECTS** folder contains `GTADOS/COMPFX.EXE`
and the original `GLIDE2X.OVL`. In a separate working copy, the experimental
diagnostic OVL now loads and receives the hardware query. An S3/VESA native
test reaches the competition build's menu; standard VGA instead fails its
VESA requirement. The diagnostic reports no board and the game reports that
query failure. See the [loader evidence](validation/guest-loader/README.md).
No GTA ARM/FPGA execution or FPS measurement is claimed. Game binaries remain
outside this repository.

Original Voodoo Graphics Tomb Raider and Carmageddon embed Glide and cannot
be accelerated simply by replacing an OVL. Tomb Raider's separate Voodoo Rush
patch is a documented exception, with its own board-selection and DOS/CD
compatibility caveats. This distinction corrects the initial broad game list.
