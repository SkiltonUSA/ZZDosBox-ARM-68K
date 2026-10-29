# Controlled scheduling and JIT follow-up

- Read prior hardware reports and found periodic VERIFY plus snapshot work in timing runs.
- Built private hosts with fixed presentation, only initial verification, no capture; ARM payloads unchanged.
- Restarted local workspace MCP service; bridge alive, Core1 free, saved screen mode CRC 9C80FBC5.
- Planned 660-frame ABBA audio comparisons, 60 warmup/600 measured, window and fullscreen.
- Added bounded eight-instruction DOSBox register reuse behind an explicit experiment flag; original adjacent mode remains selectable.
- Wrote the differential test first; its initial build failed on the missing helper as expected. Passed 6,000 A9 streams (3,104 replacements) and retained 320 adjacent cases.
- New linked x86 fixture covers mixed-width writes, register chains and carry consumers; reference passes at two relocations.
- Completed eight fullscreen console runs; no convincing repeatable earlier-wake gain. Windowed runs underway.
- First DOS build compiled ARM successfully but used an inactive default Podman connection for the 68k link. Retried with the explicit active connection; no hardware ran that incomplete build.
- Finished 16 controlled console runs (10,560 total emulated frames). All presentation/audio/restoration gates passed. Earlier wakeup has no repeatable advantage; kept defaults.
- Completed linked ARM register/partial-register/flags/SMC/checked-memory/paging/x87/SIMD/cancellation checks. New and original register reuse tests pass; header/base-record guards pass.
- Ran physical Duke ABBA at 10,000 cycles: reference 147/147 frames, candidate 148/147 in 30-second samples. Candidate made 5,773/5,772 translation-time replacements, without a useful speed gain. All four runs restored state and released memory.
- Checked source differences and recorded both build manifests. Candidate differs in register-reuse integration; host changes are a compile-time layout assertion and final counter print.
- Archived raw results and analysis in standalone console and DOSBox repositories. Corrected interpretation of earlier verification-heavy elapsed times.
- Removed only SD032G:Dev/ZZSchedule1029 after archiving. Bridge responds, Core1 idle, saved screenmode CRC remains 9C80FBC5. No installed runtime, release, ROM/save or system preference changed.
