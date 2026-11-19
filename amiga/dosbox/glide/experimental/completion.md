# Tagged completion mailbox — experimental proposal

`completion.v` is a small synchronous RTL prototype for reporting GPU retirement.
It is not wired into the ZZ9000 and assigns no XACP opcode or capability bit.
`completion_axil.v` adds a tested, single-clock AXI-Lite register interface with
local offsets only; it has no assigned ZZ9000 address. Neither module has
undergone FPGA synthesis or timing closure.
The released emulator does not use it.

## Contract

The future submission adapter must deliver one ordered token after a batch's
fence, through the same command path. The token consists of a session epoch and
a batch sequence number. Both are latched together when `token_valid` and
`token_ready` are high. Merely seeing idle before that event cannot produce a
result. Token acceptance itself is not retirement.

`drained` must mean the entire relevant GPU/DMA/interconnect has completed its
work, including buffered commands and memory responses. It must not mean FIFO
transmit-complete. All inputs to this prototype belong to the same clock domain;
a real CPU interface needs a reviewed clock-domain crossing where applicable.
The module cannot compensate for incomplete or stale upstream drain tracking.

The [DMA response prototype](response-path.md) now tests an identity route using
the actual upstream stream engine. Its echo can arrive before fence retirement;
accepting that token still requires waiting for the full drain condition. The
same report covers immediate/sticky observation of GPU AXI memory errors.

Once a token is accepted, the module waits for `drained` and retains an
acknowledgement until `ack_ready` consumes it. The acknowledgement carries the
same epoch, sequence and a cancelled flag. An unread acknowledgement blocks the
next token. The integration must separately prevent new rendering submissions
from overlapping an exclusively owned batch; this mailbox does not gate the
entire GPU ingress.

| Event | Result | Resource handling |
| --- | --- | --- |
| Idle with no accepted token | No acknowledgement | No completion inferred |
| Token accepted, work outstanding | Pending | Keep resources owned |
| Cancel while pending | Cancellation recorded; wait continues | Keep resources owned |
| Drain after token | Stable acknowledgement | Eligible for release after verified host observation |
| Cancel on the drain cycle | Cancelled acknowledgement | Same retirement requirement |
| Bus error | Sticky fault; acknowledgement suppressed | Quarantine resources |
| Host timeout | No automatic state change | Keep resources owned; request cancellation or recover |
| Reset | Mailbox cleared | Only valid after independent hardware quiescence |

A cancellation pulse is accepted with the token or while it is pending. A caller
that times out before token delivery must retain its cancellation request until
that delivery; a pulse while the mailbox is empty is not remembered. A cancel
after an acknowledgement exists does not rewrite that completed result.

A bus error takes precedence over drain and prevents an unconsumed result from
being accepted on the error cycle. Fault is sticky. Acknowledge and cancel cannot
clear it. Reset is not a GPU abort mechanism: resetting this small module alone
must never be used as evidence that DMA has stopped touching its resources.

## Experimental CPU register interface

The AXI-Lite wrapper retains the acknowledgement during software reads and
requires an identity-matched write to consume it. It accepts write address and
data independently, in either order. One read response and one write response
may be outstanding. Each response stays stable under bus backpressure.

These are **relative offsets within an unassigned experimental peripheral**:

| Offset | Register | Access | Meaning |
| --- | --- | --- | --- |
| `0x00` | Status | R | Bit 0 valid, bit 1 busy, bit 2 fault, bit 3 cancelled when valid |
| `0x04` | Epoch | R | Result epoch, or zero when no valid result |
| `0x08` | Sequence | R | Result sequence, or zero when no valid result |
| `0x0c` | Expected epoch | R/W | Stage the identity of the batch to act on |
| `0x10` | Expected sequence | R/W | Stage the matching sequence |
| `0x14` | Consume | W | Write 1 to consume a matching retired result |
| `0x18` | Cancel | W | Write 1 to request cancellation of a matching pending batch |

