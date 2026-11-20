# What is required for a hardware FPS result

There is no accelerated GTA FPS measurement yet. The diagnostic overlay reports
no board, and no GTA triangle has reached RasterIX. The 79 passing component
scenarios establish particular protocol behaviours, not a complete renderer or
game performance.

An early [physical encoder measurement](../validation/capacity/README.md) now
provides a CPU cost: 320 textured triangles take 1.077 ms median at 800 MHz,
or 3.23% of a 30 FPS frame budget. This makes translation cost promising, but
does not measure GPU rendering or predict GTA FPS. The small completion-timed scene now passes on hardware: 9,302.194 offscreen
FPS over 1,000 frames. [Scope and evidence](../validation/measure/hardware/README.md).

## Remaining prerequisites

1. **FPGA timing closure.** The fence, identity receiver, error observer and
   completion registers are integrated. The corrected board routes all 55,980
   nets, but setup timing fails by 0.773 ns at 56 endpoints in the 100 MHz texture
   path. Physical refinement still failed at -0.282 ns. The carry-free address
   candidate now routes all 55,899 nets and meets declared setup/hold constraints
   at +0.055/+0.011 ns, with zero failing endpoints. Existing timing coverage
   and CDC findings remain. The matching bitstream and boot image are packaged.
   The observed 1920x800 preset also passes steady-state 125 MHz timing; live
   mode transitions remain untested; normal offscreen operation now passes. Vivado 2025.2 runs under Wine/Rosetta on the ARM Mac; this is an
   unofficial tool environment, not an AMD-supported operating system.
2. **Verified firmware and hardware completion.** The existing submit validator
   rejects the proposed fence/echo sequence. A capability-aware interface,
   matching FPGA image and bounded physical retirement/error/cleanup tests are
   needed. Previous readback discrepancies and freezes remain unresolved.
   Successful TCP connectivity does not establish GPU correctness.
   The [C completion reader](completion-client.md) now passes 20 RTL-backed
   scenarios, including late faults. A [guarded transport and counter](measurement.md)
   now build against the recorded Core0 firmware. Its 96 ARM abort-model cases
   and service lifecycle tests pass. Normal physical MMIO, retirement, Core0
   readback and cleanup now pass; fault injection remains required.
   A [tagged submission preparer](tagged-submission.md) also passes tests against
   the unchanged firmware validator: it validates a private command copy before
   adding the firmware-only fence and identity. The experimental counter uses it;
   the matching boot image is active and the fixed-scene counter passes.
3. **Guest renderer integration.** The overlay's 146 exports are diagnostic
   stubs, not 146 implemented Glide functions. Drawing dispatch, texture
   allocation/upload, guest state, required depth/LFB behaviour and presentation
   still need implementation and validation. Changing the hardware query to
   return true would not make the game renderer work.

## Latest environment check

The user restarted after staging. Runtime firmware ABI and FPGA identity checks
pass, followed by five correctness scenes and four timed batches. All runs close
cleanly; the bridge remains responsive. ARM clock stays at 800 MHz. No GTA
workload or physical fault injection has run. [Physical evidence](../validation/measure/hardware/README.md).

