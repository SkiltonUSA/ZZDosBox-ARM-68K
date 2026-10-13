# Build and restore

This backup contains current 0.10.1 development source, the historical 0.8
baseline and a separate experimental Windows build. The Windows build has
passed physical desktop, Graphics WinMark and shutdown checks. See the per-artifact notes before deploying.
The original 0.8 artifacts remain unchanged for recovery.

Run `python3 scripts/verify_backup.py` to check the backup hashes, then
`python3 scripts/restore_build_inputs.py` to populate ignored build inputs.
The latter clones the bundled DOSBox revision locally, restores the saved ARM
ELF and original diagnostic programs, and unpacks the archived picolibc source.
No Amiga connection or hardware changes are performed.

## Checks

On the development Mac, Clang plus Homebrew LLVM and lld are used. Some scripts
retain `/opt/homebrew/opt/llvm/bin` and `/opt/homebrew/opt/lld/bin` paths; adapt
those for a different host. With Python's `unicorn==2.1.4` installed (or use `uv run --with unicorn==2.1.4 python tests/amiga/dosbox/check_pc_arm.py`):

```sh
make test-host
make test-model PYTHON=/path/to/python-with-unicorn
python3 scripts/build_dosbox_pc.py --sanitize
```

The native test defaults to our original EGA/PIT fixture. Game-specific tests
need separately supplied media; they are not part of this backup. Instruction
model tests establish bounds, relocation and protocol behaviour, not physical
hardware execution. Dated hardware logs preserve the actual A4000TX findings.

## Cross-build

This retains the existing development toolchain layout; it is not a turnkey
SDK installer. Provide an ARM GCC/G++ 15.3/binutils container with picolibc
headers and **this checkout mounted at `/work`**. The 68k container is pinned
by digest in `amiga/upstream.json`.

```sh
export DOSBOX_ARM_COMMAND='["podman","exec","zzdosbox-toolchain"]'
export AMIGA_CONTAINER_COMMAND='["podman"]'
python3 scripts/build_dosbox_dos_arm.py --pc
python3 scripts/build_dosbox_launcher.py
```

The launcher icon converter additionally needs ImageMagick (`magick`). The
archived PIC soft-float libc is restored by the preparation script; its member
sources and hashes are in `amiga/pocketsnes/libc-sources.json`. Rebuild it with
`scripts/build_pocketsnes_libc.py` if needed. Historical shared-component paths
are retained to preserve the source layout. Standalone export changes only
container command configuration and two small imported build helpers.

Keep the default O2 CPU optimisation. O3 and the two scheduler-cache experiments
showed no consistent gain; see the scheduler records. Current timing is startup
refresh throughput, not steady gameplay or a full-speed emulation claim.

## Runtime requirements

See [Amiga requirements](requirements.md) for required and optional drivers.

AmigaOS, MUI 3.8+, Picasso96, ZZ9000 and compatible XACP firmware (tested XX19c).
Disk-image access additionally requires diskimage.device and fat95. Supply
licensed DOS disks yourself. Workbench and DOS must not mount a writable image
simultaneously; use the launcher mount/eject controls. Never replace firmware
or existing system preferences merely to restore this application.

The backup includes source, executable artifacts and selected test evidence.
It excludes user games, Microsoft DOS, credentials and whole-machine data.

## Windows experiment

For a fresh checkout, `python3 scripts/restore_build_inputs.py --windows-experiment`
restores the new ARM ELF and original regression fixtures. Existing generated
outputs are preserved, so use a fresh checkout when comparing builds. Run
`make test-model` with a Python environment containing Unicorn 2.1.4. Default
restoration continues to use the historical 0.8 baseline; current tests target
the new build, so rebuild current source before testing a baseline restoration.

The native WfW harness needs separately supplied media and local scratch paths.
Neither Windows nor WinBench is bundled. Native desktop/WinBench startup does
not establish physical ARM performance or a completed benchmark score.

## Current mouse build

In a fresh checkout, restore with
`python3 scripts/restore_build_inputs.py --mouse-build` before running the
current model suite. This selects the mouse-capable ELF and its original
INT33/PS2 fixture; the older Windows experiment predates this fixture.
`python3 scripts/build_dosbox_pc.py --sanitize --game mouse` runs the native
mouse test after preparing the pinned source. No licensed media is needed.
