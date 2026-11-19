# Upstream RasterIX fence: integration candidate

RasterIX commit [3e2ee5a](https://github.com/ToNi3141/RasterIX/commit/3e2ee5aa5214b829c2a824250c445ce799ac8dc0)
adds an explicit fence opcode, `0x60000000`. Its command parser holds subsequent
commands until framebuffer writes and texture-memory transactions are no longer
pending. This is a concrete candidate for resolving our completion boundary.
It is **not a verified fix for the ZZ9000 readback discrepancy or freezes**.

## What was tested

Icarus Verilog 13.0 simulated the unchanged upstream `CommandParser.v`, with
controlled pending-transaction inputs. Four cases pass: framebuffer writes,
texture transactions, both together, and neither. The combined case also checks
that draining framebuffer writes alone does not unblock pending texture traffic.
The parser eventually accepts the following command and returns to idle.

Removing the retirement condition in a temporary source copy makes the test fail
with `Passed fence with transactions pending`. That expected failure is retained
alongside the passing output. These tests cover the parser with injected signals;
they do not simulate the complete GPU, AXI interconnect or DDR controller, and do
not identify the logic in the installed FPGA image.

The unchanged, inspected XACP firmware validator rejects the fence at word 2
with error `0x4980`, detail `0x60000000`. ABI 1.0 accepts opcodes 0 through 5;
the new opcode is outside that contract. This check ran natively, with no packet
sent to the Amiga. Depth/stencil restrictions also remain in place.

## Completion contract

The simulation confirms that the **fence word itself can be accepted while
transactions remain pending**. Ending a FIFO submission on that word therefore
does not, by itself, make transmit-complete a GPU-retirement acknowledgement.
Intermediate buffering also needs to be considered.

A candidate protocol is to issue rendering, then a fence, then a changing
completion marker. Marker observation would occur after the preceding writes
have drained, provided the complete FPGA path implements the upstream contract.
CPU cache visibility and exclusive resource lifetimes still require verification.
This sequence has not been tested on the ZZ9000.

Integration requires:

1. Confirm or build compatible FPGA logic, including the transaction tracking
   feeding the fence. The older RTL examined during the initial probes lacks it.
2. Extend the firmware validator and publish an explicit capability/version
   contract. Applications must not infer support from the RasterIX name alone.
3. Verify marker observation, texture replacement, repeated batches, cancellation
   and cleanup on hardware before advertising a rendering board to a guest.
4. Resolve depth and remaining Glide state/texture/LFB requirements separately.

No firmware, clock, installed emulator or release was changed for this audit.
No hardware performance improvement is claimed.

## Write-response integration test

The additional `--axi` test connects the upstream parser to the unchanged
`Coalescer` bypass path and its pinned `ValueTrack` dependency. A simulated AXI
slave delays write responses. Five scenarios cover 1, 3, 8, 32 and 128 writes,
independent address/data stalls, response backpressure and partial retirement.
The command following the fence stays blocked until the final response is
accepted. Removing outstanding-response tracking from a temporary copy makes
the test fail. Both passing and expected-failure logs are included here.

This covers the bypass path only. It does **not** validate coalesced bursts,
texture-cache tracking, the DDR controller, CPU cache visibility or a live GPU.
The test models successful AXI responses; detecting bus errors is a separate
requirement. No clock or emulator performance measurements were made.

## Coalesced-write and token tests

The `--coalesced` suite adds the actual upstream `Coalescer`, `CoalesceAddrGen`,
`CoalesceFiFo`, `sfifo` and `ValueTrack`, with a simulated AXI slave. Eighteen
scenarios pass: nine each at 32-bit and 64-bit data widths. These cover partial
and full eight-beat bursts, 64 pixels across multiple bursts, non-contiguous
addresses, 4-KiB boundaries, independent address/data stalls, delayed responses
and consecutive submissions without resetting the RTL.

The scoreboard checks every output address, pixel, byte strobe and `WLAST`.
It also verifies that contiguous input really merges into fewer transactions.
Removing response tracking causes an early-completion failure; flipping one
pixel bit causes a data-corruption failure. Both intentional failures are saved.
Together with the earlier suites, there are **27 passing scenarios and four
detected mutations**. These are correctness tests, not FPS benchmarks.

After each fence the test sends a changing clear-colour configuration value.
Exactly one matching token reaches the parser's configuration output, after
all preceding burst responses. This proves ordering at that output; it does
not implement a CPU-readable completion register or write a marker into DDR.

For a hardware marker, the proposed sequence is rendering → fence → marker
setup/write. The marker allocation must remain owned until its own write has
retired: observing its value does not acknowledge its AXI response. A permanent
completion area or an explicit hardware sequence acknowledgement needs a
separate verified lifetime contract. Existing XACP framebuffer addresses are
pool-constrained; this work does not authorize arbitrary completion addresses.

Upstream also exposes registered `idle = dmaIdle & renderCoreIdle` in
`RasterIX_EF.v`. An untagged idle read alone can observe the idle state from
before a queued submission starts. A host-visible completion interface must
associate its acknowledgement with the submitted batch. The current ZZ9000
integration has not been shown to expose this new signal to the CPU.

Remaining validation includes the uncached texture reader, complete
render/DMA/interconnect integration, bus errors, CPU visibility and physical
resource cleanup. No firmware or installed emulator was changed.

## Cached-texture read and replacement tests

The `--texture` suite connects the actual upstream cached `TextureReader` and
its dependencies to the parser, a synthetic pixel stage and a simulated AXI
memory. Sixteen cases pass: eight each at 32-bit and 64-bit memory widths.
The parser uploads two page-table entries, samples a four-texel group, repeats
it from cache, remaps the same virtual texels to different physical memory,
crosses cache-line and page boundaries, and returns to the original mapping.
Every returned texel is checked against its physical address. Cache-hit cases
must issue no additional memory request; replacement cases must fetch again.

Memory responses include gaps and a delayed final beat. The texel consumer
also pauses before accepting its result. In five cases per width, the texels
are consumed **before the final memory-read response**. The following fence
and configuration token still wait for the remaining responses. This is why
receiving pixels is insufficient proof that texture memory can be reused.

Two mutation checks pass: counting every response beat as a completed burst
fails with `Lost outstanding texture read`; disabling page-table-triggered
invalidation fails with `Stale or corrupt texture`. Together, the suites now
have **43 passing scenarios and six detected mutations**. They establish
component behaviour, not a fix for the previous physical readback/freezes.

For Icarus, a generated copy of `TextureReader.v` replaces two constants used
as unused FIFO output destinations with two unused wires. Icarus otherwise
rejects those output connections. The runner checks the exact source pattern
before adapting it; the original and adapted hashes are recorded separately.
Remaining compiler warnings are upstream constant-width truncations on disabled
or constant broadcast sidebands; both width-specific compile logs are retained.
No datapath or transaction-tracking logic is altered for the passing runs.

The pixel-stage activity signal is supplied by the testbench. It establishes
read activity (or a cache-hit result) before submitting the trailing fence;
this does not test triangle-to-texture scheduling or command ingress ordering.
The AXI model supplies successful, ordered responses. It does not test errors,
out-of-order IDs, live DDR, or ARM/68k cache coherence.

## ZZ9000 integration boundary

A subsequent [completion-mailbox prototype](../../experimental/completion.md)
adds nine scenarios and five detected mutations, taking the combined suite to
52 scenarios and eleven mutations. Its session/batch acknowledgement is held
until consumed, and errors quarantine work. It is synchronous RTL only: no CPU
register map, CDC wrapper, XACP opcode or physical installation is provided.
The `completion*.txt` logs and hashes are included here.

The published [XX19d-RIX provenance](https://github.com/Xanxi-Amiga/XACP-ZZ9000/blob/6e59edbaf24e0d8d2f29cdd31667a10e16c5b283/firmware/source/XX19d-RIX/PROVENANCE.json)
records RasterIX revision `9269a01c9bd4c3342bfa70ef8e3cdd751c15c6d0`.
The parser in that source distribution has no fence opcode. The manifest and
source hashes are retained here; this is source provenance, not a live FPGA
identity check. The reference Vivado project targets `xc7z020clg400-1` and lists
Vivado 2025.2. No Vivado executable was available in this development session.

Enabling opcode 6 in ARM firmware alone is therefore insufficient. A proposed
integration needs the upstream transaction-tracking changes **and** fence parser,
while preserving the ZZ9000-specific command-read-credit/interconnect changes.
It needs synthesis, implementation and timing closure before board testing.

The firmware completion contract should distinguish submission from retirement:

1. Reject fence use unless the paired, verified bitstream supports it. Do not
   assign an unofficial XACP capability bit or infer support from a product name.
2. Keep command, framebuffer and texture allocations owned until an ordered
   completion observation proves retirement. FIFO transmit-complete is insufficient.
3. Use a changing completion token after the fence, with a verified memory path
   and CPU cache synchronization. A trailing buffered NOP is not such a token.
4. Treat timeout/cancellation as unverified retirement; do not free or reuse
   buffers that the GPU may still access. Recovery must quiesce the hardware.
5. Advertise guest rendering only after repeated batches, texture replacement,
   readback, cancellation and teardown pass on hardware.

Before that hardware step, the integration work still needs to connect the
component results into a full render/DMA/interconnect test, then provide a
batch-tagged acknowledgement the ARM can read. Neither a configuration token
inside the parser nor untagged `idle` is that CPU-visible interface. A verified
bitstream and its matching XACP capability contract remain prerequisites.

No experimental firmware was produced or installed from these tests. The next
hardware prerequisite is a compatible FPGA build, not another FIFO-delay tweak.

## Reproduce

Fetch `CommandParser.v` and `RegisterAndDescriptorDefines.vh` from
`rtl/RasterIX/` at the linked commit. Hashes are in `upstream-sources.json`.

```sh
python3 scripts/test_dosbox_glide_fence.py --rtl /path/to/pinned/rtl
python3 scripts/test_dosbox_glide.py --firmware /path/to/inspected/main.c
```

The first command needs Icarus Verilog (`iverilog` and `vvp`). Optional
`--iverilog`, `--vvp` and `--ivl-dir` arguments support a workspace-local tool
installation. Generated and mutated RTL stays under `.context/`; upstream RTL
is not copied into this repository.

For the write-response test, also fetch `rtl/RasterIX/Coalescer.v` at the same
RasterIX revision and `ValueTrack.v` from its Float submodule, using the exact
path/revision in `tracker-sources.json`. Place both beside the parser and add
`--axi` to the first command. The pinned Float revision is
`83485654e380392a726ee67f8e5d2f4f8f73dd32`.

For merged bursts, add `CoalesceAddrGen.v` and `CoalesceFiFo.v` from
`rtl/RasterIX/`, plus `sfifo.v` from `rtl/3rdParty/`, at the same RasterIX
revision. Place these beside the parser and add `--coalesced`. Source hashes
and paths are recorded in `upstream-sources.json`. Use both flags to run all
27 scenarios and four mutation checks. Test output and temporary mutated
sources stay under `.context/amiga/glide/fence-model/`.

Add `--texture` to run all 43 scenarios and six mutation checks. Fetch the
`TextureReader*`, `TextureCacheDirectMapped*`, `TextureMMU`, `StreamConcatFifo`
and `axis_broadcast` modules listed in `upstream-sources.json`, preserving their
filenames beside the other RTL. The runner generates the Icarus-compatible
reader and mutated copies under `.context/`; it leaves supplied RTL untouched.

Add `--completion` to include the proposed mailbox. All four optional flags run
52 scenarios and eleven mutation checks. The mailbox RTL is local project code;
it does not replace any upstream FPGA module or modify the supplied RTL.
