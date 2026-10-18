# Descent, Doom, Quake and Duke Nukem 3D startup review

## Findings

- **Descent:** DOS/4GW Professional 1.97 failed with errors 2504 and 1011
  before starting the game. The cause was in ZZDosBox's filesystem adapter,
  not in the supplied executable. `RemoteDrive::FileExists()` passed a missing
  file's `FS_STAT` error into the DOS error state. DOSBox's temporary-file
  routine probes an unused filename, creates it, then checks that error state;
  the stale error made a successful create appear to fail. Preserve the DOS
  error state around this predicate, matching upstream `localDrive::FileExists`.
- **Doom 1.9 shareware:** the existing profile starts and renders its attract
  demo on the A4000TX. No startup correction was needed. That profile has sound
  disabled; this test does not establish audio compatibility.
- **Duke Nukem 3D 1.3D shareware:** the 386 profile reproducibly raises exception
  06h at `180:0026A902` during palette/lookups loading. Selecting the existing
  **486** CPU option gets past that failure in both native and physical tests;
  the Amiga reaches the main menu with a rendered game scene behind it.
  This selects the guest instruction set; it does not claim 486-class speed.
- **Quake 1.06 shareware:** x87 is enabled in the Compat runtime. The physical
  test reaches the graphics console and main menu with a rendered demo behind
  it. Startup includes a CD-audio warning requiring a keypress; `-nocdaudio`
  suppresses that irrelevant CD check while retaining Sound Blaster audio.
  Startup success is not evidence of smooth gameplay.

## Configuration

Use the corrected Expanded runtime for Descent (16 MiB, EMS enabled, LOADFIX
and experimental paged JIT disabled) and for Duke (486, 8 MiB, SB16). Quake
uses Compat, 8 MiB, SB16 and `QUAKE.EXE -nocdaudio`. Keep Doom's working
configuration. No game executable or asset needs patching for these changes.

The two Descent installations have different sound configurations. The older
entry has sound and music disabled; the shareware entry uses SBPro2 at
220h / IRQ 7 / DMA 1. These settings are distinct from the temporary-file bug.

## Verification

The new `TEMPTEST.COM` fixture exercises DOS INT 21h/5Ah, checks the returned
name, closes/deletes the file, and verifies that a missing parent directory
still fails. The exact fixture fails on the original adapter and passes with
this fix. The fixture also passes on the A4000TX with a clean shutdown. The ARM
instruction model passes at two relocation bases, including
JIT/SIMD restoration checks. Its root-only filesystem model now rejects a
missing parent instead of silently creating the leaf in its root.

Reproduce the native regression with:

```sh
python3 scripts/build_dosbox_pc.py --compat --x87 --expanded --game tempfile
```

The game test harness accepts `ZZ_GAME_CPU=0`, `1` or `2` for 386, 486 or
Pentium, allowing otherwise identical startup comparisons on private copies.
Native runs do not establish physical performance. Physical tests use the
existing runtime mapping probe and F10 shutdown checks; video callback counts
include repeated frames and must not be reported as game FPS.

File read-ahead/coherence and host console/display handoff tests also pass.

Physical Descent reached its title and existing-pilot selector. A later
transition remained black during the observation interval, so this review does
**not** claim completed physical gameplay verification. The native interpreter
reached the mission briefing both with the existing sound-disabled game
configuration and with an explicit `-nosound` argument. This is a startup
bug fix, not a Descent performance improvement or full compatibility pass.

All completed physical game runs returned through F10 with mapping, SIMD,
register/cache restoration and owned-memory release checks passing. The
configured AHI runs reported no audio delivery errors; this is not a listening
test. The original game folders and saves were not altered by the tests.


The corrected Compat and Expanded binaries and four profile corrections were
installed and read back successfully. Previous binaries and the library were
retained for rollback. Other runtime variants were not replaced in this review.
[Build fingerprints and regression results](../amiga/records/2026-10-18/dosbox-shooters/).

## References

- [Pinned DOSBox core](https://github.com/libretro/dosbox-libretro/tree/a563bb5e9753f207d74e9d8c417d793b784fa4be)
- [Upstream local-drive predicate](https://github.com/libretro/dosbox-libretro/blob/a563bb5e9753f207d74e9d8c417d793b784fa4be/src/dos/drive_local.cpp)
- [DOSBox temporary-file implementation](https://github.com/libretro/dosbox-libretro/blob/a563bb5e9753f207d74e9d8c417d793b784fa4be/src/dos/dos_files.cpp)
- [Open Watcom DOS/4G error descriptions](https://github.com/open-watcom/open-watcom-v2/blob/master/docs/doc/rsi/errors.gml)
