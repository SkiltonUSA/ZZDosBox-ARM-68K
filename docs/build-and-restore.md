# Build and restore

This backup contains current development source and launcher 0.19.8-dev, the historical 0.8
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
sources and hashes are in `amiga/common/libc/libc-sources.json`. Rebuild it with
`scripts/build_dosbox_libc.py` if needed. Shared A9 support lives in
`amiga/common/arm`, libc configuration in
`amiga/common/libc`, and their native checks in `tests/amiga/common`.
These are DOSBox build dependencies; no NES/SNES core or FFmpeg decoder is bundled.
Archived build records retain their original source paths and hashes.

Keep the default O2 CPU optimisation. O3 and the two scheduler-cache experiments
showed no consistent gain; see the scheduler records. Early timing records measure startup refresh throughput. Later Doom Demo3
records use the physical A9 timer and report real game FPS.

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

## Historical dynrec and scanline backups

The earlier DOS executable is `artifacts/development/neon-dynrec/pc/ZZDosBox-Dynrec`.
The optional scanline test is in `artifacts/experiments/scanline-copy/pc/`.
Both retain ELF symbols, build metadata and integration patches. The normal
launcher installation uses the filename `ZZDosBox`; the scanline build is
experimental and has not replaced it. Windows uses its separate saved runtime.

Restore inputs using `python3 scripts/restore_build_inputs.py --neon-build`
or `--scanline-build`. Generated outputs are preserved, so use a fresh checkout
for independent restoration. These flags restore their distinct model directories.
After configuring the toolchain above, rebuild with:

```sh
python3 scripts/build_dosbox_dos_arm.py --pc --pc-dynrec --pc-jit-mib 4 \
  --pc-pipeline --pc-neon --pc-doom-clock
# Add --pc-scanlines for the separate experiment.
```

Run the saved scanline ELF model checks with a Unicorn-enabled Python:

```sh
python tests/amiga/dosbox/check_dynrec_arm.py \
  --out .context/amiga/dosbox-dynrec-4m-neon-pipe-scan
```

Normal tuned output uses `.context/amiga/dosbox-dynrec-4m-neon-pipe` instead.
These flags do not install files or run anything on the Amiga.

## Current Direct runtime: PIC VFP

The current measured runtime is in `artifacts/development/pic-vfp/pc`.
Restore it with `python3 scripts/restore_build_inputs.py --pic-vfp-build`, then
use `make build-current` with the toolchain configured above. The explicit
`--pc-pic-vfp` flag preserves the old soft-float control when omitted.
[Measurements, safety contract and model checks](performance-pic-vfp.md).

## Network launcher 0.17.0-dev

The measured incremental update is in `artifacts/development/network-0.17.0`.
See its README before installing. Existing Direct and Windows binaries remain
separate. No personal profiles or licensed guest software are included.

For source builds, restore the normal build inputs first. The network source
preparer fetches and verifies libslirp and NE2000 pins. For offline preparation,
copy `sources/libslirp-4.9.5.tar.bz2` to
`.context/amiga/dosbox-network/libslirp.tar.bz2` and the two files in
`sources/ne2000/` to that same directory as `upstream-ne2000.cpp` and
`upstream-ne2000.h`.

```sh
make test-network
python3 scripts/build_dosbox_dos_arm.py --pc --pc-network --pc-disk-cache-kib 1024
python3 scripts/build_dosbox_launcher.py
```

Configure the ARM and 68K containers as described above. The dedicated NAT
backend builder currently retains the original Podman connection name; adapt
that invocation for your host. Build outputs are local and never deploy automatically.
Use the [Network guide](amiga-dosbox-network-ui.md) to stage optional guest tools.

## Latest incremental artifacts

[Launcher 0.19.7-dev](../artifacts/development/launcher-0.19.7/) adds disk creation
and sidebar UI updates. [The TCP/IP/IPX runtime](../artifacts/development/network-0.18.0/)
is a separate interpreter build. The [console-sync candidate](../artifacts/experiments/console-sync/)
is preserved for review and has not replaced the measured Direct default.

Run `make test-host-review` for console ring, presentation and network receive
sanitizer checks. `python3 scripts/test_dosbox_image_create.py` checks image creation.

[Launcher 0.19.8-dev](../artifacts/development/launcher-0.19.8/) adds offline help.
Copy both the launcher and `Network.guide` beside the existing 0.19.7 support
files. The build generates the guide from `docs/amiga-dosbox-network-setup.md`.
