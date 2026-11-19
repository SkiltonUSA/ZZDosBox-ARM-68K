# Routed baseline: clock-inheritance experiment

**Diagnostic only. No RTL change, saved candidate checkpoint or firmware flash.**

The supplied clock-wizard XDC creates a primary clock at its input even though
the block design feeds that input from PS7 FCLK0. The routed baseline reports
TIMING-4 and TIMING-27 for this redefinition. Its generated clock consequently
uses that second primary clock, rather than PS7, as its master.

The experiment exported the effective timing constraints from the routed design,
then used Vivado's `reset_timing` and `read_xdc` to reload them. This unchanged
control reproduced setup WNS +0.156 ns, hold WHS +0.010 ns, and every reported
`check_timing` coverage count. This control was required before the candidate.

The candidate removes exactly two lines: the duplicate primary `create_clock`
and its 0.100 ns input-jitter setting. PS7's original 0.300 ns input jitter stays
in place. All timing exceptions remain unchanged. PLL-generated clocks then
inherit `clk_fpga_0` as their master. The diagnostic script saves reports only.

| Check | Original/control | Candidate |
| --- | ---: | ---: |
| Setup WNS | +0.156 ns | +0.156 ns |
| Hold WHS | +0.010 ns | +0.010 ns |
| Clock pins lacking a timing clock | 566 | 566 |
| Unconstrained internal endpoints | 1,016 | 1,016 |
| TIMING-4 / TIMING-27 | 1 each | 0 |
| TIMING-47 warnings | 0 | 2 |
| Critical CDC groups: unsafe / unknown endpoints | 23 / 173 | 23 / 173 |

The two TIMING-47 warnings expose the existing bidirectional false paths between
FCLK0 and the video PLL output: the tool now recognizes a common clock origin.
This is **not a reason to delete those exceptions blindly**. Their crossing logic
and the clock wizard's runtime reconfiguration need review. The candidate does
not resolve sampled audio-clock timing, missing I/O delays or CDC correctness.

**Decision:** keep the experiment unpromoted. It supports correcting the clock
wizard's input-source configuration in a future source build, but does not
validate that build or establish a performance improvement. Do not patch a
generated XDC as the permanent solution.

Reproduction requires Vivado 2025.2 and the recorded `baseline-routed.dcp`.
Copy the checkpoint and files from the [evidence directory](../validation/clock-audit/)
into one working directory, then run `compare-clock-audit.tcl`. The script
aborts before the candidate if the unchanged control's worst setup/hold timing
differs. The recorded analysis additionally compares all coverage counts.
The tool exited 0; input/report hashes and results are in `result.json`.

AMD documents that [`reset_timing`](https://docs.amd.com/r/2021.1-English/ug835-vivado-tcl-commands/reset_timing)
clears in-memory timing constraints and requires them to be reloaded; it does
not automatically restore the source XDC files. The original checkpoint remains
the reference throughout this experiment.
