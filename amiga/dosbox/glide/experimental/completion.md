# Tagged completion mailbox — experimental proposal

`completion.v` is a small synchronous RTL prototype for reporting GPU retirement.
It is not wired into the ZZ9000, exposes no MMIO registers, and assigns no XACP
opcode or capability bit. It has not undergone FPGA synthesis or timing closure.
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

## CPU interface still required

An eventual register/CDC wrapper should retain the entire acknowledgement while
software reads it, and consume it only through an explicit write handshake.
Software must read valid/fault status, read the identity, recheck status, then
match both fields against its outstanding batch. The wrapper must confirm that
the consume handshake succeeded without a fault before resources are released;
software must not assume that issuing an acknowledge write succeeded.
This assumes exclusive consumer ownership. An identity mismatch is not success.
The wrapper's ordering, atomic snapshot and error behaviour need their own tests.
No CPU register address or existing XACP field is appropriated by this proposal.

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

Next: implement and test the CPU register/CDC wrapper, choose an explicit
ordered-token transport with the XACP integration, validate the full drain/error
path, and obtain a compatible FPGA build before physical testing. Current
XX19d-RIX firmware alone cannot activate this prototype.