The [Wine installation repair](../../../../docs/xacp-firmware-rebuild.md#wine-installation-finding)
resolved a shared-directory layout problem. Vivado batch mode now loads the Tcl
probe, reports 2025.2, finds the target part and obtains the synthesis licence.
The small completion module synthesized with zero warnings or errors. Its first
same-process `opt_design` failed with `ERROR: [Synth 20-411]` and no explanatory
message. Reopening the synthesis checkpoint in a fresh Vivado process allowed
the ordinary optimization, placement, physical optimization and routing flow to
complete. All 10 routable nets routed without errors. The OOC probe's 139 I/O
ports remain unconstrained and PS7 is absent, so this is a toolchain result,
not board timing closure. See the
[default-flow record](../validation/fence-audit/vivado-default-flow.json).
The underlying same-process failure remains unidentified. No firmware was changed.

The unchanged ARM firmware and BSP now build with Arm GNU 13.2.Rel1. The actual
board project also completed block-design validation, IP generation and XSA
export under Vivado, with warnings still requiring review. Regenerated PS7
initialization enabled a separately linked FSBL candidate. These results remove
several rebuild prerequisites. Full-board synthesis has since produced a
checkpoint, independently reopened and audited with exit 0. Both debug blocks
and full-board optimization now complete, followed by routing of all 54,909
routable nets without routing errors. Declared setup/hold constraints pass, but
566 pins lack a timing clock and 1,016 internal endpoints remain unconstrained.
Clock-domain and methodology findings remain open; this is not complete timing
sign-off or a boot-tested replacement. See the [rebuild guide](../../../../docs/xacp-firmware-rebuild.md).

The [detailed crossing review](crossing-audit.md) identifies runtime display modes
beyond the 75 MHz timing baseline and the active line-counter transfer. Nine
digital audio-clock tests pass, including edge-count and divide-by-eight checks;
they do not close physical sampled-clock timing or establish audio quality.
Eighteen fetch-consumer cases also pass. The [runtime video-clock sweep](video-modes.md)
finds setup failures in the three tested 146–162 MHz cases, with PLL-range warnings
in two. The lower tested rates have positive declared timing margins, with all
previous coverage gaps retained. No replacement firmware is promoted by these tests.

A subsequent [physical optimisation candidate](video-optimization.md) clears the
checked 150 MHz setup failure with +0.021 ns margin and retains +0.010 ns hold.
This small margin and the remaining timing-coverage gaps require further review;
the result does not establish a gameplay speed improvement or hardware readiness.

The [full upstream renderer simulation](../validation/full-renderer/README.md)
now passes five scenes through the proposed completion components, including
memory backpressure, cancellation, injected errors and a premature-drain mutation.
The ordinary and additional-marker-fence variants both pass the pixel oracle.
This supersedes the exploratory Icarus elaboration attempt. It uses Verilator,
behavioural RAM and a synthetic AXI memory model; Xanxi's board-specific changes,
HP3 path and CPU mailbox crossing are not included. No physical GPU timing or
replacement firmware follows from this result.

The [board-renderer backport](board-completion.md) now passes the same scene
oracle with Xanxi’s command-credit path, including DDR command transfers and
a pre-adapter drain check. This supersedes the renderer-only gap above; the
actual HP3/converter path is now covered separately by the
[four-state renderer test](board-hp3.md). Its corrected build passes 32
framebuffer checks. The corrected board has completed synthesis and a fresh
checkpoint audit, followed by debug-core implementation and optimization with
zero remaining black boxes. The carry-free candidate subsequently passed
declared routed timing; matching firmware and physical testing remain outstanding.
Physical HP3/DDR operation and firmware integration remain. The
[board endpoint](board-endpoint.md) adds validated project wiring and six digital
tests through the generated CPU mailbox clock converter; this is not a physical
GPU timing result.

## Measurement criteria

Once a real guest renderer passes the prerequisites, compare the same GTA build,
scene, resolution, CPU-cycle setting, audio configuration and ARM clock. Record
the identities of the player, overlay, firmware and FPGA image.

Measure completed game frame presentations over an explicitly timed interval,
after warm-up. Report actual frame count and elapsed time alongside FPS, and
repeat the same run. Count frames at the guest render/present boundary: VGA
refreshes, menu refreshes and repeated framebuffer scans are not gameplay frames.

Keep CPU submission time, GPU completion time and total frame time separate.
Do not convert native-Mac runtime, RTL simulation speed, a 100 MHz nominal clock
or the number of passing tests into an A9 gameplay FPS claim. Any improvement
needs a matched baseline and clean shutdown; isolated renderer throughput must
be labelled separately from game FPS.
