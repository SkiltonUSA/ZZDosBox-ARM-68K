# Descent level-start verification

DOS/32A 7.35 external loader; 16 MiB guest RAM, 386, EMS, 6,000 cycles,
Expanded ARM runtime. Original game executable unchanged.

- First mine rendered, keyboard movement changed viewpoint.
- No-sound and SBPro2 configurations both reached the level.
- Audio run: 3,356,672 samples queued/submitted/completed; error=0.
  Delivery counters are not a listening test.
- Both runs: VMM:0, zero paged fallback count, F10 exit, register mismatch=0,
  cache witness=0 and owned-memory release.
- No game FPS derived from callback counters; no complete-game claim.
- Separate JIT speculative invalid-opcode fixture passes at two ARM relocation
  bases and physically. Earlier invalid DOS32A upload trials are excluded.
- Initial ARM suite attempt collided with another native test's localhost port;
  the isolated repeat passed. Original physical JIT fixture needed bounded Core1
  reset; only the corrected fixture passed the normal restoration epilogue.

The two production launcher commands were updated with a verified read-back;
all other profile settings stayed identical. Original game assets/configs/saves
were preserved during installation. Normal gameplay can update pilot files.
