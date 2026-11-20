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

See the [measurement contract](../../experimental/measurement.md). The
independent [FPGA routing report](../board-hp3/routing/result.json) records a
failed setup constraint; passing software/model tests do not waive that failure.
