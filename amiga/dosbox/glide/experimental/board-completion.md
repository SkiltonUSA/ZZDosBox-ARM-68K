# Completion backport for Xanxi's renderer

The experimental completion path now runs with the **XX19d-RIX board renderer**
in simulation, including commands fetched through Xanxi's DDR read-credit buffer.
Both ordinary and marker-fenced streams pass 32 framebuffer checks each, with
234,366 checked interior pixels across the two runs. There is still **no physical
GPU or GTA FPS result**, and no firmware has been installed.

## Changes

The [patch](board-completion.patch) targets XACP commit
`6e59edbaf24e0d8d2f29cdd31667a10e16c5b283`. It backports activity tracking and
fence execution from RasterIX `3e2ee5aa5214b829c2a824250c445ce799ac8dc0`, plus the
`ValueTrack` reset correction from Float `83485654e380392a726ee67f8e5d2f4f8f73dd32`.
Credits remain with ToNi3141's RasterIX/Float and Xanxi's XACP work; the patch
retains their GPL-3.0-or-later source headers and licences.

- Track framebuffer packing, coalesced writes and their responses, texture reads,
  the board's clear/scissor/strobe path and pending fragment broadcasts.
- Add the experimental fence opcode and propagate renderer/DMA idle status.
- Preserve Xanxi's command-read reservation and queue logic. Add an activity
  output for its existing state.
- Track command-read completion **before the 64-to-32-bit width adapter** as well
  as the credit queue. Returned 64-bit beats may still contain unread 32-bit words.
- Reset the tracker status explicitly. Retain the board's existing clear engine,
  triangle register bank and texture implementation.

This is a backport, not a wholesale replacement with the newer upstream renderer.
The patch changes 16 staged files, including two copies of the same Float helper
needed by the simulation directory layout.

## Tests and finding

| Test for each command variant | Result |
| --- | --- |
| Inline clear, solid, additive, texture and alpha scenes | Five framebuffers pass |
| Inline delayed memory responses / cancellation | Five framebuffers pass in each case |
| DDR-fed commands: ordinary / delayed / cancellation | Five framebuffers pass in each case |
| Injected write error / read error | Completion quarantined; first two frames pass before the read error |
| Short DDR transfer | Completion stays pending with unread burst data |
| Force all drain signals true | Premature acknowledgement detected |
| Remove only the pre-adapter command-read tracker | Premature acknowledgement detected for the short-transfer case |

The DDR cases compare every consumed command word, NOP padding and trailing
fence against the submitted stream. Pixel checks use the existing probe oracle,
with its edge exclusions and alpha tolerance. Guard words, texture preservation,
batch identity and retained acknowledgements are also checked. The existing
upstream-renderer matrix still passes after the harness changes.

The first DDR fixture submitted an unpadded payload. The existing address
generator fetches full 16-word/64-byte bursts, leaving unread words after a
short transfer. This could contaminate the next simulated command batch. Valid
test transfers now include NOP padding. Completion also remains pending for
the intentionally malformed transfer: it must not silently declare the leftover
read data retired. Removing the new tracker reproduces the premature completion
in both variants. This does not establish the cause of earlier physical freezes.

## Reproduce

Requires Python 3, `patch`, Clang, Verilator and its C++ build tools. Start from
the published XX19d-RIX source tree. The preparer verifies its complete RTL
inventory and hashes, then modifies a **new copy**, leaving the source untouched.

```sh
python3 scripts/prepare_glide_board_completion.py \
  --rtl /path/to/XX19d-RIX/fpga/RasterIX_G1/RasterIX/rtl \
  --output .context/board-completion-source
python3 scripts/test_dosbox_glide_full_renderer.py \
  --upstream .context/board-completion-source --board --ordered-state \
  --output .context/board-completion-ordered
python3 scripts/test_dosbox_glide_full_renderer.py \
  --upstream .context/board-completion-source --board \
  --output .context/board-completion-control
```

Expected fatal assertions in mutation logs indicate detected deliberate defects.
The runner succeeds only when those specific failures occur. The source preparer
also rejects extra or changed source files before creating its output directory.
[Evidence and source hashes](../validation/board-completion/manifest.json),
[ordered results](../validation/board-completion/ordered/results.json) and
[control results](../validation/board-completion/control/results.json) are retained.

## Remaining hardware gate

Tests use Verilator's two-state simulation, behavioural RAM and a synthetic AXI
memory responder. They include the real board renderer and its internal command
credit logic, but **not** the actual AXI4-to-AXI3 converter, PS7 HP3/DDR arbitration,
CPU caches, mailbox clock crossing, display scanout or firmware validator.
Depth is not exercised. Simulation cycles are not measured ZZ9000 throughput.

Next wire the completion/error endpoint into the board project and validate
the actual converter and mailbox crossing, then synthesize and route that
candidate with reviewed constraints. Firmware needs a matching capability and
validated submission route. Only after those gates can a bounded hardware scene
provide trustworthy GPU elapsed time. Existing release binaries remain unchanged.
