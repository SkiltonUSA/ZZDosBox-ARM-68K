# TinyGL hidden-pixel rejection evidence

See report.md and fixed-results.json for the final dedicated-build comparison.

- v1: four methods in one binary; three physical runs. Retained as exploratory evidence.
- fixed-scalar: original scalar compiled into all four report columns; three physical runs.
- fixed-hybrid: scalar rejected-prefix + guarded NEON compiled into all four columns; three runs.

Dedicated columns repeat one method: average the four equal-length columns per run,
then take the median of three runs. Do not read them as distinct optimisations.
Only 109-ztriangle.arm.o differs between dedicated builds. All exact binaries,
ELFs, build metadata, source snapshots, native/model logs, closed physical logs,
bridge events and final hardware checks are retained. The installed runtime was
not replaced. These are private-buffer native TinyGL results, not DOS game FPS.
