# DMA identity response and memory-error observation

Two experimental modules now connect the completion proposal to concrete
RasterIX interfaces: `response_token.v` receives a two-word identity from the
DMA response stream, and `axi_response_fault.v` observes GPU memory responses.
Neither is installed. They assign no XACP opcode, capability or physical address.

## Identity transport using the existing DMA engine

The upstream [FrameStreamingCore](https://github.com/ToNi3141/RasterIX/blob/3e2ee5aa5214b829c2a824250c445ce799ac8dc0/rtl/RasterIX/FrameStreamingCore.v)
can route input ST0 to renderer ST1, or input ST0 back to response ST0.
The test sends the following existing DMA operations:

| Words | Existing operation |
| --- | --- |
| `0x90000004, 0, 0x60000000` | Send four bytes to ST1: the upstream renderer fence |
| `0x50000008, 0, epoch, sequence` | Send eight bytes to response ST0: the identity pair |

These words describe the **offline test**, not commands supported by the current
XACP submit API. The inspected firmware requires a single `0x9…` envelope whose
length matches the whole submission, and its renderer validator rejects fence
opcode 6. An echo submitted separately begins with `0x5…` and fails that header
check; concatenating the envelopes also violates the current length contract.
A future firmware interface needs explicit capability negotiation and exclusive
submission ownership. Do not bypass the current validator to try this on hardware.

The [tagged submission preparer](tagged-submission.md) now validates a private
copy with that unchanged validator, then adds the firmware-only fence and echo.
Its five generated scene streams match the ordinary renderer fixtures. A new
service and physical transport are still pending; this does not enable the
suffix through the released ABI.

The receiver expects epoch first, then sequence with TLAST. It retains the pair
until the mailbox accepts it. A short or overlong response latches a protocol
fault. An external memory fault also blocks input and token delivery, and remains
latched after the external pulse ends. Partial identities never become tokens.
There is no automatic recovery or timeout-based release.

**An echoed identity is not proof that the fence retired.** The DMA engine can
finish delivering the fence word and return the identity while the parser still
waits for memory. The test explicitly exercises this. A completion is permitted
only after the mailbox has accepted the identity and the combined DMA/parser
idle condition subsequently reports retirement. It uses the same registered
`dmaIdle && renderCoreIdle` shape as `RasterIX_EF`, with the real parser and
injected write/texture pending signals. Full rendering, interconnect buffering
and the complete GPU's idle logic still need validation.

This is an alternative to adding a new post-fence renderer opcode. It also
replaces the earlier clear-colour-register **test carrier** with a real DMA
response path. No render configuration register is repurposed.

The response port must be dedicated to this identity protocol. Framebuffer
readback or other arbitrary response traffic cannot share the receiver without
framing and dispatch. The current published FPGA has no response receiver and
ties TREADY high; it must instead connect TREADY to this receiver's backpressure.
The receiver can hold a second identity while the previous result is unread;
the test exercises that storage, but it does **not** authorize overlapping GPU
jobs or sharing the renderer between applications.

## Memory errors must precede acknowledgement consumption

Connect `axi_response_fault` at RasterIX's GPU-side `m_axi` interface, in the GPU
clock/reset domain, and combine its fault with protocol errors at the mailbox's
`bus_error` input. It observes:

- `BVALID && BRESP[1]` for write SLVERR or DECERR.
- `RVALID && RRESP[1]` for read SLVERR or DECERR on **any** beat.

OKAY and EXOKAY do not indicate a bus error. Response bits without VALID are
ignored. READY and RLAST do not qualify detection: a valid response is meaningful
while stalled, and a nonfinal read beat can carry an error.

The output includes the current error combinationally as well as a sticky
register. That blocks a retained acknowledgement on the same clock edge as an
error; a registered-only signal would arrive one edge too late. Timing closure
must validate this path. The monitor is passive: it does not alter AXI handshakes,
stop DMA, count outstanding work or provide a drain signal. It cannot detect a
silent memory corruption, a missing response, a protocol violation or a bus
failure hidden from this observation point. Those need separate validation and
timeout handling; timeout still does not authorize release.

Reset clears the observer and receiver only as part of independently verified
GPU quiescence. A CPU-side reset of these registers alone is not a DMA abort.

## Validation and limits

Nine error-observer scenarios cover invalid response bits, successful responses,
both error codes on both channels, stalled reads, errors racing consumption,
simultaneous faults and a fresh session after quiesced reset. Four mutations
are detected: ignored reads, nonsticky errors, one-cycle-late faults and ignoring
VALID.

Nine stream scenarios connect the unchanged upstream DMA engine and parser to
the receiver and mailbox. They cover identity/order, pending fences, response
backpressure, unread results, cancellation, short/long responses, external faults
and partial identities. Four mutations are detected: corrupted epoch, accepting
short or long identities, and forgetting an external fault pulse.

The full suite passes **79 scenarios and 23 mutation checks**:

```sh
python3 scripts/test_dosbox_glide_fence.py --rtl /path/to/pinned/rtl \
  --axi --coalesced --texture --completion --registers \
  --response-fault --response-token
```

New upstream dependencies and hashes are recorded in
[stream-sources.json](../validation/fence-audit/stream-sources.json).
Logs and the complete validation manifest are in the same directory.
Both new tests first failed because their implementation module was absent;
the completed modules pass, and all eight deliberate defects are detected.

This component stage performed no full FPGA simulation, synthesis, timing
closure or physical measurement. The previous freezes remain unresolved, and GTA still receives the
diagnostic no-board response. This work establishes a testable integration route,
not a playable or faster game.

A later [full renderer test](../validation/full-renderer/README.md) uses these
components with the upstream GPU and synthetic AXI RAM, checking completed pixels
and fault quarantine. Board-specific integration and hardware timing remain open.
