# ZZDosBox 32 MiB cache experiment


The new `--pc-jit-mib 32` profile reserves a 32 MiB executable cache inside a
64 MiB Exec-owned allocation. It limits emulated PC RAM to 8 MiB. Guest RAM,
code, tables, heap, buffers and the executable cache must all fit; the existing
4 MiB-cache profile still permits up to 32 MiB guest RAM.

Translation metadata is budgeted separately: 65,536 blocks and 512 code-page
handlers, with a 12 MiB private heap. Those limits can cause recycling before
all 32 MiB of code space is occupied. Cache capacity alone is not a speed claim.
The hardware x87 fixture used about 7.63 MiB of heap.

Build the expanded profile with the existing expanded flags and
`--pc-jit-mib 32 --output .context/cache32`. A subsequent build can use:

```
python3 scripts/build_dosbox_dos_arm.py \
  --base .context/cache32 --output .context/cache32-rows --pc-row-cache
```

`--base` verifies recorded artifacts and the upstream pin, inherits build options,
and requires the same product version and a separate output directory. Explicit
flags override inherited settings and are recorded. It rejects older manifests
without build options. It does not restore modified source files: both source
hashes and flags are recorded for review.

The player, launcher and generated welcome text share `amiga/dosbox/version.h`.
The experimental product version is **0.23.0-dev**. The matching MUI launcher
and optional row-cache player also compile successfully. x87 already uses the pinned
portable core with A9 VFP-backed binary64 arithmetic; this is not full 80-bit x87
precision. The recompiled x87 and recompiler use the same recorded core and
softfp/VFP flags, including disabled FP contraction.

Physical A4000TX validation of the initial 32 MiB build passed:

- 64 MiB owned allocation and runtime mapping challenge.
- 33,554,432-byte cache report and rewritten-code execution witness.
- x87 arithmetic, rounding and saved-state fixture.
- SIMD/FPSCR restoration, MMU/register/cache witnesses, exit 0 and memory release.

The final build also corrects an old welcome-text version. Its instruction-model
x87 checks pass at two load addresses. Native layout checks cover allocation
bounds and the 8 MiB guest limit. The final banner/version build also passed the physical x87 fixture.

JIT regression checks also pass: checked reads, register reuse, self-modifying
code invalidation, paging fallbacks and unsupported guest-address rejection.
These are instruction-model checks, separate from the physical runs above.

### Converted rows

`--pc-row-cache` is an opt-in, per-frame cache for repeated source rows during
vertical enlargement. It keeps the converted row in ARM cached RAM, then writes
it to the granted bitmap. It never reads framebuffer memory. The key includes
source, geometry, byte order and brightness; every frame invalidates the cache.
No reuse occurs for 1:1 vertical rendering or downscaling.

Native sanitizer tests compare exact pixels across sizes, byte orders and dimming.
Linked ARM tests cover resized output, soft/gradient scanlines and cancellation
while writing the framebuffer. Extra copying may outweigh conversion savings;
this option remains disabled by default. In two physical EGA-fixture runs per
variant (forward/reverse order), average copy time per displayed frame was
2.571 ms with the 4 MiB cache, 2.510 ms with the 32 MiB cache, and 3.471 ms
with converted rows. The row candidate was about 38% slower than the 32 MiB
baseline in this 320x400-to-640x480 mode. Runs presented 17–18 frames; these
are diagnostic copy costs, not game FPS or a controlled cache-capacity gain.
CPU totals for 4/32 MiB averaged 476.81/479.91 ms, with substantial run variation.
No cache-size speed improvement was established.


## Direct DMA status

The PL330 program generator and read-only physical survey are documented in
[the DMA investigation](amiga-dosbox-direct-dma.md). No hardware transfers have
been submitted. Current XACP interfaces do not provide a DMA-channel lease and
cancel/quiesce contract. An idle channel does not prove exclusive ownership.
Resetting Core1 does not stop independent DMA, so existing teardown cannot safely
free an allocation while a transfer may still reference it. A controller/channel
ownership agreement and confirmed cancellation are required before enabling DMA.
Direct ARM framebuffer writes already work; they are distinct from DMA.

[Build manifests and test logs](../amiga/records/2026-10-28/cache32/).
