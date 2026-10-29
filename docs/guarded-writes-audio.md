# Guarded ARM writes and audio batching — 0.23.0-dev

Two opt-in experiments extend the recorded 32 MiB JIT build. The installed player
and release remain unchanged. Both use 8 MiB guest RAM inside a 64 MiB Exec-owned
allocation, with the existing x87/VFP and SIMD preservation path.

## Guarded writes

`--pc-jit-write` emits an ARM fast path for guest RAM writes. Each execution reads
the live writable TLB entry. Only low-bank addresses with a writable mapping and
aligned word/dword accesses qualify; null mappings, device handlers, high banks,
unaligned accesses and boundary crossings keep the original checked helper.
Code-page write protection still removes the writable mapping, so self-modifying
code goes through its invalidation handler. No guest pointer is cached across
mapping changes. This is CPU access through the software TLB, **not hardware DMA**.

The emitted path keeps address/value arguments intact on fallback, uses scratch
registers, and returns the same no-fault result on success. MOVW/MOVT materialises
the runtime TLB address. It does not delete x86 flags merely because a memory
operation was inlined: later guest branches may consume those flags.

The independent linked ARM model's write fixture produced these helper entries:

| Helper | Reference | Guarded writes |
| --- | ---: | ---: |
| Checked byte write | 4,096 | 0 |
| Checked word write | 4,097 | 2,049 |
| Checked dword write | 4,096 | 3,073 |
| `get_CF` | 8,192 | 8,192 |
| `get_ZF` | 12,289 | 12,289 |
| `FillFlags` | 4,103 | 4,103 |

Fallback counts include intentionally unaligned and video accesses. These are
fixture entry counts, not timing or game-FPS measurements. The unchanged flag
counts are deliberate. Previous adjacent-register store forwarding remains off;
it did not establish a useful gain in the earlier game measurements.

## What the new hot profile shows

A fresh physical Duke opening-level profile again finds repeated texture/pixel
loads and stores, plus signed sample loads, clipping and word stores consistent
with guest audio mixing. This supports testing writable-memory helpers across
both workloads. Block entry counts show frequency, not the time spent per block.
The profile is collected separately from the uninstrumented comparisons.

Carry and zero flags remain live in the sampled loops: `ADC`, transparency
comparisons and conditional loop branches consume them. Removing these
calculations wholesale would change game behavior. Wider register residency
needs a proven model for helper clobbers, partial registers and control-flow
merges; this experiment does not claim to implement it.

Only the first 16 bytes of each ranked block are captured live. Longer
disassembly is labelled as historical snapshot data, even when its first bytes
match. Modified immediates must not be mistaken for verified live instructions.

## Audio scheduling

`--pc-audio-batch` drains the bounded ready PCM span before display work. It copies
up to the available four host slots, publishes completed indices once and wakes
the AHI worker once. Each AHI request remains 1,024 stereo frames at 32 kHz, with
the existing two-request prebuffer. This batches handoffs without increasing
request size or changing the ARM payload. Worker/device errors retain the normal
shutdown path. It cannot compensate for an ARM core producing audio too slowly.

## Physical results

A4000TX / ZZ9000, fixed 10,000 cycles, S3/VESA, 320×200 guest display,
fullscreen adaptive presentation, SB16 sound/music, same Duke 1.3D opening level.
Each sample counts actual game renders over 30 wall seconds after a five-second
settle. The hot-counter run is excluded. The first four runs use reference/write/
write/reference order; a later write-only control and two corrected audio runs
check the host scheduling change.

| Build | Game FPS, individual runs | Mean | Decision |
| --- | --- | ---: | --- |
| Original checked writes | 4.933 / 4.900 | **4.917** | Retain |
| Guarded inline writes | 4.867 / 4.867 | **4.867** | Leave off |
| Later write-only control | 4.867 | 4.867 | Consistent with earlier pair |
| Inline writes + batched host audio | 4.900 / 4.933 | **4.917** | Keep experimental |

Inline writes reduce helper calls in the synthetic fixture but are **1.02% slower**
in this matched game pair. Extra guards and emitted code size may offset call
savings; the measurement does not isolate the cause. Do not promote the change.

Audio batching recovers roughly 1% against the write-only candidate, but does
not beat the original 4.917 FPS mean. Recorded late audio-buffer links remain
around 36–37% of submitted requests. These counters cover the whole run, including
startup; they are not a listening test. Game-clock progress stays around 91% of
wall time. The sample's exclusive profile spends about 75% in CPU emulation,
versus about 3% in final frame copy. Earlier host servicing cannot fix a sustained
producer-side audio deficit.

All seven accepted uninstrumented runs complete queued PCM, restore SIMD/register/MMU/cache
state, exit zero and release owned memory. The initial incorrect audio relink
exits 20 and is retained as rejected evidence below. No release or installed
player is changed. [Raw logs, hashes and model checks](../amiga/records/2026-10-29/guarded-writes-audio/).

## Validation

- Real x86 byte/word/dword writes at 4,096 offsets through linked ARM dynrec,
  at two runtime addresses; partial writes, carries and device access included.
- Self-modifying guest code and checked-read regressions at both addresses.
- Paging aliases, instruction fetch, read/modify/write, nested faults and high-bank
  rejection retain the checked paths.
- x87 and SIMD context restoration; cancellation during pending presentation/copy.
- Audio host test: 50 ASan/UBSan cases for bounded capacity, exact copy/publication
  order, wraparound, corrupt queues and worker errors.
- Existing console ring, presentation ownership and network host reviews pass.

## Reproducing

Use a verified 32 MiB build as the base; each output must be a separate directory:

```sh
python3 scripts/build_dosbox_dos_arm.py --base .context/cache32 \
  --pc-jit-write --output .context/write-test
python3 scripts/build_dosbox_dos_arm.py --base .context/write-test \
  --pc-audio-batch --output .context/write-audio-test
python3 scripts/test_dosbox_jit_write.py .context/write-test
python3 scripts/test_dosbox_host_layout.py .context/write-audio-test
make test-audio-batch test-host-review
```

Build options and source hashes remain recorded. The optional Duke hot-block
counter is diagnostic only; comparisons must use uninstrumented builds with the
same game, settings, scene, frame observer and measurement interval.

## Rejected host build and layout correction

The first host-only audio relink omitted the generated `mmu.h`. Include lookup
therefore selected the common fallback header and its old report/witness
offsets. The game ran, but host teardown read code bytes at `0x82400` instead of
the report at `0x182400`, then exited 20. Its reported phase and mismatch words
exactly match the payload instructions at the wrong offset. The run is excluded
from performance results. The bridge stayed responsive and Core1 was released.

The relink now copies both generated MMU headers. A compile-time host contract
rejects mismatched control/state/report offsets; actual 68k compilation rejects
the incomplete header set and accepts the corrected set. The portable header
lookup regression exercises the same include-selection failure. The ARM payload
is unchanged. This identified build error is separate from the earlier
unexplained NES freeze.
