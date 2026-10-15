# Console synchronization candidate

A correctness candidate, tested from Amiga RAM without replacing the installed
runtime. It shares console ring helpers, synchronizes occupied spans only and
uses the common three-buffer plan. CacheClearE requests are cache maintenance,
not bulk Zorro reads; no bandwidth saving or FPS increase has been measured.

Native sanitizer tests, component builds and relocated ARM console model checks
passed. The physical run reproduced the 120-line fixture, 5,932 transcript bytes
and hash `6d85b2f0`, with clean state restoration and owned-memory release.
Baseline/candidate elapsed times were 19,918/20,534 ms in single runs. Worker
provenance differs, so this is not a controlled host-only performance comparison.

The measured default remains [pic-vfp](../../development/pic-vfp/).
See [the report](../../../docs/amiga-dosbox-optimisation-round2.md#console-synchronization-and-review-cleanup)
and accompanying build metadata, verification, logs and fixture.
