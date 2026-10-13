# Build and restore

This is a development backup of 0.8.0-dev, not a finished DOSBox release.
`artifacts/0.8.0-dev/pc/ZZDosBox` matches the installed, tested executable.
The launcher, disk helper and supplied icon are alongside it. Rejected speed
experiments live separately under `artifacts/experiments/`.

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

AmigaOS, MUI 3.8+, Picasso96, ZZ9000 and compatible XACP firmware (tested XX19c).
Disk-image access additionally requires diskimage.device and fat95. Supply
licensed DOS disks yourself. Workbench and DOS must not mount a writable image
simultaneously; use the launcher mount/eject controls. Never replace firmware
or existing system preferences merely to restore this application.

The backup includes source, executable artifacts and selected test evidence.
It excludes user games, Microsoft DOS, credentials and whole-machine data.
