# Rebuilding Xanxi's XX19d-RIX firmware for the Glide experiment

This guide records the published baseline and the work needed to build our
experimental completion path. It is not a release or a flashing procedure.
The existing emulator release does not require this experimental firmware.

## Source baseline and verification

Reviewed [Xanxi-Amiga/XACP-ZZ9000 at commit
6e59edbaf24e0d8d2f29cdd31667a10e16c5b283](https://github.com/Xanxi-Amiga/XACP-ZZ9000/tree/6e59edbaf24e0d8d2f29cdd31667a10e16c5b283/firmware/source/XX19d-RIX),
which matched upstream HEAD when checked. Pin this revision for the baseline;
do not silently combine it with a newer RasterIX or firmware tree.

The source subtree contains 684 files, totalling 11,472,864 bytes. Every
downloaded file matched its Git blob hash. All 100 XPR source-file references
resolve. The XPR SHA-256 matches the published provenance. See the
[static audit](../amiga/dosbox/glide/validation/fence-audit/firmware-source-audit.json).

Upstream's [build report](https://github.com/Xanxi-Amiga/XACP-ZZ9000/blob/6e59edbaf24e0d8d2f29cdd31667a10e16c5b283/firmware/source/XX19d-RIX/BUILD_AND_VERIFICATION.md)
records clean-project opening, block-design validation, IP regeneration and
synthesis under Vivado 2025.2. It explicitly does not establish placement,
routing, a reproduced production BOOT image or hardware validation of a rebuild.
Some older provenance text predates that synthesis check; retain this distinction.

## What is in the firmware

`BOOT_XX19d-RIX.bin` combines three partitions in order:

| Component | Source / BIF filename | Purpose |
| --- | --- | --- |
| First-stage bootloader | `arm/ZZ9000_proto.sdk/ZZ9000FSBL/src`; `FSBL_exec_HP3_64.elf` | Initialize the Zynq processing system and load the FPGA and application |
| FPGA | `fpga/XACPGPU_G4C/ZZ9000_proto/ZZ9000_proto.xpr`; `ZZ9000_G4C_C1_EXPERIMENTAL.bit` | Zorro interface, video/audio hardware, DMA/interconnect and RasterIX |
| ARM firmware | `arm/ZZ9000_proto.sdk/ZZ9000OS`; `ZZ9000OS.elf` | Board services and XACP v1.8 dispatch, including image, audio and GPU services |

The BIF is `build/BOOT_XX19d-RIX.bif`. Directory names such as G4C and G1 are
historical project names, not interchangeable public firmware versions.

The published reference BOOT has size 4,593,344 bytes and SHA-256
`901393712ca3056ffe8e8895afc08b553d3c50590648d3182a56b1ab2e0ef23b`.
Individual partition hashes are in upstream `PROVENANCE.json`. These identify
the published binaries; a newly linked or routed build is not expected to match
without reproducing the complete original toolchain and build conditions.

## Tools and current build status

| Tool | Required configuration / current evidence |
| --- | --- |
| Vivado | 2025.2, device `xc7z020clg400-1`, Zynq-7000 support; out-of-context completion module routed under Wine 11.0; fresh-process default optimization also passed |
| ARM compiler | Arm GNU 13.2.Rel1 with newlib; unchanged BSP and ARM application build passed locally |
| BSP / FSBL tools | Compatible Xilinx standalone BSP and processing-system initialization generation from the actual hardware design |
| Bootgen | 2025.2 startup/help passed under Wine; boot-image packaging not yet tested |
| GNU make / Python | Firmware and boot-ROM resource build |

Vivado installation completed after authenticated download of the selected
16.61 GB payload. After the Wine-specific directory repair below, batch mode
loaded the test Tcl, reported version 2025.2, found the device and obtained the
synthesis licence. The out-of-context completion module synthesized successfully
with zero synthesis warnings or errors. In the first run, implementation failed in
`opt_design` with `ERROR: [Synth 20-411]` and no explanatory message; the process
returned 1. Placement and routing were not reached. The cause remains unresolved,
and this result does not establish a defect in Xanxi's firmware. See the
[probe record](../amiga/dosbox/glide/validation/fence-audit/vivado-probe.json).
This small circuit cannot sign off the board design. Wine remains an unofficial
build environment.

The actual published project also opened successfully. Block-design validation,
IP output generation and a hardware-platform export completed with exit 0.
This regenerated `ps7_init.c` and `ps7_init.h` from the current design; the
exported hardware description records HP3 enabled at 64 bits. Its ARM PLL is
1333.333 MHz with CPU divisor 2 (approximately 666.667 MHz CPU); the PLL value
is not the CPU clock. This generated baseline does not incorporate a runtime
800 MHz selection. No live clock setting was changed by generating it. See the
[hardware-export record](../amiga/dosbox/glide/validation/fence-audit/baseline-hardware-export.json).

This was not a warning-free validation: Vivado reported a critical HDL parser
warning near `localparam` in `RasterizerCommands.vh`, plus interface width and
address-path warnings. Generation completing does not resolve those diagnostics
or establish full-board synthesis/timing. No automatic IP upgrade was applied.

A subsequent run reopened the saved synthesis checkpoint and executed
`opt_design -retarget`, `-propconst`, `-sweep`, `-bufg_opt` and
`-shift_register_opt` as separate passes. All five passed, followed by
`place_design` and `route_design` (exit 0). Ten routable nets were fully routed
with no routing errors. The internal setup/hold slack was +7.154/+0.246 ns
against a 10 ns clock constraint.

This is a limited toolchain result: 70 input ports and 69 output ports have no
delay constraints, and the out-of-context DRC warns that the PS7 block is absent.
It does **not** establish ZZ9000 timing closure. A further control run reopened
the same synthesis checkpoint in a fresh process and ran plain `opt_design`:
that also passed (exit 0). The selected passes are therefore not required for
this small circuit. Separating synthesis and implementation with a checkpoint
is the simpler observed workaround; the underlying cause of the earlier
same-process failure remains unidentified. See the
[fresh-process control](../amiga/dosbox/glide/validation/fence-audit/vivado-fresh-default.json).
See the [staged probe](../amiga/dosbox/glide/validation/fence-audit/vivado-staged-probe.json)
and its [routing](../amiga/dosbox/glide/validation/fence-audit/vivado-staged-route.rpt),
[timing](../amiga/dosbox/glide/validation/fence-audit/vivado-staged-timing.rpt) and
[DRC](../amiga/dosbox/glide/validation/fence-audit/vivado-staged-drc.rpt) reports.

The fresh-process control was then extended through ordinary `opt_design`,
`place_design`, `phys_opt_design` and `route_design`. This also completed with
exit 0: 10/10 routable nets, no routing errors and final reported internal
setup/hold slack of +7.154/+0.246 ns. The same OOC constraints limitations apply.
See the [default-flow record](../amiga/dosbox/glide/validation/fence-audit/vivado-default-flow.json)
and its [timing report](../amiga/dosbox/glide/validation/fence-audit/vivado-default-timing.rpt).

For this environment, save a synthesis checkpoint and end that Vivado process.
Start a new batch process for implementation:

```tcl
open_checkpoint completion-synth.dcp
opt_design
place_design
phys_opt_design
route_design
report_route_status -file route.rpt
report_timing_summary -report_unconstrained -file timing.rpt
report_drc -file drc.rpt
write_checkpoint -force completion-routed.dcp
```

This recipe passed for the small completion component. The full-board result
must be checked separately, including its board and IP constraints.

### Wine installation finding

The installer reported success but created malformed names such as `tps?`
instead of usable shared-folder links. The expected `Vivado/data`, `Vivado/tps`
and `Vivado/gnu` directories were absent, while their contents existed under
the parent `2025.2` directory. The Visual C++ post-install command also failed
because its expected path did not exist.

Creating POSIX symlinks restored `-version` output, but batch startup still
failed loading `xv_coretasks` before reading the test design. Replacing those
three newly created links with ordinary directories, using APFS copy-on-write
copies of the corresponding parent directories, allowed batch Tcl startup and
the synthesis licence check. The original shared directories and malformed
installer entries were left intact. The vendor VC++ runtime check then returned
zero. No executable or licence check was patched or bypassed.

This is a local installation workaround, not a firmware modification. Reproduce
it only after confirming the same layout issue; do not overwrite an existing
valid installation or assume symlinks are universally unsupported by Wine.

The default compiler found on the Mac reports GCC 16.2.0, but its `libc.a`
lookup returned the unresolved filename and its newlib-header probe failed.
That PATH compiler is not a usable firmware toolchain. An isolated Arm GNU
13.2.Rel1 installation was subsequently verified against Arm's published SHA-256
and used successfully for the baseline. Finding an executable named
`arm-none-eabi-gcc` is insufficient; select the toolchain explicitly.

## Rebuild sequence

Retrieve the pinned source into a dedicated checkout:

```sh
git clone --filter=blob:none --no-checkout https://github.com/Xanxi-Amiga/XACP-ZZ9000.git xacp-rebuild
cd xacp-rebuild
git sparse-checkout init --cone
git sparse-checkout set firmware/source/XX19d-RIX
git checkout --detach 6e59edbaf24e0d8d2f29cdd31667a10e16c5b283
```

The paths below are relative to `firmware/source/XX19d-RIX`.

Perform each stage in a separate working copy, preserving the pinned source
and its hashes. The commands below describe the intended flow; they have not
been validated as an end-to-end recipe in this environment.

### 1. Reproduce the unchanged FPGA baseline

Open `fpga/XACPGPU_G4C/ZZ9000_proto/ZZ9000_proto.xpr` with Vivado 2025.2.
The top module is `zz9000_ps_wrapper`. Keep the accompanying `ip_repo`, RasterIX,
Float and third-party RTL in their original relative locations.

In Vivado Tcl, after opening the working-copy project:

```tcl
update_ip_catalog
report_ip_status -file ip-status.rpt
open_bd_design [get_files */zz9000_ps.bd]
validate_bd_design
generate_target all [get_files */zz9000_ps.bd]
update_compile_order -fileset sources_1
launch_runs synth_1 -jobs 4
wait_on_run synth_1
```

Inspect the run status and synthesis log before proceeding. Resolve missing or
locked IP against the pinned project; do not automatically upgrade every IP.
Then implement through routing:

```tcl
launch_runs impl_1 -to_step route_design -jobs 4
wait_on_run impl_1
open_run impl_1
report_timing_summary -report_unconstrained -file timing-summary.rpt
report_clock_interaction -file clock-interaction.rpt
report_cdc -file cdc.rpt
report_drc -file drc.rpt
report_route_status -file route-status.rpt
report_utilization -file utilization.rpt
```

Review setup/hold timing, unconstrained paths, clocks, CDC, routing and DRC before
generating a bitstream. A run returning without a Tcl exception is not timing
closure. Preserve `zz9000.xdc`; a different Zynq-7020 board's pins are unsuitable.
After the reports are accepted, generate the bitstream and export the matching
hardware platform for the software stage. Record all tool/IP versions and hashes.

### Local job-manager limitation

The first full-project attempt used `launch_runs synth_1 -jobs 2`. The run
manager queued jobs and created two empty begin markers, but no child synthesis
logs or child Vivado processes were observed during the check. That attempt was
stopped locally; it does not establish a failed RTL synthesis.

A separate copy completed [global synthesis](https://docs.amd.com/r/2025.2-English/ug892-vivado-design-flows-overview/Synthesis):

```tcl
set_property SYNTH_CHECKPOINT_MODE NONE [get_files */zz9000_ps.bd]
generate_target all [get_files */zz9000_ps.bd]
update_compile_order -fileset sources_1
synth_design -top zz9000_ps_wrapper -part xc7z020clg400-1
```

This changes the build flow from per-IP checkpoints to whole-design synthesis;
it does not change the supplied RTL. The synthesis stage completed with zero
errors, zero critical warnings and 1,176 warnings, then wrote the complete
checkpoint. This stage summary excludes critical warnings during earlier BD
parsing and later XDC/debug processing; the full-run record retains those.

The outer Python timeout wrapper failed after Vivado logged completion: its
resumed kqueue wait reported the old deadline and process-group cleanup raised
an exception. A clean wrapper exit is not claimed. A **fresh Vivado process**
subsequently opened the checkpoint and completed the independent audit with
exit 0. See the [synthesis record](../amiga/dosbox/glide/validation/fence-audit/baseline-global-synthesis.json)
and [checkpoint audit](../amiga/dosbox/glide/validation/fence-audit/baseline-synth-audit.json).

The synthesized baseline uses 22,993/53,200 LUTs (43.22%), 28,362/106,400
registers (26.66%), 60.5/140 BRAM tiles (43.21%) and 22/220 DSPs (10%). Two
debug blocks, `u_ila_0` and `dbg_hub`, were black boxes at that stage. AMD documents that
[debug cores are implemented during optimization](https://docs.amd.com/r/en-US/ug835-vivado-tcl-commands/implement_debug_core).
These counts therefore do not establish final free resources for a Copper unit.

During `opt_design`, Wine's generated Windows JScript runner left both debug
IP synthesis jobs queued without launching Vivado. A small probe reproduced
the runner stopping at its COM environment-property assignment. Bypassing
that line alone still failed during runner initialization. The workaround
executes each **unchanged generated synthesis Tcl** through `vivado.bat`, from
that run's directory, with the same arguments:

```text
vivado.bat -log <core>.vds -m64 -product Vivado -mode batch \
  -messageDb vivado.pb -notrace -source <core>.tcl
```

This replaces process launch and completion reporting, not RTL or synthesis
commands. A successful handoff requires a real exit code of zero, a newly
produced nonempty `<core>.dcp`, and Vivado's own
`__synthesis_is_complete__` file. Only then may the wrapper produce the same
`.vivado.end.rst` marker as `ISEWrap.js`; otherwise it reports failure. Existing
checkpoints or completion markers must be rejected to prevent stale results
from satisfying this check. Never unblock a waiting run by creating a success
marker before synthesis. Both debug cores completed with exit 0 and verified
checkpoints; the waiting full-board optimization then completed with exit 0
and zero remaining black boxes. Placement and routing subsequently completed
from that checkpoint; no bitstream generation or flashing is queued.
The [debug-core record](../amiga/dosbox/glide/validation/fence-audit/baseline-debug-cores.json)
contains the generated-script and checkpoint hashes with each exit status.

The board XDC initially could not resolve `ZORRO_E7M_IBUF`; that net exists in
the completed checkpoint. False-path rules at lines 289/292 also referenced
MMCM `CLKOUT1`. Its pin exists, but no generated clock is reported for that
output; its source wire `e7m_shifted180` has no consumers. Preserve the baseline
constraints and inspect their effective properties during implementation.
Several debug connections also issued warnings during XDC processing.

The independent synthesized-design DRC has 41 warnings and one advisory,
without errors. Unrouted timing has setup WNS -0.454 ns and hold WHS -0.374 ns;
this is **not timing closure**. Placement, routing, bus-skew/CDC checks and final
timing review remain necessary. No new
bitstream or BOOT image has been produced. Reports:
[utilization](../amiga/dosbox/glide/validation/fence-audit/baseline-global-utilization.rpt),
[DRC](../amiga/dosbox/glide/validation/fence-audit/baseline-synth-drc.rpt),
[clocks](../amiga/dosbox/glide/validation/fence-audit/baseline-synth-clocks.rpt),
[timing](../amiga/dosbox/glide/validation/fence-audit/baseline-synth-timing.rpt).

After debug-core implementation and optimization, utilization is 27,117 LUTs
(50.97%), 34,798 registers (32.70%), 80.5 BRAM tiles (57.50%) and 22 DSPs (10%).
This is an optimized **unrouted** netlist, not final timing or usable headroom.
The optimization stage reported zero errors and zero critical warnings, with
74 warnings. The separate DRC still reports 41 warnings and one advisory.

The CDC report also flags three critical clock-domain groups, containing
23 unsafe and 173 unknown endpoints in total. These are tool classifications
in the unchanged baseline, not proof of a new fault or proof of safety.
False-path exceptions do not establish correct synchronization. Review their
handshakes and source constraints before treating a rebuilt image as validated.
No warning waivers or clock exceptions were added for this build.

Evidence: [optimization record](../amiga/dosbox/glide/validation/fence-audit/baseline-optimize.json),
[log](../amiga/dosbox/glide/validation/fence-audit/baseline-optimize.log),
[utilization](../amiga/dosbox/glide/validation/fence-audit/baseline-opt-utilization.rpt),
[DRC](../amiga/dosbox/glide/validation/fence-audit/baseline-opt-drc.rpt),
[CDC](../amiga/dosbox/glide/validation/fence-audit/baseline-opt-cdc.rpt).

### Routed baseline and remaining timing coverage

The full-board placement, physical optimization and routing process exited 0.
All **54,909 routable nets** are fully routed, with zero routing errors.
Under the supplied constraints, setup WNS is **+0.156 ns**, hold WHS
**+0.010 ns**, and pulse-width slack **+3.000 ns**, with no failing endpoints.
The routed design uses 25,762 LUTs (48.42%), 34,798 registers, 80.5 BRAM tiles
and 22 DSPs. These figures describe the unchanged baseline, not an integrated
Glide completion circuit or Copper accelerator.

This is **not complete timing sign-off**. `check_timing` reports 566 clock pins
without a timing clock, 1,016 unconstrained internal endpoints, 76 ports without
input delays and 75 without output delays. DRC reports 52 warnings and one
advisory, without errors. Methodology reports 570 critical warnings, of which
566 concern the missing clock; there are also 112 warnings. The bus-skew report
contains both evaluated paths and `NA` entries; it must not be described as a
complete pass.

Source review identifies two specific issues to investigate:

* The supplied `audio_clock.v` samples `bclk_in` through `clkbuf` on the 100 MHz
  fabric clock. `clkbuf[2]` then clocks the audio formatter/transmitter/receiver.
  The board constraints define the external `I2SO_BCLK` clock but no clock at
  that sampled output. Sampling delay and jitter need modelling; assuming an
  ideal fixed-phase divider would not establish the real relationship.
* The clock-wizard IP is configured with `PRIM_SOURCE` set to
  `Single_ended_clock_capable_pin`, while the block design actually connects its
  input to PS7 `FCLK_CLK0`. Its generated XDC creates another primary clock on
  `clk_in1`, producing the reported clock-tree redefinition. Changing the IP
  input-source configuration is a candidate correction, not a tested fix yet.

The original routed checkpoint and reports are preserved. Constraint experiments
use separate files, without adding false-path waivers to conceal these findings.
AMD's [clock-definition guidance](https://docs.amd.com/r/2025.1-English/ug903-vivado-using-constraints/About-Generated-Clocks)
distinguishes internal generated clocks from independent primary clocks.

Evidence: [route record](../amiga/dosbox/glide/validation/fence-audit/baseline-place-route.json),
[route status](../amiga/dosbox/glide/validation/fence-audit/baseline-routed-status.rpt),
[timing](../amiga/dosbox/glide/validation/fence-audit/baseline-routed-timing.rpt),
[methodology](../amiga/dosbox/glide/validation/fence-audit/baseline-routed-methodology.rpt),
[bus skew](../amiga/dosbox/glide/validation/fence-audit/baseline-routed-bus-skew.rpt).

The subsequent [clock-inheritance experiment](../amiga/dosbox/glide/experimental/clock-audit.md)
passed its unchanged-constraint control and preserved setup/hold margins after
removing the duplicate clock definition. TIMING-4/27 disappear, while two
TIMING-47 warnings expose existing false paths between related clocks. Missing
audio timing and unsafe/unknown CDC counts remain unchanged. The candidate is
not promoted; no modified design checkpoint or firmware was saved.

### 2. Regenerate and verify processing-system initialization and FSBL

The source snapshot does not contain `ps7_init.c`, `ps7_init.h`, an exported
XSA/HDF, a compiled FSBL or an FSBL Makefile. The FSBL calls `ps7_init()` and
`ps7_post_config()`, so the missing generated initialization is a real build
prerequisite.

The supplied FSBL `ps7_parameters.xml` says `PCW_USE_S_AXI_HP3=0`; the current
FPGA design uses HP3 for RasterIX. Treat that XML as stale metadata, not as a
valid regeneration input. Generate initialization from the reviewed current
hardware export. Verify DDR, MIO, clocks and HP3 enablement/64-bit configuration,
and retain any required FSBL hooks. The filename `FSBL_exec_HP3_64.elf` alone
does not establish correct initialization. Compare against the published FSBL
provenance and document the remaining reproduction differences.

### 3. Build the BSP and ARM application

The snapshot includes standalone BSP 6.8 sources and headers but omits compiled
archives. Regenerate or rebuild `libxil.a` and `libxilffs.a` with a compatible
hard-float toolchain before linking `ZZ9000OS`. Preserve the vendor APIs used
by this firmware; migrating to a different BSP is a separate tested change.
Create the missing BSP `lib` output directory before using its Makefile.

The application Makefile hard-codes a Windows Arm GNU 13.2 Rel1 path. Override
the tool variables explicitly. For a configured newlib toolchain, from the
`ZZ9000OS` directory:

```sh
make CC=arm-none-eabi-gcc OBJCOPY=arm-none-eabi-objcopy \
     SIZE=arm-none-eabi-size PYTHON=python3
```

This command still depends on the rebuilt BSP archives and an appropriate PATH.
Preserve these upstream choices when establishing the baseline:

- `-mcpu=cortex-a9 -marm -mfpu=neon -mfloat-abi=hard`, general `-O2`.
- `usb/ulpi.c` at `-O0`, the documented compiler workaround.
- `zz_image.c` at `-Os` with `ZZIMAGE_ENABLE_DECODE=1`.
- `CONFIG_USB_EHCI_TXFIFO_THRESH=4` and the existing USB definitions.
- `Xilinx.spec`, `lscript.ld`, boot-ROM resource generation and library order.

Retain the low-DDR linker assertions, including the 64 KiB margin before the
framebuffer, and the high-DDR service boundaries. Save the ELF, map, size output
and section addresses. Do not remove a linker guard to make an enlarged image fit.

### 4. Package only mutually compatible outputs

The installed Bootgen 2025.2 starts successfully and returns zero for `-help`.
This verifies tool availability only; no new BOOT image has been packaged.
See the [tool check](../amiga/dosbox/glide/validation/fence-audit/bootgen-availability.json).

Place the reviewed FSBL, bitstream and firmware ELF in a dedicated staging
directory under the names in the BIF. With the matching BIF beside them:

```sh
bootgen -arch zynq -image BOOT_XX19d-RIX.bif -o BOOT_GLIDE_EXPERIMENTAL.bin
```

Use a distinct experimental filename and build identifier. Retain a manifest
containing the source revision, patches, tool versions, build flags, timing
reports and hashes of all three partitions and the resulting image. Packaging
is not hardware validation. Flashing and recovery are separate steps.

## Verified ARM baseline build

The unchanged ARM application and BSP built with **Arm GNU 13.2.Rel1** for
macOS ARM64, matching the compiler release named in the upstream Makefile.
The official archive SHA-256 is
`39c44f8af42695b7b871df42e346c09fee670ea8dfc11f17083e296ea2b0d279`.
All 475 supplied files in the application/BSP directories remained unchanged.

From a fresh copy of `arm/ZZ9000_proto.sdk`, with `ARM_TOOLCHAIN_BIN` set to the
selected toolchain's absolute `bin` directory:

```sh
mkdir -p zz9000_bsp_new/ps7_cortexa9_0/lib
PATH="$ARM_TOOLCHAIN_BIN:$PATH" make -C zz9000_bsp_new -j1
make -C ZZ9000OS -j4 \
  CC="$ARM_TOOLCHAIN_BIN/arm-none-eabi-gcc" \
  OBJCOPY="$ARM_TOOLCHAIN_BIN/arm-none-eabi-objcopy" \
  SIZE="$ARM_TOOLCHAIN_BIN/arm-none-eabi-size" PYTHON=python3
```

Keep BSP archive generation serial: its component Makefiles update the shared
`libxil.a`. Both build commands returned zero. The linked ELF has no undefined
symbols and declares ARMv7, VFPv3/NEON and the hard-float calling convention.
Its low-memory end is `0x001C1C6C`, leaving **189,332 bytes** before the unchanged
`0x001F0000` safety boundary. The large BSS reported by `size` includes the
upstream `NOLOAD` heap reservation; it is not the boot-file payload size.

See the [build record](../amiga/dosbox/glide/validation/fence-audit/arm-baseline-build.json)
and [ELF attributes](../amiga/dosbox/glide/validation/fence-audit/arm-baseline-attributes.txt).
This establishes an ARM ELF build only, not an FSBL, FPGA or bootable BOOT image.
The debug-enabled BSP also embeds build paths, so archive/ELF hashes need not
match between checkout locations.

Existing warnings remain: BSP assertion macros, macro redefinitions and an RWX
load-segment warning, among others. A first attempt with Arm GNU 15.3 rejected
legacy BSP code; using the recorded 13.2 release allowed an unchanged baseline.
No warning suppression or vendor-source patch was added for these builds.

## FSBL candidate with regenerated initialization

A separate FSBL candidate now compiles and links using the supplied FSBL sources,
the newly generated `ps7_init.c`/`.h`, Arm GNU 13.2.Rel1 and the rebuilt BSP.
No supplied FSBL source was patched. Its ELF has no undefined symbols, entry
point `0x00000000`, and load regions within the supplied OCM linker layout.
An RWX load-segment warning remains. This is not a reproduction of the released
`FSBL_exec_HP3_64.elf`, and it has not been boot-tested.

The tested flags were Cortex-A9 ARM mode, VFPv3, hard-float, `-Os -g -Wall`.
For a separate copy of `ZZ9000FSBL`, set `PS7_DIR` to the generated processing
system IP directory, `BSP_DIR` to the rebuilt `ps7_cortexa9_0` directory and
`ARM_TOOLCHAIN_BIN` to the selected compiler's `bin` directory. Then:

```sh
set -e
cp "$PS7_DIR/ps7_init.c" "$PS7_DIR/ps7_init.h" src/
mkdir -p build
for source in src/*.c src/*.S; do
  name=${source##*/}
  "$ARM_TOOLCHAIN_BIN/arm-none-eabi-gcc" \
    -mcpu=cortex-a9 -marm -mfpu=vfpv3 -mfloat-abi=hard -Os -g -Wall \
    -I"$BSP_DIR/include" -Isrc -c "$source" -o "build/${name%.*}.o"
done
"$ARM_TOOLCHAIN_BIN/arm-none-eabi-gcc" \
  -mcpu=cortex-a9 -marm -mfpu=vfpv3 -mfloat-abi=hard \
  -specs=src/Xilinx.spec -Wl,-build-id=none -Wl,-T,src/lscript.ld \
  -Wl,-Map,build/FSBL.map -L"$BSP_DIR/lib" -o build/FSBL.elf build/*.o \
  -Wl,--start-group -lxil -lxilffs -lgcc -lc -Wl,--end-group
```

See the [candidate record](../amiga/dosbox/glide/validation/fence-audit/fsbl-candidate-build.json)
and [load segments](../amiga/dosbox/glide/validation/fence-audit/fsbl-candidate-segments.txt).
Review initialization, boot behaviour and the original FSBL differences before
using this candidate in any boot image. It does not replace the live firmware.

## Changes needed for our completion path

The published design has one TMU, a 64-bit HP3 memory path and a 100 MHz GPU
fabric clock. Its CPU peripheral ingress uses 25 MHz and an existing AXI clock
converter. ARM CPU frequency is separate; do not change fabric or DDR clocks
to match an ARM overclock setting. See the [clock and integration audit](../amiga/dosbox/glide/experimental/integration.md).

| Area | Published behaviour | Required experimental work |
| --- | --- | --- |
| RasterIX parser | Based on upstream `9269a01c9bd4c3342bfa70ef8e3cdd751c15c6d0`; lacks our candidate fence | Port reviewed fence/drain behaviour without losing Xanxi's command-read-credit and board integration changes |
| Response path | Command-response ready is tied high; no identity receiver | Wire the tested epoch/sequence receiver and mailbox on the GPU clock |
| CPU access | Existing GPU ID and FIFO mappings are occupied | Add a reviewed address segment, complete decode and AXI crossing; do not alias existing registers |
| Memory errors | Existing submission completion is FIFO transmit-complete | Observe AXI errors and actual outstanding-work retirement; preserve fault quarantine |
| Firmware validator | Single wrapped payload; restricted render features | Add an explicit capability/ABI for validated fence/identity packets; keep old clients working |
| Guest renderer | Diagnostic overlay reports no board | Connect drawing, textures, state and presentation only after completion is reliable |

Current `main.c` exposes GPU query `0x0600`, fixed test `0x0601` and submit
`0x0603`. Its submit ABI is 1.0, and it checks FPGA interface version 2.0.
`xacpgpu_submit_idle_poll()` reports success after FIFO transmission; this is
not proof that every pixel write has retired to DDR. Do not equate XACP DONE
with our proposed tagged render-completion acknowledgement.

The published hardware GPU aperture is ARM physical `0x06600000`–`0x06800000`.
Firmware reserves its final 64 KiB for command staging and validates render/
texture data in the lower `0x1F0000` bytes. These are published service boundaries,
not new application allocations. Keep the memory map, firmware validator and
FPGA enforcement consistent. The current validator also rejects depth, stencil
and TMU1 enablement; adding a fence alone does not implement full Glide support.

## ZZUAE Copper acceleration opportunity

This is a proposed follow-on experiment, not an enabled firmware feature.
RasterIX is a [fixed-function pixel renderer](https://github.com/ToNi3141/RasterIX),
not a programmable compute/shader core. Its texture unit, 100 MHz fabric clock
and 64-bit HP3 connection do not implement Amiga Copper MOVE/WAIT/SKIP,
custom-register effects or chipset DMA arbitration. Core0 is an ARM CPU;
RasterIX executes in FPGA logic, separately from either ARM core.

There are two distinct paths:

| Path | Work to accelerate | Work that remains in ZZUAE initially |
| --- | --- | --- |
| RasterIX drawing | Batched colour spans and suitable composition after guest display events are resolved | Copper execution, virtual beam timing, CPU/register ordering, DMA and Paula timing |
| Dedicated FPGA Copper companion | Copper decode, comparisons and bounded state transitions in new RTL beside RasterIX | Authoritative chipset scheduling and register side effects until an equivalent integrated design is demonstrated |

Start with the drawing path after the completion protocol is validated. Reuse
its tagged batch completion and buffer-ownership rules, preserve event positions
within each scanline, and avoid CPU readback of GPU-rendered pixels where the
presentation path permits it. Retain the cached ARM renderer as the fallback.
Rendering a completed image with a GPU does not remove the cost of executing
its Copper list or guarantee faster whole-emulator operation.

The existing ZZUAE experiments already provide useful controls: GPU drawing
followed by CPU readback/conversion lost against cached CPU drawing, while
synchronous Core0 Copper transitions also added overhead. Neither should be
reintroduced as an assumed optimization. Use complete-scene measurements,
including command preparation, synchronization, presentation and audio gaps.

For a dedicated FPGA unit, first simulate the existing extracted Copper state
machine against the UAE reference. A request per Copper cycle would recreate
the handoff problem, so useful batches must end at a proven dependency boundary.
CPU edits to Copper lists, COPJMP, DMA enable changes, blitter waits, interrupts
and side-effecting register writes require ordering with the virtual chipset.
A prefetched list cannot be replayed blindly. FPGA execution must follow guest
cycle grants, not the monitor's physical scanout clock or raw 100 MHz clock.
This does not provide a complete AGA implementation.

Before allocating FPGA resources, obtain full-design utilization and routed
timing, including the completion changes. A separate Copper unit would compete
for logic, memory/interconnect bandwidth and timing margin; its size and gain
have not been measured. Keep the existing GPU configuration unchanged while
establishing the baseline. Do not assign a new opcode, register address or DMA
region without checking the firmware map and compatibility contract.

Acceptance requires PAL/NTSC event-trace and pixel parity, CPU list edits and
restart cases, blitter/DMA interactions, clean cancellation and buffer release,
then matched State of the Art, Acid in the House and AGA scene measurements.
Report whole-emulator elapsed time and audio underruns alongside renderer time.
There is no Copper-on-FPGA performance claim at this stage.

The first [resolved-image span experiment](../amiga/dosbox/glide/experimental/copper-spans.md)
reconstructed three RGB565 captures exactly. Acid's menu and the Banshee briefing
exceeded its 128 KiB drawing-command budget; a State of the Art silhouette fit.
This rejects universal whole-frame colour-span replacement under that budget,
not GPU acceleration in general. It is an offline representation test, not a
Copper trace, GPU execution or measured speed gain.

The selective follow-up keeps smaller regions on the CPU. At a 256-pixel
minimum area, candidate regions cover 37.34%, 76.83% and 63.20% respectively,
using 1,636, 3,484 and 5,892 drawing bytes. All reconstruct exactly with their
CPU remainder. Eight tests and three new mutation controls pass. These
coverage figures justify a future event-derived drawing test; they are not
percentages of emulator work saved. Production rendering remains unchanged.

## Additional source-review finding

`fpga/test/RUN_G4C_UNIT_SIM.tcl` references `fpga/rtl/XACPCommandReadCredit.v`,
which is absent in the distribution. The module is actually at
`fpga/RasterIX_G1/RasterIX/rtl/RasterIX/XACPCommandReadCredit.v`.
Correct the test harness path in a working copy before running that test. This
is separate from the XPR, whose 100 source references all resolve.

Using the correct RTL path, the unmodified upstream testbench passed under
Icarus Verilog 13.0: nine bursts / 72 beats, stalled consumer, full FIFO and
ordered RID/RRESP/RLAST. Both compilation and simulation returned zero. The
[recorded result](../amiga/dosbox/glide/validation/fence-audit/upstream-credit-check.json)
includes source hashes. This tests command-read-credit behaviour only; it does
not simulate the complete GPU, HP3 or physical board.

## Video and audio clock follow-up

The [crossing audit](../amiga/dosbox/glide/experimental/crossing-audit.md) records
detailed routed CDC findings, audio synchronizer register properties and nine
passing digital tests of the unchanged sampled audio clock. Both deliberately
broken simulation controls were detected. Sampling quantises output periods to
the 100 MHz clock grid; this is not a hardware audio-quality measurement.

The baseline's 75 MHz video timing does not cover all firmware-programmed display
modes. Active frame/line transfers also need functional review independently of
long-held configuration signals. No crossing exceptions were added, and no RTL
or deployed firmware changed during this audit.

## Evidence required before promotion

First establish the unchanged full build, then compare an experimental build
with only the intended patches. Run the component suite, full-path simulations,
reset/error/backpressure tests and implementation checks. Hardware tests must
prove quiescence, cache visibility, repeated submissions and clean cleanup before
gameplay benchmarking. A timeout or reset does not authorize releasing memory
still accessible by DMA. No accelerated GTA FPS has yet been established.

## Credits and licensing

XACP and this ZZ9000 integration are by **Xanxi**. RasterIX is by **ToNi3141**;
the ZZ9000 hardware and original firmware are from **MNT Research**. Preserve
their notices, the bundled component licences and AMD/Xilinx BSP notices.
Upstream [LICENSES.md](https://github.com/Xanxi-Amiga/XACP-ZZ9000/blob/6e59edbaf24e0d8d2f29cdd31667a10e16c5b283/firmware/source/XX19d-RIX/LICENSES.md)
describes the mixed licensing; the SDK's 0BSD markings do not relicense the
whole firmware. A distributed modified firmware needs its corresponding source,
build instructions and applicable licence notices.
