# EMS and Sound Blaster compatibility runtime

`ZZDosBox-Compat` adds DOSBox's EMS 4.0 board interface, XMS services and
Sound Blaster Pro 2 (8-bit), Sound Blaster 16 and PC speaker to embedded DOS sessions. The launcher keeps these
settings per program. Existing profiles retain their previous behaviour;
newly added programs select EMS and Sound Blaster with 8 MB guest RAM.

In **CPU & memory**, enable **EMS expanded RAM**. In **Audio**, select
**Sound Blaster 8-bit (Pro 2)** or **Sound Blaster 16-bit**. The separate
**PC speaker / beep** checkbox enables PIT/port-61 speaker emulation, including
when Sound card is Off. Settings are saved per program.

| Card | Game setup resources | BLASTER environment |
| --- | --- | --- |
| 8-bit / Pro 2 | 220h, IRQ 7, DMA 1; OPL3 music | `A220 I7 D1 T4` |
| 16-bit / SB16 | 220h, IRQ 7, DMA 1, high DMA 5; OPL3 music | `A220 I7 D1 H5 T6` |

Choose the same card in the game's own sound setup. Selecting a card in the
launcher supplies the emulated hardware; it does not rewrite every game's
configuration automatically. This is emulation, not a physical
Sound Blaster or a requirement to install Creative drivers on the Amiga.

AHI v4 or later and a working unit 0 configuration are required for playback.
The application uses that configured output; it does not select ZZ9000AX or
HDMI automatically or change global AHI preferences. Turn both Sound card and PC speaker off
when no AHI output is available. PC speaker sound uses the same AHI output. External CAMD MIDI remains separate.

The first compatibility runtime supports embedded DOS, Direct rendering,
normal/adaptive presentation, scanlines and the dynamic core. Booted
MS-DOS/Windows, networking and Developer tracing/memory tools retain their
existing runtimes. The launcher reports incompatible selections rather than
silently ignoring EMS or sound. XMS is enabled in the compatibility runtime;
EMS can be disabled independently. VCPI, UMBs and DOS=HIGH are not enabled.

Equivalent launcher configuration overrides:

```ini
[dos]
ems=true
[sblaster]
sbtype=sb16
[speaker]
pcspeaker=true
```

These are launcher options; the runtime also accepts `--ems on|off` and
`--sound sbpro2|sb16|off` and `--speaker on|off`. The standard runtime remains
available when EMS, Sound Blaster and PC speaker are all off. The `ZZDOSLIB16`
profile format reads older libraries; older launchers cannot read v16.

Retain the pre-update library with the old launcher when rolling back.

## Implementation

The pinned dosbox-libretro EMS, XMS and guest-memory allocator share one pool
inside the existing guest RAM. EMS maps a 64 KiB page frame; pages are remapped,
not copied across Zorro. The pool starts above the HMA. BIOS extended-memory
reporting is suppressed while these managers own the pool, preventing raw
BIOS allocation from overlapping EMS/XMS handles. Booted DOS does not receive
these embedded services. EMS board mode avoids enabling VCPI or entering V86.

Upstream DMA, Sound Blaster DSP and DBOPL generate PCM on Core1. A deterministic
32 kHz stereo mixer advances on emulated millisecond ticks. Complete 1024-frame
blocks use a bounded SPSC queue in application-owned uncached memory. Samples
are big-endian for AHI; producer and consumer indices occupy separate cache
lines. A separate AmigaDOS audio process owns the AHI requests and local
buffers, with linked asynchronous requests and a two-block prebuffer. Full
queues apply backpressure while preserving cooperative cancellation. Normal
shell exit pads and drains the final partial block; cancellation stops playback.

No firmware changes, static physical DDR allocations, proprietary drivers or
game files are included. Original DOSBox notices are retained in staged sources;
`prepare_dosbox_compat.py` and `prepare_dosbox_audio.py` record integration patches.

## Verification

Original DOS COM tests exercise EMS/XMS shared allocation and release, repeated
page remapping and execution from the remapped address, Sound Blaster DSP reset,
DMA1 transfer and IRQ7 acknowledgement, and OPL register-driven tone generation.
Native AddressSanitizer runs pass and produce nonzero digital and FM PCM.
Physical A4000TX / XX19c tests on 16 October 2026 passed:

| Test | Physical result |
| --- | --- |
| EMS/XMS | Shared allocation/release and 32 repeated page-map/execution cycles passed under ARM dynrec. |
| Sound Blaster | DSP reset, DMA1 and IRQ7 passed; 4,096 stereo frames queued, submitted and completed through AHI, error 0. |
| OPL3 | Register-driven tone; 18,432 stereo frames queued, submitted and completed through AHI, error 0. |
| Teardown | All three runs released owned memory and restored MMU/cache state with zero register mismatches and zero cache-witness errors. |

On the same hardware, Raiden passed its former missing-EMS error and reached
the manual-word prompt. Pinball Illusions passed its former sound error and
reached the table-selection menu after a slow loading stage. Both returned
cleanly through F10. A subsequent launch using the installed MUI launcher
selected ZZDosBox-Compat and reached Raiden’s Acer intro in fullscreen, then
returned cleanly; its AHI worker completed all 574,464 queued frames. These observations do not establish full-game compatibility.

The owner confirmed hearing the brief test tones. This verifies audible output
as well as AHI completion; sustained gameplay audio quality and each game's
sound-driver compatibility remain separate checks.

Build the isolated runtime with the normal Direct build flags plus
`--pc-compat`. Run `scripts/build_dosbox_pc.py --compat --sanitize --game ems`,
then `--game sb`, `--game sb16`, `--game beep`, `--game opl` and
`--game dates` for native integration checks. SB16 exercises signed 16-bit
DMA 5 and IRQ 7; the speaker test drives PIT channel 2 and port 61 with SB
disabled. Tests assert nonzero PCM, rather than just successful process exit.
The date regression compares open-file INT 21h timestamps with FindFirst.


## 0.20.1 hardware follow-up

Physical A4000TX checks passed for SB16 signed DMA5/IRQ7 (2,048 AHI frames in the final aligned-buffer fixture),
PC speaker with SB disabled (18,432 frames), and the SBPro2 regression (4,096
frames). Every audio queue drained without AHI errors; all runs restored
MMU/cache state and released owned memory. The native tests also assert actual
nonzero PCM. Audible quality of the new SB16/speaker modes still needs listening.

The file-date regression initially caught a reset Amiga clock (January 1978).
The service now maps pre-DOS timestamps to 1 January 1980 consistently on open,
stat and directory enumeration. This does not change the host clock or files.
The corrected regression passes on hardware, and Eye of the Beholder III reaches
its animated introduction. Native testing reached its main menu.
