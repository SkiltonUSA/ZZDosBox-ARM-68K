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

Physical retest pending; native success alone does not prove target execution.

## WinBench and backup status

The supplied Win16 WinBench program opened its licence screen in the native
host build. No benchmark completed and no physical A9 WinBench score exists.
The smaller-TLB ARM build completed; physical testing is still pending.
The backup keeps the verified installed 0.10.1 executable separate from this
experiment. Historical model logs below predate the smaller-TLB change.
