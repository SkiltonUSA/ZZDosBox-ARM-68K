# Completion registers: FPGA integration plan

The published XX19d-RIX design already contains a CPU-to-GPU AXI clock
converter. Keep the experimental completion mailbox and its register wrapper
on the GPU clock; use Vivado's AXI infrastructure for the CPU connection.
A bespoke synchronizer for each mailbox field is unnecessary with this design.
This records the original source-based plan. The [board endpoint](board-endpoint.md)
now implements its wiring in an isolated project; no replacement is installed.

## Evidence

Inspected [zz9000_ps.bd at XACP commit 6e59edb](https://github.com/Xanxi-Amiga/XACP-ZZ9000/blob/6e59edbaf24e0d8d2f29cdd31667a10e16c5b283/firmware/source/XX19d-RIX/fpga/XACPGPU_G4C/ZZ9000_proto/ZZ9000_proto.srcs/sources_1/bd/zz9000_ps/zz9000_ps.bd).
The [machine-readable audit](../validation/fence-audit/clock-topology.json)
records its SHA-256 and exact connections. Reproduce it with:

```sh
python3 scripts/audit_dosbox_glide_clocks.py /path/to/zz9000_ps.bd
```

The script refuses a different source hash. It reads connectivity only; it
does not run Vivado, check timing constraints, identify the live bitstream or
change the board clock.

| Function | Published connection |
| --- | --- |
| CPU peripheral ingress | PS `M_AXI_GP1` → `ps7_0_axi_periph/S00_AXI`, FCLK1 at 25 MHz |
| GPU command registers | Interconnect `M08_AXI` → `gpu_cmd_fifo/S_AXI`, FCLK0 at 100 MHz |
| Existing crossing | `ps7_0_axi_periph/m08_couplers/auto_cc`, AXI Clock Converter 2.1 |
| Command stream | FIFO `AXI_STR_TXD` → `RasterIX_0/s_cmd_axis`, same 100 MHz clock |
| GPU memory | RasterIX → `axi_interconnect_4` → PS HP3, 64-bit data, same 100 MHz clock |
| GPU/FIFO reset | `proc_sys_reset_0/peripheral_aresetn`, synchronized to FCLK0 |
| Peripheral ingress reset | `rst_ps7_0_25M/peripheral_aresetn`, synchronized to FCLK1 |

The memory interconnect includes an AXI4-to-AXI3 protocol converter. Its buffering
must remain part of the retirement analysis. ARM CPU overclocking is separate
from these published fabric clock settings; no CPU or GPU clock change is proposed.

## Proposed wiring

1. Add a separate AXI-Lite peripheral output to `ps7_0_axi_periph`. All nine
   existing outputs are occupied; preserve them. Configure the new output at
   FCLK0, following M08's converter/reset arrangement. Validate the regenerated
   converter and both reset domains in Vivado rather than copying generated IP.
2. Connect `completion_axil` and `completion` to FCLK0 and the GPU reset domain.
   Keep ordered token, drain and fault inputs in that domain. Do not cross the
   epoch and sequence as independently sampled asynchronous buses.
3. Allocate a reviewed address segment in the FPGA/firmware ABI. Existing GPU
   identity registers occupy `0x83c60000`–`0x83c6ffff`; the command FIFO occupies
   `0x83c70000`–`0x83c7ffff`. Neither is spare mailbox space. No new address is
   assigned here. The prototype has an 8-bit local address: the integration must
   reject out-of-range upper address bits rather than silently aliasing a larger
   segment every 256 bytes.
4. Deliver an explicit ordered epoch/sequence token after the new fence through
   the command path. The [DMA response prototype](response-path.md) tests an
   existing stream route for the identity, with a dedicated receiver. The
   published FPGA still lacks the new fence and receiver wiring. Updating the
   firmware validator alone cannot provide them.
5. Expose complete drain and sticky error information. The block-design GPU
   scalar ports expose performance signals, not a tagged completion/error port.
   Neither `perfBusy` nor FIFO transmit-complete proves batch retirement.
   Observe accepted AXI read/write error responses and all relevant outstanding
   work; do not tie the prototype's `bus_error` input low.
6. Publish a capability/version only after the complete path is implemented.
   ARM software must use reviewed device mappings, ordering and exclusive batch
   ownership before consuming the registers described in [completion.md](completion.md).

The current command-response stream has no connected interface receiver.
`m_cmd_resp_axis_tready` is tied to `gpu_const1`, and the FIFO's receive channel
is disabled (`C_USE_RX_DATA=0`). Consequently, enabling an output token inside
RasterIX alone would not make it readable by the CPU. A response-FIFO design
would require separate wiring and protocol work; it is an alternative to this
register endpoint, not something already available.

## Required validation before hardware

- Regenerate and validate the block design, including AXI-Lite metadata,
  complete address decode, reset release, converter placement and constraints.
- Simulate the real converter with asynchronous phase/backpressure, both reset
  sequences and outstanding CPU reads/writes. Existing wrapper tests cover its
  single-clock bus contract, not this converter.
- Exercise the full command and HP3 paths: ordered token delivery, delayed
  responses, memory errors, cancellation and repeated submissions. Preserve
  Xanxi's command-read-credit and other board-specific changes.
- Complete FPGA synthesis, implementation and timing closure using the intended
  toolchain. The routed baseline does not include these completion extensions.
- Only then perform bounded hardware checks with explicit quiescence and cache
  visibility. A reset or timeout never authorizes freeing GPU-owned memory.

The topology inspection itself adds no simulation or FPS result. The subsequent
[response-path tests](response-path.md) bring the component suite to 79 scenarios
and 23 detected mutations. Neither stage resolves the earlier
hardware freezes. The released emulator remains unchanged.

The subsequent [full renderer simulation](../validation/full-renderer/README.md)
connects the actual upstream renderer to synthetic AXI RAM and the proposed
receiver/observer/mailbox. Pixel correctness, delayed responses, cancellation
and error quarantine pass. This addresses the upstream rendering side; it does
not replace validation of Xanxi's modifications, converters, HP3 or CPU crossing.

The [minimal board backport](board-completion.md) now preserves Xanxi’s renderer
and command-credit logic while adding tested fence/drain propagation. Its
synthetic-memory tests do not validate the converter, HP3 or CPU crossing.
