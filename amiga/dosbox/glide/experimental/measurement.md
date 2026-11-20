# Completion-driven GPU counter

The experimental firmware and `ZZGlide-Measure` now pass physical GPU runs of
1, 10, 100 and 1,000 frames. The largest batch measured **9,302.194 offscreen FPS**
at 800 MHz ARM, with pixel checks and clean CLOSE. This is not GTA or display FPS.
[Results and raw logs](../validation/measure/hardware/README.md). The build remains
experimental and is not included in the release.

The first measurement is deliberately small: a 64 × 72 RGB565 offscreen target
with a solid triangle. Five correctness scenes first check clearing, solid
colour, additive blending, texturing and alpha blending. The timed phase repeats
the solid scene for 1–1,000 frames, then checks pixels again. This measures batch
throughput including Core0 preparation, submission and polling. It is neither
full-resolution gameplay FPS nor display refresh rate.

Each frame carries a unique epoch/sequence token after the renderer fence.
Only a matching retired result, successfully consumed and followed by a clean
mailbox status, increments the counter. FIFO transmission, cancellation and
timeouts do not count. Errors retain GPU ownership until recovery; an elapsed
deadline does not prove DMA has stopped. The host prints the last valid reply
on failure, including state, error/detail codes, completed frames and checked
pixels. A last reply is not a fresh status observation after a timeout.

The host calibrates the ARM global timer against `timer.device` before and after
the run. It rejects a change exceeding 1%, and uses integer arithmetic so the
68k host does not need an FPU. Successful runs print completed frames, elapsed
ticks and `GPU_COMPLETED ... FPS=...`, followed by the scope of that number.

## Firmware integration

Private opcode `0x7a20` is confined to this candidate. It provides QUERY, START
and CLOSE; it does not alter the published XACP capability mask or the existing
submit validator. START queues work for the Core0 idle loop. The interrupt-side
reply uses a consistently published snapshot. Legacy GPU submissions and clock
transitions are excluded while the measurement owns the GPU. A successful
CLOSE requires completed pixel checks; a fault cannot be closed as success.

The builder starts from the recorded firmware containing AY, Paula, Copper and
retained-clock services. It preserves them, verifies the base hashes and linker
allocation limit, and writes a separate output tree. It neither flashes the
board nor changes its clock. The linked candidate has 56,068 bytes remaining
below the low-firmware limit.

## Guarded completion accesses

The Cortex-A9 specifies synchronous external aborts for Strongly-ordered
accesses. This is narrower than the general ARMv7 rule allowing asynchronous
aborts for some other memory types. See section 6.4.2 of the
[Cortex-A9 technical reference manual](https://documentation-service.arm.com/static/5e8e27fcfd977155116a63ee).

The transport checks Core0, the recorded A9 revision, ARM exception state,
vector base, translation-table base and exact Strongly-ordered identity mapping
for the completion aperture. It does not change the live MMU. Two assembly
access instructions have dedicated failure-return labels. A small BSP hook
recovers only a precise external abort at those instructions with matching
address and read/write direction. Unrelated exceptions keep the original BSP
handler. The hook uses only core registers and restores the original exception
frame and IRQ/FIQ mask.

This handles a returned `SLVERR`/`DECERR`; it cannot recover from an interconnect
that never responds. Normal-path retirement, Core0 pixel visibility and cleanup passed with the
matching image. Physical error injection remains untested.

## Reproduce offline checks

```sh
python3 scripts/test_dosbox_glide_mmio.py --vectors /path/to/BSP/asm_vectors.S
python3 scripts/test_dosbox_glide_measure.py --host
python3 scripts/build_glide_measure_firmware.py \
  --base /path/to/recorded-copper-firmware \
  --toolchain /path/to/arm-gnu-13.2/bin \
  --output /path/to/fresh-measure-firmware
```

The MMIO runner requires Unicorn. It executes 96 ARM instruction-model cases
using the patched BSP frame/return sequence, including its abort-barrier
workaround. Tests cover SVC/SYS callers, all IRQ/FIQ mask combinations, successful
reads/writes, precise bus errors and unrelated faults. Reporting a failed read
as success is detected by mutation. Native counter tests reject counting a
pending batch and cover delayed retirement, pixel failure, cancellation,
timeout, mismatched identity and terminal failure. Five additional ARM service scenarios execute the actual service, validator and
pixel verifier with modeled MMIO/FIFO inputs: normal completion/restart, busy
rejection, mapping failure, FIFO failure and bad pixels. These are not hardware tests.

[Retained evidence](../validation/measure/README.md).

## Offline boot packaging

`package_glide_measure.py` preserves the recorded bootloader and accepts the
recorded measurement ELF plus a supplied `7z020clg400` bitstream. It first
round-trips the reference image, then checks every candidate payload against
its input (including Bootgen’s 32-byte PCAP DMA padding) and verifies that the
BootROM header remains unchanged. It writes
only to a fresh local directory. It does not install firmware or establish
that the FPGA implements the required endpoint.

```sh
python3 scripts/test_glide_boot_image.py --reference /path/to/reference-BOOT.bin
python3 scripts/package_glide_measure.py \
  --reference /path/to/reference-BOOT.bin \
  --fsbl /path/to/preserved-fsbl.elf \
  --firmware /path/to/ZZ9000OS-measure.elf \
  --bitstream /path/to/reviewed-completion.bit \
  --bootgen /path/to/bootgen --output /path/to/fresh-package
```

The packaging regression uses the old FPGA payload deliberately. Its receipt
sets `fpga_is_reference_payload: true`; that package cannot run the new
completion counter and must not be installed for the GPU test. Hardware testing
requires the matching routed, timing-reviewed FPGA build. No such image has
been installed by this packaging check.

## Larger workload profiles

The [ABI2 extension](../validation/workload/README.md) adds 320×240 and 640×480
profiles, each with 320 textured triangles per frame. Packets pass the same
validator, tagged completion and error handling. The frame counter waits for
all 65 packets; it never reports packet throughput as frame throughput. ABI1
remains supported for the original test. Physical testing now passes at
54.47 FPS (320×240) and 45.57 FPS (640×480), averaged across three 100-frame
runs per resolution at 800 MHz ARM. These are synthetic offscreen workloads;
DOS guest execution and presentation remain outside the measurement.

Build and check with `test_glide_workload.py --firmware <main.c>`,
`test_glide_measure_firmware.py --firmware <main.c>` and
`test_dosbox_glide_measure.py --host`. Packaging accepts `--build-record` only
when the firmware hash, base provenance and recorded source hashes match.
