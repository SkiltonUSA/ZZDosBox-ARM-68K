# Expanded 0.22.5-dev

Incremental development update for an existing ZZDosBox installation. With the
player closed, back up and replace `ZZDosBox-Expanded` in the application drawer.
Retain the existing launcher, configuration and support files. This is not a
complete installation or a new tagged release.

Guarded inline ARM reads improved Duke's measured opening-level rendering from
4.53 to 4.67 FPS (+2.9%) at 10,000 cycles in repeated windowed samples. Device,
high-bank and unaligned reads retain the checked fallback. Writes and
self-modifying-code tracking are unchanged. The ordinary player contains no
Duke frame observer or timed auto-exit.

Memory, CPU, paging, self-modification and x87 instruction-model checks passed.
See [physical results and limits](../../../docs/amiga-dosbox-duke-arm-optimisation.md).
Audio continuity is still limited; a completed AHI buffer is not a listening test.

Build with `make build-expanded`. `build.json` identifies the upstream pin,
compiler, flags and output hashes; `SHA256SUMS` identifies this executable.
Source overlays, tests and licence notices are retained in the repository.
[Requirements](../../../docs/requirements.md) remain unchanged: the TF4060
identifies the test machine, not a required accelerator model.
