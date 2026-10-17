# Startup and disk-copy measurements

See [the report](../../../../docs/amiga-dosbox-load-profile.md) for conditions,
results and limitations. The logs contain bounded physical Amiga runs, not
steady gameplay or WinBench scores. No game or operating-system media is included.

- `comparison-builds.json`: matched Compat builds; source hashes match after
  normalizing the build-directory names. The feature difference is file read-ahead.
- `windows-copy-builds.json`: identical Windows ARM image with two 68k hosts.
- `bulk-copy-candidate.patch`: research candidate; not a production change.
- `measurements.json`: parsed timings and service counters.
- `analyse.py`: recreate the summary from the logs in this directory.

The candidate file service passed the existing native `fs_host_test.c` fixture
with AddressSanitizer and UBSan, including 100 KiB I/O, bounds, truncation,
permissions, link rejection, nested paths, cleanup and exclusive image ownership.
The test-only `bulk-copy-mock/proto/exec.h` supplies Exec CopyMem for that native
fixture. Both physical hosts built with `-Wall -Wextra -Werror`.
