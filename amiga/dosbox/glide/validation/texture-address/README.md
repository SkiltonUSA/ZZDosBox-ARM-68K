# Carry-free sampler candidate

A side-by-side comparison of the original sampler and four carry-free address
expressions passes 262,144 cycles across 4,096 dimension/LOD combinations,
including stalls, wrap/clamp and half-pixel offsets. A changed-address mutant
fails. The full-renderer regression also passes, including error/quarantine
and outstanding-command detection. These are digital simulations, not timing
closure or hardware FPS. The candidate remains separate from the routed board.

Run `scripts/test_glide_texture_address.py --renderer /path/to/RasterIX`.
The change is retained as `experimental/texture-address.patch`; original and
candidate source hashes are in `results.json`.

The original address calculation adds the mip-level offset to the packed
texture coordinates. The offset mask clears the coordinate bits, so these
operands cannot share a set bit: `offset & coordinate == 0`. Consequently,
addition and bitwise OR produce the same address. Replacing the four additions
removes their carry propagation without adding pipeline stages or changing
handshakes. The exhaustive field check includes all encoded dimensions/LOD
values; the side-by-side RTL test also checks output latency and stalls.

The carry-free candidate also passes the generated HP3 converter in XSim:
10 cases, 32 verified framebuffers, delayed memory responses, cancellation,
error quarantine and the premature-completion mutation. See [HP3 results](hp3/hp3-results.json).

Full-board synthesis and an independent checkpoint audit passed with real
exit codes of zero. The design uses 23,308 LUTs, 29,100 registers, 60.5 BRAM36
equivalents and 22 DSPs. Compared with the corrected reference synthesis, this
removes 60 LUTs and 20 registers; it is not a speed measurement. See
[synthesis evidence](synthesis/result.json).

Debug-core implementation and optimization are complete. An independent Vivado
process reopened the optimized checkpoint and passed with exit zero, with no
remaining black boxes. It uses 27,439 LUTs, 35,544 registers, 80.5 BRAM36
equivalents and 22 DSPs including debug instrumentation. DRC findings, CDC
summary groups and clock definitions match the reviewed reference. Existing
constraint/CDC warnings remain; this is not timing signoff. The original
optimization wrapper lost its exit status, which is recorded as unknown rather
than inferred from its completion marker. The independent audit and both
debug-core runs have captured successful exits. See
[optimization evidence](optimization/result.json). Routing resumed from the saved placement after the Mac restart and completed
with exit zero. All 55,899 nets route without errors. Declared setup, hold and
pulse-width constraints pass: +0.055 ns setup and +0.011 ns hold, with zero
failing endpoints. DRC, methodology and CDC summary findings match the reviewed
reference. Bus-skew reports have no violated entries, but retain ten NA rows;
unclocked/unconstrained paths also remain. This is not complete timing signoff
or hardware validation. See [routed evidence](routing/result.json). Bitstream
generation is in progress; no physical GPU FPS result exists.
