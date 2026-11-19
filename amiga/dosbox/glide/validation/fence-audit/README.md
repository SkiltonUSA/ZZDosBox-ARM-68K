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

## ZZ9000 integration boundary

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
