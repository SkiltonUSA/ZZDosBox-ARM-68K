# Counter build and model evidence

- `abort-results.json`: 96 ARM success/fault/mask cases through the patched BSP
  abort frame and return. Physical AXI errors are not injected here.
- `abort-mutant.log`: a deliberately false successful read is rejected.
- `counter-results.json`, `counter.log`: completion-driven counting and a built
  68020 host application. No physical FPS result.
- `counter-mutant.log`: counting pending work is rejected.
- `service-results.json`: five ARM service scenarios with modeled hardware
  inputs; actual command lifecycle, validator and pixel verifier execute.
- `firmware-build.json`: recorded base, source and ELF hashes; preserved Core0
  services and linker-space result. Build only, not installed.

- `boot-inputs.json`: the staged bootloader payload matches the existing boot
  image byte for byte. The candidate and published baseline have identical PS7
  components (567 parameters). This receipt predates the matching FPGA build; see the later
  [bitstream/package evidence](../texture-address/bitstream/result.json).
- `boot-roundtrip.json`: native Bootgen reproduces all five existing partition
  payloads, load/entry addresses, attributes and the BootROM header unchanged.
  The complete file differs because three raw ARM payloads have separate image
  headers instead of one ELF image header. This checks the packaging tool with
  the existing FPGA payload; it does not validate a new FPGA bitstream.

- `package-test.json`: the standalone packager passes with the recorded firmware
  and old FPGA payload. This deliberately incompatible test package is not for
  installation; it proves payload preservation and rejects malformed containers.

- `host-usage.json`: the 68k executable displays its usage screen on the real
  Amiga, returns cleanly, and is removed from RAM. The bridge remains responsive.
  This path sends no GPU request and supplies no FPS measurement.

See the [measurement contract](../../experimental/measurement.md). The
independent [FPGA routing report](../board-hp3/routing/result.json) records a
failed setup constraint for the earlier candidate. The subsequent
[carry-free candidate](../texture-address/routing/result.json) passes declared
setup/hold constraints, with remaining timing-coverage limitations.

- `staging.json`: firmware-writer completion for recovery and candidate images,
  verified source uploads and a responsive Amiga afterward. No bootcard readback
  or GPU execution is claimed. A cold power-cycle is required before measurement.

## Physical completion results

The subsequent [hardware runs](hardware/README.md) pass normal retirement, Core0
pixel verification and clean close. The 1,000-frame batch measures 9,302.194
offscreen FPS at 800 MHz ARM. Earlier staging/build receipts retain their
original scope; they are not current deployment-status statements.
