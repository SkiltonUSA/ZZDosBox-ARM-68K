# Preparing a validated, tagged submission

`submit_prepare.inc` is an experimental firmware integration unit. It compiles
with the actual, unchanged XX19d-RIX validator and generates command streams
identical to the five ordinary full-renderer fixtures. Native sanitizer tests
and Cortex-A9 soft/softfp compile checks pass. It is not connected to a released
XACP service, FIFO or hardware transport; no new firmware or FPS result follows.

## Command preparation

The caller keeps `GRX_TaggedSubmission` in firmware-private storage. Its command
array is 2 KiB, plus metadata, so it should not be placed on a small task stack.
The shared source and private destination must be distinct. The caller supplies
valid diagnostic output pointers and makes the shared staging data visible
before calling the helper.

The helper unpublishes any previous output, checks capacity, copies the client
commands, and validates that private copy. It appends seven words only after
validation succeeds:

```text
90000004 00000000 60000000       renderer fence envelope
50000008 00000000 epoch sequence  identity response envelope
```

These are the same internal stream operations exercised by the
[renderer tests](board-completion.md), not new public XACP opcodes. The original
render envelope remains unchanged. The board's `C_TX_FIFO_DEPTH` is **512 words**,
although the legacy validator permits up to 4,096. This preparer uses the actual
512-word FIFO limit, leaving **505 words** for client commands. The test reads
the board design and checks this limit; the eventual transmitter must also
check current FIFO vacancy. A capacity or validation failure leaves
the published output count at zero, preventing reuse of a previously valid
packet. The helper returns the existing validator error without sending data.

Copying before validation ensures that validation and transmission use the
same command bytes. This does not make the whole shared-memory copy atomic or
freeze textures and framebuffer contents. Exclusive submission and resource
lifetime rules still apply. The firmware assigns the epoch/sequence; the client
must not be allowed to choose or reuse that identity through shared arguments.

The old validator still rejects an outer DMA echo, concatenated envelopes and
client fence opcodes. Opcode 5 *inside* the renderer envelope means texture-page
loading, not a DMA echo; its existing range/count checks remain intact. No raw
command bypass or relaxed validator was added.

## Integration remaining

The eventual service must serialize all GPU users, retain the private packet and
its resource references, transmit the prepared stream, then use the
[completion reader](completion-client.md). FIFO transmit-complete is not pixel
retirement. `next_state` is a proposed validator state only: preparation does
not update the live firmware state. Publish it only after verified retirement;
an error or timeout quarantines the batch and prevents further submissions
until recovery establishes quiescence.

The helper retains ABI 1.0 validation for incoming commands. A capability-aware
experimental service and matching FPGA image are still needed before sending
the appended suffix. Physical MMIO error handling, cache visibility, cleanup
and routed timing remain pending. The released submit path is unchanged.

## Validation

```sh
python3 scripts/test_dosbox_glide_submit.py --firmware /path/to/XX19d-RIX/arm/ZZ9000_proto.sdk/ZZ9000OS/src/main.c
```

The runner extracts and compiles the source validator, recording its hash. It
checks five scene packets against the existing full-renderer fixture generator,
the largest packet, six invalid sizes, destination guards, source mutation
during validation, rejected fence/echo attempts and unchanged live validator
state. AddressSanitizer and UndefinedBehaviorSanitizer pass. Both ARM ABI builds
compile the same validator/preparer pair; they are not hardware execution.

The existing encoder/validator regression also passes: 306 texture/aspect/
origin/blend batches with depth rejection, ten offscreen fixtures and rejection
of a client fence. [Evidence](../validation/tagged-submission/README.md) retains
the exact generated stream, hashes and logs. Byte identity relates this helper
to the ordinary renderer fixtures; it does not extend coverage to new GPU
operations or replace physical testing.
