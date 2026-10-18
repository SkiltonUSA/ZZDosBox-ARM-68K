# Descent, Doom, Quake and Duke Nukem 3D startup review

## Summary

| Change | Location | Evidence |
| --- | --- | --- |
| Preserve DOS error state during file-existence checks | ZZDosBox filesystem adapter | TEMPTEST fails before and passes after; fixes DOS/4GW errors 2504/1011 |
| Defer invalid FE-group opcodes until execution | ARM dynamic recompiler | Independent self-modifying-code fixture passes with the fix |
| Run Descent through DOS/32A 7.35 | Per-game launcher command | First level and input work; zero paged fallbacks in the recorded run |

The DOS/32A change is a compatibility workaround, not a general repair to
DOS/4GW paging. The JIT regression was independently verified; it is not
established as the cause of Descent's original failure. The game executable
and data were unchanged. [Tested runtime backup and setup](../artifacts/development/descent-startup/).

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

The original DOS/4GW path reached the pilot selector but later transitions
were slow or remained black during observation. A subsequent physical check
uses **DOS/32A 7.35** as an external loader: `DOS32A DESCENT.EXE`, or
`DOS32A DCNTSHR.EXE` for the supplied shareware archive. The game executable
is unchanged. With 16 MiB, the game reports `VMM:0` and no virtual memory;
the ARM log records **zero paged interpreter fallbacks**, compared with
1,857,429 in the earlier DOS/4GW run. These were different interactive runs,
so the counts are diagnostic evidence, not an FPS comparison.

The DOS/32A run reached the briefing and first playable mine on the A4000TX.
Input changed the viewpoint. The sound-enabled configuration also reached the
level and completed 3,356,672 audio samples with no reported delivery error.
This establishes first-level startup, not complete-game compatibility.

A separate recompiler regression was found while investigating the loader.
Translation lookahead used to abort on an invalid FE-group encoding, even
when preceding guest code overwrote those bytes before execution. The decoder
now ends the block and defers the invalid opcode to execution. The original
`JITSPEC.COM` fixture passes in the native interpreter, fails in the original
ARM model, and passes with the fix at two relocation bases and on hardware.
The hardware pass restores the register/cache witnesses and releases memory.
The original hardware fixture required the launcher's bounded Core1 reset;
it did not complete the normal restoration epilogue.

Early DOS/32A 9.12 trials used an accidentally truncated bridge upload and
are **excluded** from compatibility conclusions. The selected 7.35 binary was
transferred with wget and matched the source MD5 before execution. The JIT
fixture independently establishes the decoder bug; those corrupt-upload runs
do not establish it as the cause of a valid Descent failure.

This product uses DOS/32 Advanced DOS Extender technology. The external
loader and its complete licence accompany the personal Descent installation.

All completed physical game runs returned through F10 with mapping, SIMD,
register/cache restoration and owned-memory release checks passing. The
configured AHI runs reported no audio delivery errors; this is not a listening
test. The original game folders and saves were not altered by the tests.


The corrected Compat and Expanded binaries and four profile corrections were
installed and read back successfully. Previous binaries and the library were
retained for rollback. Other runtime variants were not replaced in this review.
[Build fingerprints and regression results](../amiga/records/2026-10-18/dosbox-shooters/).

The installed Descent profiles now invoke DOS/32A 7.35. Only those two command
fields changed in the existing library; the runtime and library have rollback
copies. [Level-start and decoder regression evidence](../amiga/records/2026-10-18/dosbox-descent/).

## References

- [Pinned DOSBox core](https://github.com/libretro/dosbox-libretro/tree/a563bb5e9753f207d74e9d8c417d793b784fa4be)
- [Upstream local-drive predicate](https://github.com/libretro/dosbox-libretro/blob/a563bb5e9753f207d74e9d8c417d793b784fa4be/src/dos/drive_local.cpp)
- [DOSBox temporary-file implementation](https://github.com/libretro/dosbox-libretro/blob/a563bb5e9753f207d74e9d8c417d793b784fa4be/src/dos/dos_files.cpp)
- [Open Watcom DOS/4G error descriptions](https://github.com/open-watcom/open-watcom-v2/blob/master/docs/doc/rsi/errors.gml)
