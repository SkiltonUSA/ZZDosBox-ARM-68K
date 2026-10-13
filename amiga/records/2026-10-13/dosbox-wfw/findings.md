# Windows for Workgroups 3.11 installation research

Supplied eight-disk OEM media; no Microsoft binaries or disk images are included
in these records. Installation used a separate 32 MiB MS-DOS 6.22 disk, 8 MiB
of guest RAM, standard VGA, no mouse driver and no network.

## Fault and correction

Unmodified port: both CD-based and hard-disk-based setup stopped in DOSX with
GP fault EC=00A4 at 0053:2811, then EC=0014 at 0053:244B. Adding ROM protection
alone did not fix it. Implementing the advertised fast-A20 port 0x92 and
initializing the actual A20 mapping to disabled let graphical Setup complete.
This was a guest PC emulation issue; no ZZ9000 firmware change was needed.

The BIOS and VGA ROM ranges now ignore guest writes, following upstream.
The PC build also retains upstream core_full for nested page-fault handling;
DOSBOX_RunMachine unwinds a negative decoder result as upstream requires.
An original protected-mode boot fixture faults on an absent page, installs its
mapping in the handler, resumes the interrupted load and checks one fault.
The ARM instruction-model test passes, along with fast-A20 alias/readback,
ROM write protection and the existing PC regression suite.

## Native verification

ASan installation completed. A fresh boot displayed Program Manager; WINVER
reported 386 Enhanced Mode. Keyboard File/Run opened WINVER, and File/Exit
returned to DOS before stopping the emulator. Basic VGA is 640x480, 16 colours.
Startup uses WIN /N /D:FC, with networking and 32-bit disk/file access disabled.
No SMARTDRV or IFSHLP driver is required for this conservative configuration.
The supplied media identifies Windows for Workgroups 3.11; its WINVER dialog
reports the Windows 3.10 kernel version. No performance claim comes from this
native host test.

The native interactive research harness is optional, local and media-dependent;
it is not a redistributable Windows installer or part of the ARM payload.

## Related reference

[AHCIFIX](https://github.com/PluMGMK/ahcifix.386) addresses specific Intel AHCI
firmware errors in VDS buffer segment handling and EBDA storage. This port has
no AHCI device and uses BIOS disk-image I/O, so that VxD was not installed.
The project is a useful reference for tracing mode transitions and pointers,
but is not evidence for the cause of this separate standard-mode Setup fault.

## Physical validation

The initial ARM build booted MS-DOS but stopped after 3063 emulated ms with
`Out of Memory`. This was the worker allocator, not exhausted Amiga RAM.
It returned to Workbench without a Guru, with zero register/cache-witness
mismatches and owned memory released. See physical-before-small-tlb.log.

The original banked TLB allocates 65536 entries per high bank: 1.25 MiB on
32-bit ARM, exceeding the 2 MiB worker heap once multiple banks are needed.
The new layout uses 4096 entries / 16 MiB address ranges: 80 KiB per bank,
retaining all 4 GiB of linear address coverage. Guest RAM stays at 8 MiB and
the Exec-owned allocation stays at 32 MiB. The protected-mode fixture also
maps and reads 0x80000000, 0x81000000 and 0xff000000 across a TLB flush.

The smaller-TLB ARM model regression suite passed, including the high-bank
paging fixture. The revised build then reached Windows Program Manager on
the physical A4000TX and completed WinBench 3.11 Graphics WinMark.

## Physical WinBench result

- Graphics WinMark: **1,923,752 pixels/sec**, as reported by WinBench.
- Settings: 8 MiB guest RAM, 3000 cycles/ms, standard VGA 640x480/16 colours,
  Workbench window 640x480, ARM FPS overlay enabled; networking and PCM off.
- Launch-command to result observation: **at most 267.25 seconds**. This includes
  input/UI/screenshot delays, rather than precise test-only instrumentation.
- WinBench labels the CPU **37 MHz 80386**. This estimate and the pixels/sec
  score use the emulated PC clock; neither establishes physical 386/37 speed.
- Its built-in Compaq 486/33m comparison uses QVision at 800x600/256 colours,
  a different graphics configuration. It is not an equivalent hardware test.

The result screenshot and individual subtest values are preserved alongside
this report. The earlier native licence-screen test did not produce a score;
this completed run used the real ZZ9000 A9.

WinBench and Help closed normally. Program Manager's Exit Windows confirmation
returned to the DOS prompt. F10 then closed the emulator. The log reports
SHELL PASS, BIOS HDD close=ok, all file handles closed, zero register mismatches
and cache-witness errors, and LAUNCHER exit=0 with owned memory released.
No Guru or reboot was observed.

The complete interactive session (boot, application loading, test and manual
interaction) recorded 12,946 refreshes, 213,047 emulated ms and 805,852 wall ms.
Those totals are not the benchmark-only duration or a game FPS measurement.
A future CPU-equivalence comparison needs benchmark-only wall-clock timing;
it must not scale this mixed-session average into an asserted PC clock speed.

## Installed build and recovery

The alternate Windows runtime and executable Workbench launch script remain
in their separate Windows311 drawer. The canonical tested 0.10.1 DOS runtime
is unchanged. The prior Windows runtime and pre-WinBench image are retained.
WinBench resides inside the private guest disk at C:\WINBENCH\WINBENCH.EXE.
Windows is keyboard-only: guest mouse, PCM audio and networking remain absent.
The Amiga was left on Workbench with no Core1 owner after verification.

Microsoft software, WinBench binaries and guest disk images are local supplied
media and are not included in source backups or releases.
