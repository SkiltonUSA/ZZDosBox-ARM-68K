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