Both expected fields must be freshly written with all four byte strobes before
each control attempt. A complete control attempt clears that staging validity,
even if it fails due to a mismatched identity or premature consume. A partial
write has no effect. Wrong offsets, misalignment, read-only writes, unsupported
values, missing identity or an unavailable matching result return AXI `SLVERR`.
They do not reset the mailbox or clear a GPU fault. Reads have no side effects.

Software must have exclusive submission and result-consumer ownership:

1. Read fresh status; reject fault or wait if no result is valid.
2. Read both identity fields, recheck status, and match the outstanding batch.
3. Write both expected identity fields and wait for their successful responses.
4. Write Consume=1 and wait for its successful write response.
5. Read fresh status again; quarantine on fault before releasing resources.

The control write is committed when both address and data have been accepted
and validated. Its response cannot change later while the CPU stalls it.
An error before commit prevents consumption. An error after commit appears in
fresh status, even if the queued write response is already `OKAY`. Similarly,
a read captured before a fault remains an old snapshot until the CPU accepts
it. The tests exercise both cases. A successful write response alone is not a
substitute for the final fault check. Transport failure or timeout also does
not authorize resource release.

The wrapper operates in the mailbox's clock domain. CPU/FPGA clock crossing,
complete address decoding, ARM memory attributes/barriers, bus endianness and
physical acknowledgement visibility still need target integration and testing.
No existing XACP field or ZZ9000 address is appropriated by this proposal.

The submitting controller must prevent reuse of an epoch/sequence pair while an
old result could still be observed. This module transports identities; it does
not generate epochs or reject duplicate input identities. A new session must
receive a new epoch after verified recovery; rollover needs equivalent quiescence.
The epoch must travel with the submitted work, not be sampled from mutable global
state when an old batch finally completes.

## Validation

The test connects the upstream fence-capable `CommandParser` to this mailbox.
A clear-colour configuration value acts as a **test-only** carrier for the
sequence. That is not a proposal to repurpose the clear-colour register. Epoch
is held stable for the synthetic batch; a real adapter must convey it in order.
Memory/DMA retirement and errors are injected signals, not a complete GPU model.

Nine scenarios cover stale pre-submit idle, delayed framebuffer and texture
retirement, cancellation, epoch identity, an unread result backpressuring the
next token, cancellation on drain, sticky error quarantine, error while a result
is awaiting consumption, and a fresh session after a quiesced reset.

Five mutations are detected: allowing stale idle to produce a result, ignoring
drain, reporting the wrong epoch, permitting result consumption on an error,
and losing cancellation. Combined with the preceding component suites there
are 52 passing scenarios and eleven detected mutations. These are correctness
checks, not throughput or game FPS measurements.

Run from the repository root using the pinned RTL in the
[fence audit](../validation/fence-audit/README.md):

```sh
python3 scripts/test_dosbox_glide_fence.py --rtl /path/to/pinned/rtl --completion
```

Add `--axi --coalesced --texture` for the complete suite. Generated simulation
and mutation outputs stay under `.context/amiga/glide/fence-model/`.

The additional `--registers` suite tests the AXI-Lite wrapper without upstream
GPU logic, using injected tokens/drain/errors. Nine cases cover empty and
premature consumes, either AW/W order, identity mismatch, partial and stale
staging, cancellation, invalid registers, stale read snapshots and post-commit
faults. Four mutations are detected: bypassed identity, premature half-write
execution, retained stale staging and a changing stalled read response.
The combined suites now pass **61 scenarios and fifteen mutation checks**.

```sh
python3 scripts/test_dosbox_glide_fence.py --rtl /path/to/pinned/rtl \
  --axi --coalesced --texture --completion --registers
```

The [published clock-topology audit and integration plan](integration.md) identify
an existing 25-to-100 MHz AXI converter arrangement for a new register endpoint.
No custom mailbox-field crossing is needed if that arrangement is retained.

Next: integrate and validate the AXI endpoint and its crossing, choose an explicit
ordered-token transport with XACP, validate the full drain/error path, and
obtain a compatible FPGA build before physical testing. Current XX19d-RIX
firmware alone cannot activate this prototype.
