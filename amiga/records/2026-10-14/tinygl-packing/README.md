# Dedicated TinyGL packing / triangle-dispatch measurements

See [the report](../../../../docs/amiga-tinygl-packing-experiment.md) for the
method, results and limitations. Four O2 builds differ only in the compiled
`ztriangle` object. All other ARM object hashes match.

Each build directory preserves the executable, ELF, compiler metadata, source
snapshot, patched upstream sources, native and linked-model logs, and three
physical runs. `comparison.json` compares median per-run means of the four equal
timing batches. The four columns in each run repeat that build's fixed routine;
they are not different algorithms. The exact generated images/depth hashes must
match across every build.

`texture.png` is a **native reference rendering**, not an Amiga screenshot.
`dispatch-coverage.log` comes from host-only instrumentation, absent from timed
ARM executables. `dispatch-bbox-only.log` retains the rejected thin-triangle
selector test. Private local path prefixes in text logs are normalised; binary
artifacts and hardware report values are unchanged. `manifest.json` records
SHA256 for each archived file.

The diagnostics were staged in RAM only. These native offscreen measurements do
not establish DOS game FPS or scanout performance and do not promote a candidate
into the installed emulator. The CPU-guide review is a separate configuration
analysis; guest cycle settings were unchanged during these tests.
