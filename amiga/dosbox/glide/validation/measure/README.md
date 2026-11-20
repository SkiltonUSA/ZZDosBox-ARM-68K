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
  components (567 parameters). The matching FPGA bitstream is still absent;
  no new boot image has been packaged.
- `boot-roundtrip.json`: native Bootgen reproduces all five existing partition
  payloads, load/entry addresses, attributes and the BootROM header unchanged.
  The complete file differs because three raw ARM payloads have separate image
  headers instead of one ELF image header. This checks the packaging tool with
  the existing FPGA payload; it does not validate a new FPGA bitstream.

See the [measurement contract](../../experimental/measurement.md). The
independent [FPGA routing report](../board-hp3/routing/result.json) records a
failed setup constraint; passing software/model tests do not waive that failure.
