# Firmware completion reader

The experimental C reader implements the CPU observation sequence for the
[tagged mailbox](completion.md). It passes **20 scenarios against the actual
mailbox RTL**, and compiles for Cortex-A9 ARM32 with both soft and softfp ABIs.
It now has a guarded MMIO transport in a separately built experimental firmware,
but neither transport nor counter has run on hardware. It is not published as an
XACP service or used by the released emulator. These tests provide no GPU or GTA FPS result.

## Resource lifetime

Each submitted batch retains its epoch, sequence and completion state. A poll
performs at most eight bus transactions; a pending batch requires just one
status read. The caller schedules further polls without blocking the firmware
main loop. A deadline does not permit the caller to free GPU resources.

The reader checks status, reads the result identity, rechecks status, stages
the expected identity, consumes the result, then reads status again. Only a
matching retired or cancelled result followed by an empty, nonfaulted mailbox
permits resource release. Transport errors, GPU faults and identity mismatches
quarantine the batch. Terminal results are retained so subsequent polls do not
consume a second result or retry a failed control operation.

Exclusive submission and consumption are prerequisites. The final empty-state
check relies on no other client submitting work during this sequence. A session
must verify the endpoint identity before using the reader. The reader does not
create tokens, establish exclusivity, cancel work, reset hardware or allocate
the GPU aperture.

## Transport integration

The transport callbacks must report failure on an unsuccessful transaction and
provide the device-memory ordering required by the ARM mapping. They must not
report a posted write as successful before its response is known. DMA visibility
and cache maintenance remain the transport/integration layer's responsibility.

The pinned XX19d-RIX BSP's `Xil_In32` is a volatile load and the ordinary
`Xil_Out32` path is a volatile store. Neither returns an AXI error status.
Both the BSP default data-abort handler and the firmware's registered
`arm_exception_handler_id_data_abort` loop indefinitely. A callback that simply
wraps those accesses and returns success would therefore violate this contract:
a rejected mailbox control write could stop firmware execution instead of
quarantining the batch. The [experimental transport](measurement.md#guarded-completion-accesses)
now adds a narrowly scoped abort hook and verifies the Strongly-ordered mapping.
Its ARM model tests pass; physical transport validation remains required before
enabling a capability or claiming a GPU measurement.

This source finding concerns the pinned firmware at XACP commit
`6e59edbaf24e0d8d2f29cdd31667a10e16c5b283`; it does not identify the cause of a
previous hardware freeze. Only the separate candidate’s exception handler was
changed; installed firmware remains unchanged.

## Tests

```sh
python3 scripts/test_dosbox_glide_completion_client.py
```

The C reader calls a Verilator-driven AXI transport connected to
`completion_axil.v` and `completion.v`. The fixture injects tokens, drain and
fault signals; it does not replace the mailbox with a software model. Write
address/data arrival order alternates, and read/write responses experience
backpressure. Other suites cover the full endpoint, converter and renderer.

| Scenarios | Count |
| --- | ---: |
| Idle before a token, delayed drain, retirement and repeated terminal polls | 1 |
| Cancellation remains pending until drain | 1 |
| Incorrect epoch or sequence remains unconsumed | 2 |
| Existing GPU fault | 1 |
| GPU fault after each observation/write before the final status read | 7 |
| Transport failure at each of the eight transactions, including a lost successful write response | 8 |

Removing the final status read causes the post-consume GPU-fault test to fail.
The original reader is rebuilt and rerun after that mutation check. ARM builds
are compile checks, not hardware execution. Evidence is retained in
[validation/completion-client](../validation/completion-client/README.md).
