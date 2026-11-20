# Board completion endpoint

The experimental board copy connects RasterIX retirement and fault reporting to
a CPU-readable endpoint. It preserves the existing PS7 settings, device address
map and GPU memory aperture. **No replacement firmware is installed and no GPU
or game FPS is claimed.**

The earlier candidate completed full-board synthesis with 23,372 LUTs, 29,120
registers, 60.5 BRAM36 equivalents and 22 DSPs. It predates the
[four-state HP3 corrections](board-hp3.md). Debug-core placeholders and
`ZORRO_E7M_IBUF`/`CLKOUT1` constraint lookup warnings remain; this is not routing
or timing signoff. [Scoped synthesis evidence](../validation/board-endpoint/pre-hp3-synthesis/result.json).

The corrected HP3 candidate has now completed synthesis and an independent
checkpoint audit: 23,368 LUTs, with register/BRAM/DSP counts unchanged. Its
debug-core implementation and optimization have also passed with zero unresolved
blocks; placement and routing are running. The
[corrected record](../validation/board-hp3/synthesis/result.json) remains separate
from the earlier checkpoint; neither is an installed firmware image.

## Wiring and interface

- Keep the endpoint, response receiver and fault observer on FCLK0, 100 MHz.
- Add M09 to the peripheral interconnect, preserving M00–M08. Vivado generates
  an AXI Clock Converter 2.1 between the 25 MHz peripheral side and this endpoint.
- Route the renderer response stream to the ordered epoch/sequence receiver.
  Connect the renderer's drain signal and passive AXI response-error observer.
- Use `0x83c80000`–`0x83c8ffff` only in this isolated candidate. This is not a
  published XACP ABI. The identity word at offset `0x1c` is `GRX1`.
- Decode all 32 address bits. Invalid high offsets must return `SLVERR`, not
  alias a valid register every 256 bytes.

The [register contract](completion.md) retains results until software consumes
an exact epoch/sequence match. Cancellation still waits for drain. A fault
quarantines the result; resetting the endpoint alone never proves DMA stopped.
The firmware must retain exclusive submission ownership and handle AXI errors;
ordinary unchecked MMIO writes are not sufficient when a control write can
return `SLVERR` during a fault race.

## Validation

The endpoint passes 11 directed scenarios. The standalone register wrapper retains
its nine-scenario regression and four detected deliberate defects. Truncating
the endpoint address to eight bits is also detected.

Six additional tests use the **actual generated M09 converter**, its AMD AXI
infrastructure and XPM CDC implementation, at 25/100 MHz with a 3 ns phase offset:

1. Identity, empty status and rejection of an aliased high address.
2. Rejection of consumption before renderer drain.
3. Exact identity, address-first/data-first writes and read/write backpressure.
4. Cancellation retained until drain.
5. Sticky fault quarantine after a result becomes available.
6. Clean restart with the opposite reset-release order.

These are Verilator digital simulations, not metastability or physical timing
tests. They do not include PS7 execution, the HP3 converter, DDR arbitration or
actual display scanout. Reset during an in-flight bus transaction remains a
separate test. AMD IP remains in the locally generated project and is not
redistributed here.

The separate [HP3 renderer test](board-hp3.md) exercises the generated GPU
protocol converter and AMD FIFO using XSim and behavioural RAM.

## Reproduce

Prepare the renderer using [the backport instructions](board-completion.md), then:

```sh
python3 scripts/prepare_glide_board_project.py \
  --fpga /path/to/XX19d-RIX/fpga \
  --renderer .context/board-completion-source \
  --output /path/to/new-board-copy
```

The preparer rejects a changed or extended renderer inventory and refuses an
output inside either input tree. Run `completion/integrate-board.tcl` with
Vivado 2025.2 inside that copy. It validates/generates the block design and exports
an XSA; it does not create or install a bitstream. Then run
`completion/synthesize-board.tcl` for the synthesis checkpoint and reports.

```sh
python3 scripts/test_dosbox_glide_endpoint.py
python3 scripts/audit_glide_board_endpoint.py \
  --baseline /path/to/baseline/zz9000_ps.bd \
  --candidate /path/to/new-board-copy/zz9000_ps.bd
python3 scripts/test_dosbox_glide_cdc.py \
  --generated /path/to/project.gen/sources_1/bd/zz9000_ps \
  --xpm /path/to/Vivado/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv \
  --glbl /path/to/Vivado/data/verilog/src/glbl.v
```

The CDC runner expects the generated M09 converter wrapper named
`zz9000_ps_ps7_0_axi_periph_imp_auto_cc_1`; confirm that association against the
board audit before reusing it with another topology.

The integration refresh uses the Vivado **IP object**, not the block-design cell
object. Vivado requires a Verilog module-reference top even when child modules
use SystemVerilog. Its metadata parser still reports the renderer's existing
parameter-list header syntax warning; it must not be mistaken for completed
synthesis or timing signoff. Existing audio interface width/address warnings
also remain visible.

[Retained results and source hashes](../validation/board-endpoint/README.md).
