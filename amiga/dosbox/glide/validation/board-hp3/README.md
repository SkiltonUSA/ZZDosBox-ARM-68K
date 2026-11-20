# HP3 converter validation

The corrected renderer passes **10 XSim cases and 32 framebuffer checks**, with
117,183 interior pixels compared. Both Verilator renderer variants also pass
32 framebuffer checks each. No physical GPU or gameplay FPS is claimed.

- `xsim/`: clean, patch-prepared run through the generated AXI4-to-AXI3 converter
  and AMD FIFO, with behavioural RAM. Includes compiler logs, input hashes,
  four declaration-order normalizations and pixel results.
- `verilator-ordered/`, `verilator-control/`: the revised source passes both
  existing renderer regression variants, including their deliberate defects.
- `failures/before-reset.log`: unknown framebuffer controls stall startup.
- `failures/before-address.log`: after resetting those controls, additive reads
  still stall with undriven upper framebuffer address bits.
- `failures/before-blend.log` and `before-blend-pixels-2.hex`: the intermediate
  build completes but writes an unknown pixel at offset 4088 under delayed
  memory responses. The pixel checker rejects it; completion alone is not enough.
- `topology.json`: revised board mapping, PS7 settings, GPU aperture and endpoint
  wiring remain as intended.
- `converter-equivalence.json`: all eight converter/vendor/header inputs in the
  newly generated board project are byte-identical to the tested inputs.
- `integration.json`: revised project validation and XSA export succeeded.
- `manifest.json`: retained evidence hashes and tested patch identity.
- `synthesis/`: corrected full-board synthesis and independent checkpoint audit,
  including source/checkpoint hashes, utilization, DRC, clocks and unrouted
  timing. The original wrapper's exit code is unknown; the separate audit
  exited zero. Only the two expected debug placeholders remain at this stage.
- `synthesis/comparison.json`: comparison with baseline synthesis, retaining
  the hashes of both timing reports.
- `optimization/`: both debug cores implemented, full-board optimization exited
  zero, and no black boxes remain. Reports retain the unresolved timing/CDC
  findings; placement, routing and physical validation are separate steps.

The forced-drain case must fail with an outstanding-transaction assertion. That
failure is the expected detection of a deliberately broken completion signal.
The positive error-injection cases instead prove that completion stays
quarantined after a read or write response error.

The RAM model serializes transactions. These scenes do not test physical DDR
arbitration, out-of-order responses, long-burst splitting, depth rendering or
CPU/firmware submission. AMD IP, generated Vivado projects and bitstreams are
not distributed here. See [the report](../../experimental/board-hp3.md).
