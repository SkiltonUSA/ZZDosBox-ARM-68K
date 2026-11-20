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

The carry-free candidate also passes the generated HP3 converter in XSim:
10 cases, 32 verified framebuffers, delayed memory responses, cancellation,
error quarantine and the premature-completion mutation. See [HP3 results](hp3/hp3-results.json).
