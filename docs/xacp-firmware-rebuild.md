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
| Vivado | 2025.2, device `xc7z020clg400-1`, Zynq-7000 support; small out-of-context completion module synthesized under Wine 11.0; implementation failed |
| ARM compiler | ARM GNU bare-metal toolchain with newlib, hard-float Cortex-A9 libraries; compiler version must be recorded |
| BSP / FSBL tools | Compatible Xilinx standalone BSP and processing-system initialization generation from the actual hardware design |
| Bootgen | Zynq boot-image packaging; confirm availability separately from the minimal Vivado installation |
| GNU make / Python | Firmware and boot-ROM resource build |

Vivado installation completed after authenticated download of the selected
16.61 GB payload. After the Wine-specific directory repair below, batch mode
loaded the test Tcl, reported version 2025.2, found the device and obtained the
synthesis licence. The out-of-context completion module synthesized successfully
with zero synthesis warnings or errors. Implementation then failed in
`opt_design` with `ERROR: [Synth 20-411]` and no explanatory message; the process
returned 1. Placement and routing were not reached. The cause remains unresolved,
and this result does not establish a defect in Xanxi's firmware. See the
[probe record](../amiga/dosbox/glide/validation/fence-audit/vivado-probe.json).
This small circuit cannot sign off the board design. Wine remains an unofficial
build environment.

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
That PATH compiler is not yet a usable firmware toolchain. Finding an executable
named `arm-none-eabi-gcc` is insufficient.

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

Place the reviewed FSBL, bitstream and firmware ELF in a dedicated staging
directory under the names in the BIF. With the matching BIF beside them:

```sh
bootgen -arch zynq -image BOOT_XX19d-RIX.bif -o BOOT_GLIDE_EXPERIMENTAL.bin
```

Use a distinct experimental filename and build identifier. Retain a manifest
containing the source revision, patches, tool versions, build flags, timing
reports and hashes of all three partitions and the resulting image. Packaging
is not hardware validation. Flashing and recovery are separate steps.

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
