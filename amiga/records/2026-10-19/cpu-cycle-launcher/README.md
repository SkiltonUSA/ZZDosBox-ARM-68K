# CPU-cycle launcher validation

Launcher 0.22.6-dev adds 60000 and 100000 without renumbering earlier choices.

- Native library/profile and session round trips cover all six choices and reject
  an out-of-range index.
- Configuration tests check high-choice Expanded routing, global override
  precedence, network/OpenGL/interpreter restrictions and the existing 10000 path.
- M68k launcher build passed with warnings as errors; MCP smoke checks passed.
- Executable installed after backing up the previous launcher, with MD5 verified
  before and after replacement. Launcher.cfg was byte-identical across replacement.
- Updated launcher started successfully on physical hardware. Both 60000 and
  100000 were selected and visually verified on the CPU page. The previous 10000
  choice was restored; no high-cycle profile was saved.

Physical ARM 100000-cycle gameplay samples and teardown witnesses are in
[the Duke evidence](../duke-optimisation); no runtime was rebuilt for this UI change.
