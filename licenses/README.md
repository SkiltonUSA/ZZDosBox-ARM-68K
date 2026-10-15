# Notices

DOSBox/libretro is GPL-2.0-or-later; see DOSBox-COPYING and the bundled upstream source. Shared Amiga-MCP-Debugger code uses GPL-3.0-or-later, so combined distributions use GPL-3.0-or-later. GCC list support retains GPLv3 plus the GCC Runtime Library Exception. Picolibc retains its component notices in the source archive and COPYING.picolibc. The supplied DOSBox icon is owner-provided artwork; no new licence is asserted for it.

Credits: DOSBox team and libretro contributors; Xani/Xanxi's XACP framework and ZZPicoDrive execution model; Stefan Stuntz for MUI. Development: Codex assisted, Amiga-Bridge MCP. MUI, XACP firmware, commercial games and Microsoft DOS are not bundled.

TinyGL: pinned jserv source is retained in `sources/tinygl-c2e4859.tar.gz`; see
`TinyGL-LICENSE`. Credits: jserv, Fabrice Bellard, C-Chads and TinyGL contributors.
Experimental raster, matrix and compiler work is documented separately from
the installed Direct runtime.

Networking: libslirp 4.9.5 sources and per-file notices are preserved in
`sources/libslirp-4.9.5.tar.bz2` (BSD-3-Clause and component notices).
The pinned DOSBox-X/Bochs-derived NE2000 sources are in `sources/ne2000/`
with their original LGPL-2.0-or-later headers. Portability transformations
are recorded in `scripts/prepare_dosbox_network.py`.
Optional mTCP and Crynwr packet-driver binaries are supplied separately by
the user through the hash-checked staging script; their notices are retained
when staged. They are not included in this development artifact.

Research math dependencies: hlsl++ (MIT, `HLSLPP-LICENSE`) and ARM CMSIS-DSP
(Apache-2.0, `CMSIS-DSP-LICENSE`). Pinned versions and the ARM32 hlsl++ patch
are recorded with the matrix experiments. Neither is a production DOSBox
dependency. The Simd library was reviewed as a reference, not imported.
