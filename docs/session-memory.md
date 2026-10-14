# ZZDosBox development handoff

## Current Direct runtime — 14 October 2026

The measured PIC VFP runtime is installed and passed its installed-path
EGA/PIT check. The previous Direct runtime is retained for rollback. The
MUI launcher, OpenGL and Windows executables are unchanged by this update.
[Report, measurements and reproduction](performance-pic-vfp.md).
The sections below are earlier development history.


## ZZDosBox software scanline experiment — 13 October 2026

Separate `RAM:ZZScanBench` implements a Classic software mask in the final ARM
copy. F9 cycles Off/Full/Skip. It is independent of XANI's Workbench FPGA
controls; no firmware change. Fullscreen 640x480 only; window/overlay output
is unmasked. Normal runtime and launcher settings remain unchanged.

Matched Doom Demo3 at 10000 cycles/ms, 8 MiB guest, no audio/overlay:
normal **7.3281 FPS**, black rows rewritten **7.4409**, black rows preserved
**7.5970**. Single runs: preliminary +3.67% vs normal, +2.10% vs identical mask
rewritten. Only final-copy work is reduced, not x86/VGA rendering. Exactly
three buffers initialized; 3,121,920 black rows skipped. All runs cleaned up.

Separate physical EGA and interactive Full -> Skip -> Off/F10 checks passed.
Screenshots show 240 black odd rows in both masked modes, restored when Off;
owner was not watching. Not a tear-free scanout claim. Native sanitizers and
ARM-model pixel/ownership/transition/cancellation tests passed. Core1 idle,
Workbench restored, global FPGA scanlines Off. Candidate is RAM-only; local
build `.context/amiga/dosbox-dynrec-4m-neon-pipe-scan/ZZDosBox-Dynrec`.
Evidence and exact hashes: `amiga/records/2026-10-13/dosbox-scanlines/`.

## ZZDosBox NEON / 4 MiB / overlap installed — 13 October 2026

Separate candidate `RAM:ZZTuneNeon` (lost on reboot), local
`.context/amiga/dosbox-dynrec-4m-neon-pipe/ZZDosBox-Dynrec`, SHA256
`55085f15f23bc3fc077498e2b2cd9bf3770558a9706147ece163240ac73dcb7d`.
Now installed as `SD032G:Dev/ZZDosBox/PC/ZZDosBox`, verified MD5
`62a29c0ece11553121368b219c286741`. Initial dynrec preserved as `ZZDosBox-1M`
(MD5 `8020c504f3f3d621939d1ceffed0818f`); `ZZDosBox-Interpreter` also retained.
Launcher loads the new runtime on its next launch. Launcher.cfg MD5 before
and after installation: `297dddafcdd1fa80cf87d4aeaca3361d`. No profiles or
separate Windows executable changed.

Doom Demo3, matched 10000-cycle settings: 6.94 initial / 6.96 overlap /
6.97 4 MiB + overlap / **7.29 NEON + 4 MiB + overlap** real FPS. About 5%
above initial dynrec. All complete with clean NEON/MMU/cache/memory teardown.
Total allocation still 32 MiB, guest RAM 8 MiB, private heap 8 MiB. Build and
model commands, exact hashes and physical evidence: today's `dosbox-tuning/`.

Interactive test at 3000 cycles/ms reached window mode but injected reverse
switch/F10 did not take effect. Bridge briefly timed out; reconnect succeeded.
Owner confirmed Workbench responsive. CLI `Break 10 C` cancelled our test,
with context/register/witness restoration clean and owned memory released.
No reboot. Second manual-key test passed: owner confirmed both Alt+Return
switches and F10; `physical-interactive2.log` records both switches, checked
restoration and memory release. Core1 owner absent, launcher on Workbench.
Installed after that test using checked staging/rename with rollback. Nothing
launched after installation. See `dosbox-tuning/installation.json`. Injected
input failure's cause remains unconfirmed; physical keys did not reproduce it.


## Dynrec connected to the normal ZZDosBox launcher — 13 October 2026

At the owner's request, installed the exact physically tested dynrec executable
as `SD032G:Dev/ZZDosBox/PC/ZZDosBox`. The MUI launcher already invokes that
sibling path for program, terminal and boot launches; no UI rebuild needed.
MD5 verified `8020c504f3f3d621939d1ceffed0818f` (SHA256 and build identity below).
Preserved the previous runtime as `PC/ZZDosBox-Interpreter`, MD5 verified
`a0472312aa609271755c08f16702d42f`. Launcher.cfg checksum remained
`6e698ae2f79aaabed42b713e9416db24`; profiles/settings were not changed.

A user-owned fullscreen DOS session was active during installation. It was
left running, without injected input or a second Core1 launch. It still uses
the already-loaded interpreter: exit and launch again for dynrec. Empire combat
and Windows compatibility remain pending; the Doom result is the verified
speed measurement. Installation used a checked staging file and rename with
rollback on failure. See `dosbox-dynrec/launcher-install.log` in today's records.
The owner then requested closing the active game. Sent F10 down/up; confirmed
the Core1 owner disappeared and the MUI launcher returned on Workbench. The
next launch will load dynrec. No replacement game was launched.
The separate Windows311/ZZDosBox-WFW executable remains its previous build.
To revert the normal launcher, close its DOS session, preserve the dynrec file,
and restore `ZZDosBox-Interpreter` to the sibling `ZZDosBox` executable path.


## ZZDosBox ARMv7 dynamic recompiler verified — 13 October 2026

Built a separate prototype with `scripts/build_dosbox_dos_arm.py --pc
--pc-dynrec --pc-doom-clock` (explicit Podman connection as in the record).
Artifact: `.context/amiga/dosbox-dynrec/ZZDosBox-Dynrec`, 1,071,752 bytes,
SHA256 `e3d0718d9bb41e9b0ee51053c32e9a8b9c4ac7717625f86099325590d827f0aa`.
Temporary hardware path: `RAM:ZZJit1013/ZZDosBox-Dynrec` (lost on reboot).
Canonical installed player/launcher remain the interpreter; not promoted or
published. The temporary HTTP transfer server has been stopped.

Uses pinned libretro DOSBox ARMv7 dynrec, with a 1 MiB executable code cache
and 4 MiB heap inside the existing 32 MiB owned allocation. Local A9 I/D cache
publication replaces Linux syscalls; no firmware/Core0/shared-L2 changes.
Physical generated-code rewrite witness, self-modifying DOS code and fullscreen
EGA/PIT passed. Linked-model CPU/files/MZ, paging, cancellation, BIOS/HDD,
mouse and relocation checks passed. Normal interpreter sanitizer check passed.

Full Doom 1.9 shareware Demo3 at the previous 8 MiB / VGA / 10000 cycles /
fresh defaults / fullscreen / no-overlay settings completed all 2134 gametics:
**307.311 seconds / about 6.94 real FPS**, versus 689.143 seconds / 3.10 FPS
for the interpreter. **2.24× throughput**. One physical run per build, sampled
with the read-only observer; approximately one-frame endpoint uncertainty.
Guest-clock 11.67 FPS is not the hardware score. No input/screenshots during
the measured run. This does not establish general Intel CPU equivalence.

JIT witness 1, 12,365 commits, 24,818,803 dispatches; SHELL PASS / guest exit 0,
13,012 flips, checked register/cache restoration clean, memory released.
Workbench, bridge and Roadie confirmed available; no Core1 owner remains.
Evidence and repeatable build/model commands:
`amiga/records/2026-10-13/dosbox-dynrec/`. Broader games and Windows 3.11 remain
pending on dynrec. No game media or guest RAM dumps included in records.


## ZZDosBox overall optimisation candidate — 13 October 2026

User requested overall optimisation first, with Empire combat retesting later.
Two local source changes are under verification: exact incremental generic
scaling and 4 KiB read-ahead for exclusive BIOS HDD images (immediate writes).
Installed runtime and the owner's active session are untouched. Read-ahead does
not cover Empire's mounted directory. Local disk/pixel differential tests, the ARM scaler model, ARM/68k build and
full linked ARM model regressions pass. Candidate (not deployed): 978232 bytes,
SHA256 64598728b83dbcb8237b656f252923d95a274005a93c65fb04992ff50805e41b,
retained at `.context/amiga/dosbox-optimisation/ZZDosBox`. Physical speed
comparison remains pending. See
`amiga/records/2026-10-13/dosbox-optimisation/findings.md`.

The `hard drive data rate limit=0` remark was explicitly clarified as a forum
report, not an observed Amiga improvement. That DOSBox-X option is absent
from this port. Reviewed dosbox-rewrite at 659519f0bda38b0ec454c40ccd1371144077d8e8
as a design reference; no emulator-base switch or code import was made.

## ZZDosBox mouse and three-game installation — 13 October 2026

Default `SD032G:Dev/ZZDosBox/PC/ZZDosBox` and
`PC/Windows311/ZZDosBox-WFW` now use the mouse build (977864 bytes), SHA256
`604a2b276d8896139927445391d9a6ff92687d2b96416fd173727d25745b65b0`,
MD5 `a0472312aa609271755c08f16702d42f`. Both promoted copies were independently
checksummed. Recovery: `PC/ZZDosBox-PreMouse` and
`PC/Windows311/ZZDosBox-NoMouse`. Launcher remains 0.10.1-dev; no firmware change.
Mouse input is automatic, no extra Amiga library. DOS INT33 and BIOS PS/2;
three buttons, window/fullscreen mapping, focus/display-transition releases.
Host pointer hidden over the guest image. No continuous relative pointer capture.

Windows now uses its supplied Microsoft/IBM PS/2 `MOUSE.DRV`. Previous
`WINDOWS/SYSTEM.INI` is backed up as `SYSTEM.PRE` inside the private disk.
The temporary WFWHD diskimage unit 6 was ejected before boot. Never mount it
concurrently with the emulator. No Microsoft driver/image goes to GitHub.

Physical tests: Oregon Options/quit, Windows movement/double-click Write,
Windows exit to DOS, final build window/fullscreen switches and input. Native
ASan and full linked ARM model suite pass. All final hardware teardowns restored
register/cache witnesses and released owned memory. One injected F10 during
a view transition was missed; repeat after the player has focus. The final
test session closed cleanly. At 15:30 local, a new canonical `PC/ZZDosBox`
session was observed in fullscreen at 6000 cycles/ms (process 2, Core1 owned),
started outside this agent’s test script. It was left running. Recheck ownership
before further deployment or tests.

Added 242 checksum-verified game files in Shell/Games/EMPIRE, OREGON and CIV;
appended three profiles while preserving the five existing entries. Oregon
uses `OREGON.EXE -16`. Civilization's English hard-disk files were extracted
from the supplied data-CD image; no CD mount is needed for that profile.
Empire/Civilization are installed, not full compatibility results. Launcher
had unsaved File startup selections with empty paths; restored Default for
AUTOEXEC/CONFIG so it could save/close normally. Private game/media archives
remain local. Evidence: `amiga/records/2026-10-13/dosbox-mouse/`.

## ZZDosBox physical Windows / WinBench pass — 13 October 2026

The smaller-TLB build is installed as
`SD032G:Dev/ZZDosBox/PC/Windows311/ZZDosBox-WFW` (960580 bytes), SHA256
`e7a17153553633ca535743c8f52051b2c3d9195362be29f356d4ba2e660d6f27`,
MD5 `4566100283e38bbdfa92bbe6fc802a89`. Executable Workbench launch script:
`Start-Windows311` in that drawer. Canonical 0.10.1 runtime is unchanged.
Prior alternate runtime: `ZZDosBox-LargeTLB`; prior disk: `WFW311-base.IMG`.
Current WFW311.IMG contains licensed user-supplied Windows, DOS and WinBench;
it is private and has changed since its pre-run MD5 after guest writes.

The full ARM model suite passed. On hardware, Windows reached Program Manager
and WinBench 3.11 completed Graphics WinMark: **1,923,752 pixels/sec** at 8 MiB,
3000 cycles/ms, standard VGA 640x480/16 colours, window + FPS overlay enabled.
WinBench's **37 MHz 80386** label uses the emulated clock; it is not a verified
physical PC speed equivalent. Result observation was within 267.25 real seconds
of the start command, including UI/screenshot delays.

Launch WinBench via Program Manager File / Run: `C:\WINBENCH\WINBENCH.EXE`.
Local first-run licence details were entered; no network registration occurred.
Windows mouse, PCM sound and networking remain unimplemented. Close apps, then
File / Exit Windows, confirm, and use F10 at the DOS prompt to close ZZDosBox.
This full lifecycle passed: SHELL PASS, disk/handles closed, no register/cache
witness mismatch, launcher exit 0, owned memory released. No Guru/reboot.
Left on Workbench, launcher still open, no Core1 owner. Evidence and screenshots:
`amiga/records/2026-10-13/dosbox-wfw/`. Physical findings and owner-provided DOS/Windows screenshots are now backed up
on private GitHub main `20dc70cc47be9fbae8a5b5b2d252b61b6e615313`; remote hash and 821-file manifest verified.

## ZZDosBox Windows research and GitHub backup — 13 October 2026

Private GitHub backup updated to `33420b220ab1b6e82a67e4c9d5b7b206cb2eb34a` on `main`. Remote hash verified;
805-file checksum verification and host tests passed. Receipt:
`.context/zzdosbox-github-backup-receipt.json`.

Windows for Workgroups 3.11 installed and reached Program Manager in native
host ASan testing after the fast-A20 fix. WinBench opened its licence screen;
no benchmark score exists. The physical Windows experiment stopped with worker
heap exhaustion, then returned cleanly. Smaller sparse TLB banks have been
built but have not yet been retested physically. 8 MiB guest RAM and 32 MiB
owned allocation are unchanged. See `amiga/records/2026-10-13/dosbox-wfw/`.

The canonical installed 0.10.1 runtime was retrieved read-only and its SHA-256
matched `b92eff5ffb7b36bce27b6e4bcb24acc78b98a2ff31e6338728ef9a1d7f1693c6`.
Backup artifacts retain this tested runtime separately from the Windows
experiment. Microsoft media, WinBench, games and disk images stay local.

## ZZDosBox 0.10.1 startup, CD, MIDI and welcome installed — 13 October 2026

Canonical drawer: `SD032G:Dev/ZZDosBox/PC/`. Runtime `ZZDosBox`: 918612 bytes,
CRC32 `A7118584`, SHA256
`b92eff5ffb7b36bce27b6e4bcb24acc78b98a2ff31e6338728ef9a1d7f1693c6`.
MUI `ZZDosBox-Launcher`: 160104 bytes, CRC32 `625D32CF`, SHA256
`f43418fa2e246c0de4321b1af9416d4940c7ec943bd1d1a94bb92492593ff403`.
Both were promoted and independently checksummed. Launch with Stack 65536.

DOS terminal now contains Terminal, AUTOEXEC.BAT, CONFIG.SYS and CD-ROM tabs.
Startup files select Default / Edited / File. For DOS 6.22 the launcher mounts
DOSHD, recovers any startup transaction, installs temporary root startup files,
ejects before boot, then restores originals and ejects on return. Preserve
`ZZDSTART.TXN`, `ZZDAUTO.BAK` and `ZZDCONF.BAK` if interrupted; next preparation
recovers them. Native fault tests and a physical File + ISO launch passed;
both original startup files were compared byte-for-byte after return.

CD is read-only D: from ISO9660 or an existing Amiga CD mount. Physical UMSD:
uses `usbscsi.device` unit 0, AMINET13, 325420 sectors. Both its directory and
an original test ISO were read by embedded DOS. Real DOS 6.22 used the new
87-byte `ZZDOSCD.SYS` and existing Microsoft MSCDEX 2.23 to read the ISO.
Driver remains in the app drawer and DOS disk root. No USB/CD configuration
or firmware changes. No audio-CD support.

The original 654-byte `ZZHELLO.COM` (CRC32 `0E8A118A`) draws the blue CP437
welcome panel, ASCII logo, project/version, detected DOS version and shortcuts.
It is installed in the app drawer, DOS disk root and embedded C: drawer;
launcher startup calls it without changing permanent AUTOEXEC/CONFIG files.
Physical MUI launches passed in DOS 5 fullscreen and DOS 6.22 window. It does
not depend on ANSI.SYS or the non-DOS-6 COLOR command. Existing helper files
must match the shipped bytes; conflicting files are preserved and reported.

Alt+Return switches window/fullscreen; Ctrl+minus/plus changes CPU cycles in
500 steps (500–10000); F10 closes. Physical tests passed four switches per
session, with clean exits from both display modes and speed changes 3000 ->
3500 -> 3000. Retain both bitmap allocations until synchronous Core1 reset.
Intuition focus changes asynchronously; final code delays activation outside
bitmap locks. Tests wait for actual active window/screen before the next key.
Early tests needed a click to refocus before clean exit; no reboot was needed.
Do not synthesize Workbench drag/resize gestures (earlier Intuition stall).

Settings supports external MT-32/intelligent MPU-401 or General MIDI/UART
through CAMD, off by default. Internal CAMD capture physically verified 5015
bytes / 5005 messages, three notes, one SysEx, errors zero. No external notes
were sent and audible Roland playback is untested. New Check audio drivers
found CAMD 37.14, out.0, AHI 4.180 and two ZZ9000AX modes. This only establishes
installed drivers/routing; PCM Sound Blaster/AdLib/PC speaker audio is pending.

Previous binaries/config: `ZZDosBox-PreStart`, `Launcher-PreStart`,
`Launcher.cfg-PreStart`; full DOS disk backup `Disks/DOS622-PreStart.IMG`.
Original profiles restored after tests (format 5); selected MS-DOS 6.22,
window 640x480, FPS on, MIDI/CD off. Launcher left on DOS terminal, no Core1
owner, DOSHD ejected. No GitHub push; private GitHub backup remains 0.8.

Native ASan EGA/PIT/CD/MIDI/welcome, startup/profile units, full ARM model
(including CD/MIDI/speed) and shared ARM/MCP checks pass. All recorded target
runs finished with register/cache witnesses restored and owned memory released.
Evidence: `amiga/records/2026-10-13/dosbox-startup-cd/`; scratch automation and
screenshots: `.context/amiga/dosbox-startup-cd/`.

Owner also highlighted Staging's HLT-based `cpu_idle`. Reviewed and documented
as follow-up; **not implemented** and no ZZ9000 idle-power saving measured.
Current core emulates HLT but prompt callbacks still run NOP idle loops.
An emulated HLT does not sleep the physical ARM, whose XACP handshakes still
poll. Do not introduce ARM WFI/WFE without a verified wake-up contract.
Cortex-A9 and this port are 32-bit; no 64-bit x86 guest support.

## ZZDosBox 0.9.1 resizable window installed — 13 October 2026

Supersedes the 0.9 installation below. Canonical location remains
`SD032G:Dev/ZZDosBox/PC/`. Runtime `ZZDosBox`: 856340 bytes, CRC32
`38EF3834`, SHA256 `8d337250df045eba3e143ac8553381ee9bda1fb1178288be83b3d281977b0f97`.
Launcher: 109744 bytes, CRC32 `10DCA4AE`, SHA256
`270d4b8517b0bac6644690b6bb5950cf7105deea46f18a22a4f1d31b621772df`.
Both installed and independently checksummed. Previous binaries/config are
`ZZDosBox-PreSizes`, `Launcher-PreSizes`, `Launcher.cfg-PreSizes`; older
PreWindow backups remain. Launch UI with Stack 65536 as before.

DOS terminal now has Display (Workbench window / Fullscreen) and Window size
(320×240 / 640×480 / 960×720). **Show FPS is in Settings**, per the owner's
latest request. Format 3 saves the preset per profile; formats 1/2 import at
640×480. Disk boot also consumes the preset. Fullscreen disables the size
control and stays 640×480. Native Workbench size gadget permits client sizes
between 320×240 and 960×720, with a centred 4:3 picture and black margins.
The P96 bitmap remains allocated at maximum capacity throughout the session;
resize only changes the next validated ARM grant dimensions.

Verified physically: all three terminal sizes, keyboard VER, ARM FPS and F10;
MS-DOS 6.22 fullscreen; MUI preset cycling/save/readback, fullscreen disabling
size; MUI Open terminal boots 6.22 at 640×480, native close gadget returns.
All runs restored register/cache witnesses and released owned memory. Final
FPS relocation is layout-only, rebuilt and installed after these runs.
Native ASan, configuration/scaling/geometry/mapping/FPS tests, full ARM PC
model (including changing geometry in both formats), and shared ARM/MCP
checks pass. **Physical drag-resizing remains untested.** Do not synthesize
window dragging/sizing through the bridge; see the earlier Intuition stall.

Final profile: MS-DOS 6.22, window, 640×480, FPS enabled. Other profiles kept
their existing settings. Launcher is on DOS terminal, no Core1 owner, disk
image ejected from Workbench. No firmware/startup/disk-image changes or GitHub
push. Evidence: `amiga/records/2026-10-13/dosbox-window-sizes/`;
local automation/screenshots: `.context/amiga/dosbox-window-sizes/`.


## ZZDosBox 0.9 window / DOS choice / ARM FPS installed — 13 October 2026

Canonical `SD032G:Dev/ZZDosBox/PC/ZZDosBox` is 0.9.0-dev, 850584 bytes,
CRC32 `6F849EC3`, SHA256
`4634499134230d9f9eec534b130c0e1c9c55c68132baaf8a65e8eaf9c285d6a8`.
Launcher is 108640 bytes, CRC32 `58875C97`, SHA256
`4276774c19b07fc2573a1b8629a80d23d4fb91df3ba4108a37acf6894f588de2`.
Both were transferred and independently checksummed. Backups in the same
folder: `ZZDosBox-PreWindow`, `Launcher-PreWindow`, `Launcher.cfg-PreWindow`.
The DOSBox icon remains installed. Launch the UI with Stack 65536 (its icon
also retains 65536); do not inherit a small Shell stack.

DOS terminal chooses DOSBox DOS 5 or installed MS-DOS 6.22. Settings chooses
Workbench window/fullscreen and Show FPS. A new MS-DOS 6.22 profile boots
existing `Disks/DOS622.IMG`, with 8 MB guest RAM; no disk contents were replaced.
MS-DOS uses its own disk startup files. Host-drawer, colour and edited-startup
controls are disabled there. Commands for booted DOS refer to its disk paths.
Session/profile format 2 saves the new options; old profiles import as
fullscreen, FPS off, embedded DOS. New profiles default to window mode.

ARM renders into a locked, nonce-verified offscreen P96 bitmap, then the host
blits completed frames into a clipped Workbench window. Current Workbench
32-bit format 9 is physically verified. Fullscreen retains deferred XACP PAN.
The embedded prompt now renders through VGA and uses BIOS keyboard/idle IRQ
handling. Optional Core1 overlay measures completed video refreshes against
XX19c's nominal 333.333 MHz global timer; it is not unique game frames.

Verified physically: MS-DOS 6.22 window + FPS + VER (startup and raw keyboard),
embedded DOS 5.00 window with FPS off + VER, Keen window graphics + FPS,
MS-DOS fullscreen + FPS + PAN ACKs. All returned on F10 with register/cache
witnesses restored and owned memory released. MUI saved options survived a
restart; its Open terminal button booted 6.22 with FPS, and the window close
gadget returned cleanly. No crash data recorded. Native ASan fixture, config
migration/FPS unit tests, full ARM PC model including VGA-shell raw-key VER/EXIT,
and `make test-amiga-arm test-amiga` passed.

Final state: launcher on DOS terminal tab, MS-DOS 6.22 selected, window + FPS
saved, no Core1 owner. Image remains ejected from Workbench. No firmware or
startup-service changes. Evidence: `amiga/records/2026-10-13/dosbox-window/`.
Local automation/screenshots: `.context/amiga/dosbox-window/`. No GitHub push;
the previous private backup remains 0.8.0-dev.

## ZZDosBox GitHub backup — 13 October 2026

Created private repository https://github.com/SkiltonUSA/ZZDosBox-ARM-68K,
main commit `1303167e68a41965593895bfdae57d27acf24906`. Remote branch and
artifact Git blob hashes verified. Local export: `.context/ZZDosBox-ARM-68K`;
receipt: `.context/zzdosbox-github-backup-receipt.json`. Contains 0.8.0-dev
runtime/launcher/icon, editable source and shared dependencies, pinned upstream
Git bundle, picolibc sources/build support, DOSBox-only handoff and hardware
records. No games or Microsoft DOS media. This is not an Amiga disk backup.
Independent export passed 639-file checksum manifest, host tests, native ASan
fixture and ARM instruction model. Original runtime and hardware untouched.
Repository remains private; no GitHub Release was created.

## ZZDosBox scheduler experiments / icon update — 13 October 2026

**Recovered:** bridge temporarily stopped answering after an injected Workbench
window-sizing gesture while capturing the new DOSBox icon. No Core1 application
was running. After requesting a physical mouse-button release, the bridge
responded again and the owner confirmed recovery; a fresh injected button-up
cleared any remaining qualifier. No reboot was observed and the bridge reports
no crash data. The bridge source `input_inject.c` warns that injected window
drag/size gadgets can put Intuition into a modal loop requiring a physical
mouse-button release. Avoid **all synthetic Workbench icon clicks and window
drag/size gestures**. The earlier `amiga_window_resize` also timed out; do not
follow it with injected dragging. This was a UI capture failure, not an
emulator crash observation.

Installed runtime remains the original 0.8.0-dev (CRC32 1FCAF3E4), and the
launcher/config are unchanged. User's supplied DOSBox ICO is now installed as
a classic 64x64 Workbench launcher icon; readback and metadata passed (WBTOOL,
stack 65536). Old icon: `SD032G:Dev/ZZDosBox/PC/LauncherIcon-Original`.
Master: `amiga/dosbox/assets/dosbox.ico`; conversion/build helper:
`scripts/prepare_dosbox_icon.py`, `amiga/dosbox/launcher/icon.c`.

Two scheduler experiments passed native differential PIC traces, exact game
output/last-image checks, boot tests, ARM model tests and physical EGA/PIT/
timed-game teardown. Neither is selected. Product-only cache: Keen mean wall
15030.5 -> 15118.5 ms; Vette 20062.5 -> 19818 ms (two trials each).
Integer-deadline cache: Keen median 15032 -> 15324 ms; Vette 20047 -> 19913 ms
(three trials each). Original scheduler restored in production source; full
experimental source and evidence archived under
`amiga/records/2026-10-13/dosbox-scheduler/`. Inactive Amiga candidates
`ZZDosBox-PIC` and `ZZDosBox-Cycles` remain separate from the normal executable.

CPU-only `--pc-cpu-opt O3` added as an opt-in build experiment; default stays
O2. O3 build/model and physical EGA/PIT lifecycle passed. Its staged executable
`ZZDosBox-O3` was independently checked after recovery: 838324 bytes, CRC32
3B1273CE. The installed baseline was also rechecked: CRC32 1FCAF3E4.
Local binary/ELF/build record: `.context/amiga/dosbox-cpu-o3/`.
Two matched trials per game/build also rejected O3: Keen mean wall time
15034.5 -> 15100 ms (+0.44%); Vette 20075 -> 20156 ms (+0.40%). Each run
completed 558 frames/8000 simulated ms and passed register/cache restoration
and owned-memory release. These are startup refresh measurements, not steady
play or unique frames. Final bridge ping/launcher window passed, no Core1 owner,
no crash data; canonical executable remains CRC32 1FCAF3E4. Local default O2
rebuild matches the installed baseline SHA256 byte-for-byte. No GitHub push,
firmware/startup change or baseline replacement.

## ZZDosBox 0.8 game/profile validation — 13 October 2026

No executable changes. Tested the installed canonical 0.8 via its saved MUI
profiles: Keen menus/new game/world-map movement and scrolling; Vette with
LOADFIX through car/difficulty/opponent/course selection to its original
manual question (driving unverified); Elite with LOADFIX through Jameson's
status and launch/front-view screen (no prolonged flight test). All three
F10 exits passed register/cache restoration and owned-memory release.
Screenshots interrupt execution, so interactive-run FPS is not a benchmark.

A temporary fifth profile (8 MiB, EGA, 1000 cycles, green/black, edited
AUTOEXEC) survived MUI Save and a new launcher process byte-for-byte. Run
AUTOEXEC printed ZZD08-AUTOEXEC-PASS and exited normally; runtime settings
matched the profile. This shell-only test does not prove all RAM or EGA
drawing. Original four profiles were restored byte-for-byte, then Keen was
selected/saved for the final launcher. No media or game binaries changed.

Controlled, screenshot-free 2 MiB/VGA/3000-cycle startup runs with scripted
Enter: Keen 558 frames/8000 simulated ms, 15000/14989/15050 wall ms,
mean 37.168 refresh FPS; Vette 558/8000/20038, 27.847 FPS; Elite
559/8000/14114, 39.606 FPS. These are video refresh rates across startup,
not unique game images or steady play; emulation is below real time.
Separate instrumented runs show CPU interpreter plus PIC/VGA events taking
56.8% of ARM time in Keen and 59.7% in Vette. Next optimisation should
measure interpreter/guest-memory and event-dispatch costs; no speedup claimed
from these tests. Audio remains unimplemented.

Evidence: `amiga/records/2026-10-13/dosbox-08-games/`; local controls/captures
in `.context/amiga/dosbox-08-games/` and `.context/dosbox-08-{games,bench}.py`.
No new Guru observed; bridge reports no crash record. Final state: launcher
Programs/Keen selected, no Core1 owner, disk images ejected. No GitHub push.

## ZZDosBox 0.8.0-dev installed — 13 October 2026

Canonical runtime/UI in `SD032G:Dev/ZZDosBox/PC/` are now **0.8.0-dev**.
Runtime 838,356 bytes, SHA256
`65b433b4902c7fe495bdf562129cff912fc7a5eecf4917b7e3837edfb1c561f5`,
CRC32 `1FCAF3E4`. UI 100,420 bytes, SHA256
`58eb1d0e362c85c0b8c6f252cbf9d6aff7d76ecd50cc025a8d290f2d81ea61cc`,
CRC32 `DD8BE52F`. `ZZDosBox-HDD` matches the canonical runtime.
Previous 0.7 runtime/UI/config are `ZZDosBox-PreHDD`, `Launcher-PreHDD`,
`Launcher.cfg-PreHDD`; earlier PreOvernight copies remain too.

`Disks/DOS622.IMG` is a 32 MiB BIOS hard disk, CHS 128/16/32, FAT16 partition.
Prepared MS-DOS 6.22 and DOS utilities come from the owner's supplied disks;
Keen/Vette/Elite are in C:\GAMES. HIMEM works with 8 MiB guest RAM (7,104 KiB
free XMS); DOS=HIGH stalled in native testing and is omitted. EMM386 is present
as a DOS file but not enabled/validated. No full interactive SETUP run claimed.
User media is local only. No new FPS benchmark.

The first mount helper stalled before an ARM job started. Owner then reported
**Guru during the restart**, not during initial mounting. Bridge retained no
crash data after reboot, so exact Guru cause is unknown. The likely mount
deadlock was using `DiskImageCtrl DRIVE DOSHD:` while FAT95 was inhibited.
Corrected helper uses raw UNIT 5, non-resolving device-list lookup, static path
buffers (main frame 76 bytes), minimum stack guard, 32 KiB child stacks and
progress logging. Old helper is retained disabled as `ZZDosDisk-PreGuru`.
Do not restore it. Corrected `ZZDosDisk` is installed and physically tested.

Retests passed: readonly mount/browse/eject, DOS refused while Workbench held
image, raw diskimage.device second open refused while BIOS held image, real A9
DOS write/flush/F10 return, fresh-process readback, Workbench readback. Register
and cache-witness restoration and owned-memory release passed. MUI mount/eject,
mounted-boot refusal, HDD boot/VER/F10 return also passed. Test BAT/text files
were removed. Final state: Workbench, launcher Programs tab, no Core1 owner;
unit 5/DOSHD exists with image ejected. All floppy image units are empty after
reboot; use Mount floppies if wanted. No startup/firmware changes.

The Disks tab selects MS-DOS hard disk by default. Use Mount hard disk for file
transfers and Eject hard disk before Boot. F10 returns to the launcher. The
AUTOEXEC/CONFIG inside the image control booted DOS; launcher profile editors
still apply/reference embedded DOS. Native exclusive-lock/bounds/persistence,
ARM BIOS HDD fixture, existing PC model tests and make test-amiga-arm/test-amiga
passed. Native SYS transferred boot files; supplied EXPAND ran under Mac
DOSBox-X to unpack utilities. Gzip transfer CRC/extraction passed; whole-image
bridge CHECKSUM timed out and was not counted as successful verification.

Evidence: `amiga/records/2026-10-13/dosbox-08/`; detailed local events/screenshots
under `.context/amiga/dosbox-hdd/`. Scripts `.context/dosbox-hdd-test.py`,
`.context/dosbox-hdd-ui-test.py`, `.context/dosbox-hdd-promote.py`. Stop on
STILL RUNNING before doing another dependent operation. No GitHub publication.

## ZZDosBox 0.7.0-dev installed — 12 October 2026

Completed the next development build in `SD032G:Dev/ZZDosBox/PC/`. Launch
`ZZDosBox-Launcher`; the normal `ZZDosBox` is the final banked-TLB build with
the rare high-address path separated. Runtime: 832,744 bytes, SHA256
`23914a18812cdb39cbc114153fd3cfb9ad4b29d7968d7269e0f401458de723ae`. ARM ELF SHA256
`93c824f6aa5307e6045b012d69d28ab311f798b234d3c28b66f61a5e284d30fd`. Launcher: 97,604 bytes, SHA256
`cc0a14b2eb3a460aca4617cad1418bd10913082670f28d0c9e0f9d0964c578b7`. Installed files were checked against local CRC32.

New: nested DOS directories/files on the shared C: drawer; 1/2/4/8 MiB guest
RAM; immutable 1.44 MiB BIOS floppy boot; Disks tab selector/Boot disk button.
Real A9 DOS boot accepted VER/MEM, identified MS-DOS 6.22 and reported 8,192 KiB
total / 7,168 KiB extended memory via the disk's HIMEM driver. F10 restored
Workbench; the MUI button path was also exercised and returned to the launcher.
Disk CD-ROM driver BANANA fails because there is no emulated IDE/CD-ROM. This
is expected, not proof of a failed DOS boot. Booted DOS uses the image's own
AUTOEXEC/CONFIG files; launcher editors apply/reference embedded DOS only.

Physical C: directory/write/read/remove, boot/input/exit and repeat Keen runs
passed restoration and release checks. Linked ARM tests cover boot sectors,
write protection, bounds, relocation, VGA formats, cancellation and high-address
failures; native ASan tests cover DOS/file fixtures, boot and three game starts.
The software TLB shrinks from 21,103,704 to 1,442,964 bytes, keeping the original
32 MiB Exec allocation. High-bank allocation can fail cleanly within the
2 MiB heap; no general protected-mode support is claimed.

Three matched timed Keen startup runs per version: old full-TLB build averages
38.91 FPS; final 0.7.0-dev averages 36.63 FPS (6.16% longer median wall time).
This is a compatibility/memory milestone, not a net FPS gain. Prior working
binary/config/UI remain as `ZZDosBox-PreOvernight`, `Launcher.cfg-PreOvernight`,
`Launcher-PreOvernight`. `ZZDosBox-Opt` matches the final installed runtime.

DOSBox-X 2026.10.01 at `378e6f8a25768594df87c9f5c99b5df9b3f6a8b0` was built
on the Mac and passed the same original DOS file/MZ fixtures with normal and
dynamic_rec settings. It was NOT run on the A9 or merged into our core. See
`docs/amiga-dosbox-feasibility.md` for comparison and next steps. Writable BIOS
HDD / MS-DOS hard-disk installation, embedded XMS/EMS, audio, mouse/joystick and
ARM dynrec remain pending. The shared writable C: drawer is still available.

Records: `amiga/records/2026-10-12/dosbox-07/`; scratch/tests/screenshots and
source backups: `.context/amiga/dosbox-overnight/`. No media, firmware, startup
or unrelated emulator changes; no GitHub publication this turn. Final UI stays
on Programs; the bridge/Roadie remain available and no Core1 owner is active.

## 2026-10-12 — DOSBox interpreter port: first physical execution

User requested porting libretro/dosbox-libretro. Separate prototype sources:
`amiga/dosbox/`, scripts `build_dosbox.py` and `build_dosbox_arm.py`; upstream
pinned at a563bb5e9753f207d74e9d8c417d793b784fa4be. Read
`docs/amiga-dosbox-feasibility.md` before continuing. Shared emulator/launcher
sources were reused without editing the NES/SNES installations.

Installed diagnostic only at `SD032G:Dev/ZZDosBox/ZZDosBox-Test`, 0.1.0-dev.
CLI: set Stack 65536, then run that executable; it finishes automatically.
Same XX19c single-Core1 ownership restrictions as the NES/SNES workers apply.
RAM:ZZDosBox-Test.log is the last output; persistent evidence is under
`amiga/records/2026-10-12/dosbox-interpreter/`.

Verified actual DOSBox normal x86 interpreter on Core1: original COM fixture,
arithmetic/flags/stack/32-bit operations, RAM checksum f23411c5, guest exit0,
expected DOS text; twelve successful runs plus cooperative cancellation.
MMU/cache/register restoration and cache witness passed; launcher exit0 and
owned memory released. Bridge/Roadie alive and owner port absent afterward.
32 MiB allocation; larger page-table reserve tested at all16,384 alignments.
Do not use the inherited PIC libc's hard-float setjmp; build the soft-float
source as this script does. Diagnostics free callback strings between runs.

This is NOT the complete DOS kernel or a playable PC. INT21h text/exit are
minimal diagnostic adapters; no shell/files/VGA/audio/dynrec yet. ~65ms timing
includes guest reset and paging initialization, not emulation FPS. Next is the
real DOS kernel and file adapter, followed by video/input/audio and separately
validated ARM dynrec. No unrelated startup/firmware settings changed.


## 2026-10-12 — DOSBox real DOS kernel: physical COM/EXE and file round trip

User requested continuing the port. Separate new sources `amiga/dosbox/dos/`
and builders `scripts/build_dosbox_dos.py`, `scripts/build_dosbox_dos_arm.py`
now link upstream DOS services, EXEC, MCB/PSP memory and a bounded private
C: drive. Original COM and MZ EXE fixtures only, no downloaded DOS/game image.
Read `docs/amiga-dosbox-feasibility.md` before continuing.

Installed 0.2.0-dev at `SD032G:Dev/ZZDosBox/DOS-Kernel/ZZDosBox-DOS` with
`FileTest.com`, `ExeTest.exe`, `INPUT.TXT`. Both execute with exit 0 on Core1;
file read/write/reopen/seek/EOF/missing-file checks and MZ relocation,
resize/allocate/free pass. Retrieved `FileTest.out` (256 bytes) matches input
exactly, SHA256 `34e74cd225ecbf83b8c1fd7424fbfa663bcfff126e5d23a674668c67ab9da38e`.
Cancellation, 14 restoration registers, cache witness and owned-memory release
pass; launcher exit 0. Final state: Workbench, Roadie/bridge present, no Core1
owner. Native ASan and ten ARM-model cases pass. Evidence:
`amiga/records/2026-10-12/dosbox-dos/`. Executable SHA256:
`84df1354c92cd130cc20c9531838caacb16ff718ee7c0bdec6b2945dca344755`.

This is staged file access (8 files, 64 KiB each), not arbitrary AmigaDOS
access. No shell/VGA/audio/BIOS peripherals/dynrec yet. Shared uncached region
ends at allocation+0x1c0000; total allocation remains 32 MiB with existing
mapping/MMU/teardown invariants. No NES/SNES or shared launcher edits.

Mac MCP restarted using `make amiga-hardware`; only service startup, no
configuration changes. MakeDir with a trailing slash failed; corrected path
without trailing slash and CRC-verified transfer succeeded. First log pull
timed out but retry succeeded. New executable did not crash the machine.
Toolchain container now has ARM C++ development packages; DOS source units
need `-fno-freestanding` for C++ headers, but no libstdc++ archive is linked.
Next: interactive DOS text console/shell before VGA/input/audio. No GitHub
publication performed this turn.


## 2026-10-12 — ZZDosBox 0.3.0-dev interactive console

User requested continuing the DOSBox build. Installed separately at
`SD032G:Dev/ZZDosBox/Console/ZZDosBox`, with `Echo.com`, `IoTest.com`,
`INPUT.TXT` and executable script `Start-ZZDosBox`. Launch with
`Execute SD032G:Dev/ZZDosBox/Console/Start-ZZDosBox` (sets 65536-byte stack).
Default demo accepts text; `exit` quits normally, window Close cancels.
The executable can also accept one COM/EXE path. This is a small DOS program
runner, NOT the upstream DOS shell or a playable PC emulator.

New sources `amiga/dosbox/console/`; existing DOS adapter adds optional
streaming callbacks and resets DOS echo state after a cancelled run.
Build both existing DOS scripts with `--console`; tests in
`tests/amiga/dosbox/check_console_arm.py`. Physical evidence and build hashes:
`amiga/records/2026-10-12/dosbox-console/`. Native ASan, earlier DOS file/EXEC
regression and six ARM-model cases pass. MMU alignment test covers 16,384
alignments. The shared launcher sources and NES/SNES installations are unchanged.

Five physical checks passed on the final binary: 5,032-byte DOS console API
transcript (`5ade1b27`, no mismatches), input-wait cancellation, full-output
cancellation, typing hello/exit into the native window (118-byte expected
transcript `2e350fbb`), and clicking Close while waiting for input. All report
restored registers/cache witness, launcher exit 0 and owned-memory release.
Screenshot `console.png` is in the evidence folder; original also under
`.context/amiga/dosbox-console/hardware/`. Final state: Workbench, bridge and
Roadie present, Core1 free. No firmware/startup changes or GitHub publication.

The first AmigaDOS CON-handler approach did not display a window. Break on
its identified CLI stopped it with clean teardown; no reboot. The final UI
uses an explicit Intuition window and async console.device reads, aborting
and waiting for outstanding input before closing the device/window. The ARM
transport uses 256-byte RX and 4096-byte TX rings inside the existing uncached
shared region. Counter wrap, backpressure and blocked cancellation are tested.

Xani's guidance (relayed by owner) supports preserving the PicoDrive-style
execution model for XACP 1.8+. It does not prove our extra MMU/debugger paths
on later firmware or our own 68030 compatibility. Future audio should measure
AHI servicing/synchronization and batch exchanges, not just lower sample rate.
Next: upstream shell and BIOS services, then VGA/input and audio. No FPU,
dynrec, PC scan-code mapping or general host-filesystem mount yet.

## 2026-10-12 — ZZDosBox 0.4.0-dev shell and supplied game probes

Continued the approved DOSBox port; user supplied Vette and Commander Keen 6
ZIPs for testing. Added actual upstream shell and live flat C: drive, installed
separately at `SD032G:Dev/ZZDosBox/Shell/ZZDosBox`. Default root is `CDrive`;
`Execute SD032G:Dev/ZZDosBox/Shell/Start-ZZDosBox` sets stack 65536 and opens the
prompt. CLI `--drive drawer` selects a flat 8.3 root; `--command command`
queues one command then EXIT. No game graphics or PC keyboard/audio yet.

Sources `amiga/dosbox/shell/`, builder `build_dosbox_shell.py` (native ASan +
filesystem facade ASan/UBSan), `build_dosbox_dos_arm.py --shell` (ARM/68k), and
`tests/amiga/dosbox/check_shell_arm.py`. Six model cases passed at two relocation
bases; game startup probes additionally pass with edge-bound guards. New live
filesystem uses 4 KiB chunks and 16 handles; path/linked-file restrictions,
truncation/seek, large files, enumeration and handle cleanup are tested.
Existing XX19c single-owner, mapping/MMU/cache/teardown rules still apply. Shared
launcher, NES/SNES sources, firmware and startup settings were not changed.

Physical DIR, TYPE, redirection, COM/EXE launch, COPY and REN passed. Pulled
RENAMED.BIN (100003 bytes), SAVED.TXT (12) and OUTPUT.TXT (256) match expected
bytes. Input-wait cancellation, interactive DIR/EXIT, window Close also pass.
Evidence `amiga/records/2026-10-12/dosbox-shell/`; screenshot original under
`.context/amiga/dosbox-shell/hardware/shell.png`. All six physical runs restored
14 registers with zero mismatches, clean cache witness and owned-memory release.

Vette: 25 original files/706099 bytes in `Shell/Games/VETTE`. Its unchanged
packed EXE initially reports corrupt when loaded low. Original test helper
LOADFIX.COM (source `shell/loadfix.S`) reserves 64 KiB before EXEC; then
`loadfix vette.exe` reaches BIOS graphics AH=00 AL=0E on the physical A9.
Keen: five original files/714476 bytes in `Shell/Games/KEEN6`; `keen6c` reaches
an unimplemented PC BIOS call AX=0000. Both match model results, fault cleanly
(worker 1/launcher 20), and make zero file writes. They are NOT playable or
FPS benchmarks. Game transfers CRC-verified; game data stays out of GitHub.

Read `docs/amiga-dosbox-feasibility.md` for limits and full results. Next is PC
BIOS/device integration, EGA/VGA memory/ports and direct framebuffer output,
then PC keyboard and sound. No GitHub publication this turn. Final console
closed; Workbench/bridge/Roadie available and Core1 free.

Installed executable (pulled back and byte-compared with final build): 470500
bytes, SHA256 `97cced163c25f0a116036ed9c461e204861d30fcc0d3d2c6c0b126c9576c5de3`.

## 2026-10-12 — ZZDosBox 0.5.0-dev PC BIOS/EGA/VGA milestone

Continued the DOSBox port with actual upstream PIC/PIT, keyboard controller,
CMOS, BIOS and INT10/EGA/VGA. New editable sources `amiga/dosbox/pc/` and
builder `scripts/build_dosbox_pc.py`; ARM/68k build adds `--pc` to
`build_dosbox_dos_arm.py`. Toolchain connection currently needs explicit
`--container-command '["podman", "--connection", "nuflix-converter-root"]'`.
The default root connection points to a stopped VM; do not start/replace it
just for this build. The existing ARM toolchain container is on nuflix.

Installed separately at `SD032G:Dev/ZZDosBox/PC/ZZDosBox`; EGATEST.COM in
`PC/CDrive`. Existing Shell executable and game drawers remain available.
Final executable 692312 bytes, SHA256
`4fddc62bf3ea5a5790fa8ca48a4c0f8916d292ced5f0fd3171b03b73ca052493`.
CRC-verified deployment. Use stack 65536 and `--command egatest`, or
`--drive SD032G:Dev/ZZDosBox/Shell/Games/KEEN6 --command keen6c`, or
`--drive SD032G:Dev/ZZDosBox/Shell/Games/VETTE --command "loadfix vette.exe"`.
This fullscreen PC prototype requires a command; the older Shell app still
provides the interactive prompt. No Workbench launcher/game frontend yet.

Actual complete ARM frames are copied directly into locked P96 RTG memory;
68k only coordinates descriptors, file service and deferred PAN. Triple
640x480 BGRA/ARGB buffers, aperture/nonce checks, temporary NC/XN mappings
removed before completion, bitmap ownership retained until ACK or Core1 reset.
Shared launcher/MMU and NES/SNES sources unchanged. Same 32 MiB owned block;
2 MiB PC RAM and 2 MiB runtime heap (upstream paging metadata ~21 MiB).
Native/model checks do not establish tear-free scanout. The ARM RTC is fixed
at 1980-01-01 and uses a static buffer instead of unsupported libc TLS.

Native ASan COM/EXE/EGA/PIT passes; native games reach Keen intro and Vette
car selection. ARM-model two relocation bases/both formats verify all 307,200
pixels and unmap; bad-nonce case rejects and unmaps. ASan/UBSan descriptor
checks cover 7,440 geometries. Model must preserve Thumb state on each quantum
resume because soft-float code includes Thumb; the initial model stall was
that test-harness issue, not a physical ARM fault.

Final physical EGA/PIT: 18 frames/274 emulated ms/1871 host ms, normal DOS return.
Keen: 558 frames/8000 emulated ms/41335 host ms (~13.5 generated FPS across
startup). Vette: 558/8000/43722 (~12.8, includes a screenshot request). Both
reach automatic timed cancellation (worker 2/launcher 0); 14 checked registers,
cache witness and owned memory restore cleanly. No game file writes. Actual
screenshots show Keen startup and Vette Corvette selection. Evidence:
`amiga/records/2026-10-12/dosbox-pc/`; original PNGs under
`.context/amiga/dosbox-pc/hardware/`.

Not playable yet: no live Amiga-to-PC keys or audio. The test injects Enter at
4 emulated seconds, release at 4.1, stops at 8; host deadline 90 real seconds.
F10 requests cancellation. Timing excludes launcher startup and includes disk
service/display waits; do not call 558/8 a 70 FPS hardware result. Coarse 20 ms
polling plus synchronous frame-grant/PAN waits is a clear optimisation target.
Next: live PC keyboard mapping, profile CPU/copy/coordination separately,
reduce waits/batch exchanges, then sound. No firmware, startup or GitHub
publication changes. Final physical runs leave Workbench/bridge available
and Core1 ownership released.

## 2026-10-12 — ZZDosBox host wait optimisation

User requested optimisation opportunities. Replaced unconditional host DOS
`Delay(1)` with synchronous timer.device waits; new default 1 ms. CLI
`--poll-ms 1|4|20`; 20 retains the old tick wait for A/B tests. Editable changes
only in `amiga/dosbox/pc/host.c` and `display.c` (observed copy timing). ARM
ELF is byte-identical to the previous milestone. MMU/ownership/ACK/reset
rules, x86 cycle budget, ROMs and frame counts unchanged. Timer failure
requests cooperative stop; no host busy spinning or outstanding timer IO
at cleanup.

Controlled physical comparison, 558 frames and 8000 emulated ms each, no
screenshots: Keen legacy 41.397s/13.48FPS → 1 ms 19.148s/29.14FPS (2.16x).
Vette legacy 42.166s/13.23FPS → 1 ms 25.298s/22.06FPS (1.67x). Keen 4 ms:
20.470s/27.26FPS, 4384 polling waits versus 12534 at 1 ms (~65% fewer wakeups).
Legacy tick waits averaged 32 ms on this machine. Times exclude launcher
startup but include loading/service/display waits; these are NOT steady
gameplay benchmarks or full-speed PC emulation. No 030 or audio test yet.

All comparison runs, EGA/PIT and input-wait cancellation passed, with clean
register/cache-witness restoration and owned-memory release. Final build
repeat: Keen 19.197s, Vette 25.294s. Observed grant-to-copy-complete totals
4.711s/4.690s (~8.4ms/frame); PAN ACK observed waits 5.150s/5.049s. These are
host-observed latency (including polling), not isolated ARM kernel timings.
Polling sleep overlaps ARM work; do not add it to copy/ACK times or call it
CPU utilisation. Remaining targets: pipeline frame/copy/PAN safely, specialise
common-width scaler loops, then profile interpreter/VGA work. No tear-free
claim added.

Updated normal executable `SD032G:Dev/ZZDosBox/PC/ZZDosBox`: 695036 bytes,
SHA256 `11e71fdbfa8f9e561fb7ea8f0c09843986bd0dfaa015210f8cfd14dd42832627`.
Previous installed 692312-byte executable preserved as `PC/ZZDosBox-PreWait`;
`PC/ZZDosBox-Opt` is the tested identical new candidate. Still 0.5.0-dev,
requires --command, 8-second emulated diagnostic run and injected Enter as
before. Records: `amiga/records/2026-10-12/dosbox-scheduling/`. No firmware,
startup or GitHub publication changes.

Post-promotion default-path EGA/PIT smoke passed with `POLL ms=1`, clean
restoration and owned-memory release. Final state: Workbench and bridge
available, no Core1 owner. No user-facing gameplay controls were added here.

## 2026-10-12 — ZZDosBox ARM scaler optimisation

Continued optimisation with `amiga/dosbox/pc/scale.h`, used by `direct.cpp`.
320-pixel rows use paired stores; 640-pixel rows copy directly; format choice
is outside the inner loop. Other widths retain exact nearest-neighbour
scaling. All output pixels, vertical scaling, row cancellation, mapping/nonce
checks and ACK/reset-before-free ownership rules are preserved. Scalar integer
code only; no VFP/NEON, firmware or shared-launcher changes.

Native ASan/UBSan (`tests/amiga/dosbox/scale.cpp`) and compiled A9 row model
(`check_scale_arm.py`) pass all 1,024 widths and both formats, 1,310,720 exact
pixels each, with source/destination bounds. Full linked ARM PC-model tests
pass both relocation bases/formats and bad-nonce unmap. Build checks fit the
existing allocation/image/BSS limits.

Fresh hardware before/after comparison, 1 ms host wait, 558 frames and 8,000
emulated ms each, no screenshots:

- Keen: 19.195 s / 29.07 FPS -> 16.780 s / 33.25 FPS (+14.4%).
- Vette: 25.355 s / 22.01 FPS -> 23.055 s / 24.20 FPS (+10.0%).

Observed copy phase fell 8.43 -> 4.18 ms/frame (Keen), 8.44 -> 4.25 (Vette).
These are host-observed latencies including mapping/copy/unmap/polling, not
isolated kernel timings. FPS is timed startup throughput; still below real-time
PC emulation. EGA/PIT, both games and input-wait cancellation restore checked
registers/cache witness and release owned memory. Zero game file writes.

Updated normal `SD032G:Dev/ZZDosBox/PC/ZZDosBox`, 695244 bytes, SHA256
`68faf71c85135dc1e915141a82d821a522a7769598b82596d0c6077ec4ed4b4b`.
Previous 695036-byte build preserved as `PC/ZZDosBox-PreScale`; older
`PC/ZZDosBox-PreWait` remains. Still 0.5.0-dev, same command-line timed test,
no live gameplay keyboard/audio. Records:
`amiga/records/2026-10-12/dosbox-scaler/`. Next: safe overlap of frame production,
copy and PAN; profile interpreter versus VGA work. No GitHub publication.

Post-install default-path EGA/PIT smoke passed. Final state: Workbench/bridge
available and Core1 ownership released.

## 2026-10-12 — ZZDosBox copy/PAN pipeline experiment

A bounded two-copy batch can use the third bitmap slot while the previous
PAN awaits ACK. Production ownership helpers passed an ASan/UBSan event
model (168,000 frames, wrap, slot exclusivity, bounded unlock intervals,
timeout retention). Physical EGA/PIT, both games and Ctrl-C during Keen
startup passed restoration/release. Final normal game completion now drains
the last PAN, giving 558 ACKs for 558 frames.

Same-binary serial vs pipeline: Keen 16.808 vs 16.749 s (33.20 vs 33.32 FPS),
Vette 23.092 vs 23.071 s (24.16 vs 24.19 FPS). Both generated 558 frames / 8,000
emulated ms, 1 ms poll. ZERO overlapping copies: no meaningful gain, and no
physical coverage of the overlapping-copy branch. Source defaults to serial;
`--pipeline-video` opts in, `--serial-video` forces serial. No tear-free claim.
Tested default-pipeline candidate SHA256
`1240008bff62accb65f8bfbcbc6e6ed209129d5094d45530ad8e82e78e0123c5`
was only staged as `PC/ZZDosBox-Opt`, not promoted. ARM ELF unchanged.
Records: `amiga/records/2026-10-12/dosbox-pipeline/`.

## 2026-10-12 — ZZDosBox interpreter -O2 installed

Following the pipeline experiment, compile only upstream core_normal.cpp with
-O2 rather than -Os. All other ARM units remain -Os. PC builds default to O2;
`--pc-cpu-opt Os` reproduces the previous compiler setting. Build provenance
records per-file overrides. No dynrec, NEON/VFP, cache, cycle-budget or scaler
change. Serial display remains default, pipeline explicit opt-in.

ARM image 538,432 bytes (+16,648), within the same 32 MiB allocation. Linked
ARM model passes both relocation bases/formats, exact EGA pixels, arithmetic/
memory, DOS file and MZ relocation fixtures, bad nonce and mapping revocation.

Physical same-work startup comparisons (558 frames / 8,000 emulated ms, 1 ms
poll, no screenshots): Keen 16.808 s / 33.20 FPS -> 15.608 s / 35.75 FPS (+7.7%);
Vette 23.092 s / 24.16 FPS -> 22.157 s / 25.18 FPS (+4.2%). Single-run measurements,
not steady gameplay or real-time emulation. All 558 frames ACKed, zero game
file writes, register/cache witness restored and owned allocation released.
Physical EGA/PIT, input-wait cancel and active Keen Ctrl-C (216 frames,
3,108 emulated ms, timed_done=0) also passed. The rebuilt default is byte-identical
to the tested explicit O2 candidate.

Installed `SD032G:Dev/ZZDosBox/PC/ZZDosBox`: 714,964 bytes, SHA256
`927b5dd8738de7e0b44e6a08dc289a215dc7741fd2a673e0cecca4ec8dfb6379`.
ARM ELF SHA256
`83f12352d8a6607e32af8882fd422d85181d4966799a432e45adfaaca8d30ad7`.
Previous scaler build (695,244 bytes) preserved as `PC/ZZDosBox-PreCPU`;
older PreScale/PreWait backups retained. Still 0.5.0-dev; gameplay keys/audio
pending. Records: `amiga/records/2026-10-12/dosbox-cpu-o2/`. No firmware,
startup or GitHub changes. Next: profile CPU/VGA, evaluate bounded ARM dynrec
with code-cache synchronisation; input/audio are separate functional work.

Post-install default-path EGA/PIT passed (18 frames/18 ACKs). Final state:
Workbench, Roadie and bridge available; no Core1 owner.

## 2026-10-12 — ZZDosBox profiling and RGB565 conversion installed

Continued optimisation by adding optional `--profile`: exclusive 64-bit ticks
from the already mapped global timer, without changing timer/PMU registers.
Buckets: interpreter (includes guest memory/VGA handlers), PIC/timer/VGA events,
colour capture, framebuffer grant wait, mapped scale/copy and other work
(including DOS/BIOS/file/startup/cleanup). Totals published only at completion;
profiling OFF by default. Explicit timing scopes unwind before teardown on
longjmp exits. `scripts/analyze_dosbox_profile.py` requires exact exclusive sums.

Profiles showed capture taking 27.1% of Keen and 22.9% of Vette ARM run time.
`pc/capture.h` now converts RGB565 through two 256-entry constant tables
(2 KiB), bit-identical for all 65,536 colours. Format branch is outside each
row's loop; 32-bit rows use the existing scalar memcpy. No NEON/VFP, dropped
frames, guest cycle-budget, cache-policy or display-ownership change.

Native ASan/UBSan and A9 converter model pass all colours, 1,024 widths and
8/16/32 bpp, with guards; native tests cover four source alignments. Full
linked ARM model passes both relocations/formats, exact pixels, arithmetic/
DOS/MZ, profile publication/sums, invalid nonce and cancellation. Native
profile model covers nesting, unwind, timer-low wrap and depth errors. The
isolated converter harness initially linked libc's VFP memcpy; corrected to
the production scalar memory.c before hardware testing. Production uses the
scalar helper throughout.

Fresh physical comparison, profiling disabled, 1 ms poll, 558 frames / 8,000
emulated ms, no screenshots:

- Keen: 15.511 s / 35.97 FPS -> 13.332 s / 41.85 FPS (+16.3%).
- Vette: 22.093 s / 25.26 FPS -> 19.337 s / 28.86 FPS (+14.3%).

All 558 frames ACKed, zero file writes, checked registers/cache witness restored,
owned memory released. Single-run startup throughput, not steady gameplay or
real-time emulation. New instrumented runs halve conversion ticks (49.8% Keen,
49.4% Vette); interpreter/events now roughly 60%/65% of ARM time combined.
Their instrumented FPS is not the headline result. EGA/PIT, input-wait cancel
and active Keen Ctrl-C (258 frames, 3,711 emulated ms, timed_done=0) pass.

Installed normal `SD032G:Dev/ZZDosBox/PC/ZZDosBox`, 720,548 bytes, SHA256
`a3e8c8cf41590d5b72b53bb2896ba100824b14c4954311c7bf8f20920fb3a034`.
ARM ELF SHA256
`36a8d323e72bf922c4a1852aa777569aa0988b893d64e3d684b6e88edafc8558`.
ARM image 541,596 bytes; same 32 MiB owned allocation. Previous interpreter
build (714,964 bytes) preserved as `PC/ZZDosBox-PreVGA`; older backups retained.
Still 0.5.0-dev; gameplay keyboard/audio pending. Records:
`amiga/records/2026-10-12/dosbox-profile/`. No firmware/startup/GitHub changes.
Next performance targets: interpreter/guest memory and PIC/VGA event scheduling;
bounded ARM dynrec remains a separately validated future step.

Post-install default-path EGA/PIT passed (18 frames / 18 ACKs). Final state:
Workbench, Roadie and bridge available; Core1 ownership released.


## 2026-10-12 — ZZDosBox PIC/VGA optimisation installed

PC builds now default to `-O2` for upstream pic.cpp, vga_draw.cpp and
vga_memory.cpp, alongside the existing interpreter `-O2`. Other ARM units
remain `-Os`. `--pc-hardware-opt Os` reproduces the preceding module settings;
per-file flags are in cache keys and build.json. Source algorithms, guest
cycle budget, soft-float and display ownership remain unchanged.

Fresh physical profiling-disabled comparison (558 frames / 8,000 emulated
ms, 1 ms polling, no screenshots):

- Keen: 13.291 s / 41.98 FPS -> 12.990 s / 42.96 FPS (+2.3%).
- Vette: 19.321 s / 28.88 FPS -> 18.192 s / 30.67 FPS (+6.2%).

Both pairs match slice counts, file reads and console hashes. All frames
ACKed, zero file writes, checked registers/cache witness restored and owned
memory released. Single-run startup throughput, not steady gameplay FPS.
Separate profiles show event ticks down 14.5% / 18.9%; interpreter time barely
changes. Profiling remains off by default.

Full linked ARM model passed both relocations/formats, exact EGA pixels,
CPU/DOS/MZ fixtures, profile sums, nonce rejection and cancellation. Physical
EGA/PIT, input-wait cancel and active-game Ctrl-C pass; the latter stops Keen
at 262 frames / 3,757 emulated ms, timed_done=0. Default build is byte-identical
to the explicitly selected hardware-O2 candidate.

Installed `SD032G:Dev/ZZDosBox/PC/ZZDosBox`: 728,548 bytes, SHA256
`12667663cfa1426196620bfa5a5396ba9f64e615098f971d373a1d72bc01a519`.
ARM ELF SHA256
`885a85f983e344a482f14595f8c81ba7732b01923c46e796f7635c01e15a0975`.
ARM image 549,576 bytes (+7,980); unchanged 32 MiB owned allocation. Prior
720,548-byte converter build retained as `PC/ZZDosBox-PreEvents`; older backups
retained. Version 0.5.0-dev. Live PC controls/audio remain pending.
Records: `amiga/records/2026-10-12/dosbox-events/`. No firmware, startup or
GitHub publication changes. Remaining large costs: interpreter/guest memory
and PIC/VGA events. Bounded ARM dynrec is a separate future validation task.

Post-install default-path EGA/PIT passed (18 frames / 18 ACKs). Final state:
Workbench, Roadie and bridge available; Core1 ownership released.


## 2026-10-12 — ZZDosBox MUI launcher and Workbench DOS disks

Installed **0.6.0-dev**, still based on `libretro/dosbox-libretro`, not DOSBox-X.
Open `SD032G:Dev/ZZDosBox/PC/ZZDosBox-Launcher` (Workbench Tool icon,
65,536-byte stack, MUI 3.8+). Tabs: Programs, AUTOEXEC.BAT, CONFIG.SYS,
DOS terminal, Settings, Disks, About. Per-program profiles include optional
LOADFIX, editable/default startup batch, memory 1/2 MiB, VGA/EGA/CGA,
1000/3000/6000 cycles, colours and timed/unlimited sessions. Current profile
seeds: DOS hard drive, Commander Keen 6, Vette, Elite. Config and backups live
beside the launcher. `Launcher.pre-ui` / `Launcher.pre-prompt` retain initial
UI configs; normal saves use `.bak`. Default AUTOEXEC omits unsupported PROMPT.

Writable C: drawer: `SD032G:Dev/ZZDosBox/Drives/C`; selectable in the launcher
and accessible directly from Workbench. Roughly 27 GiB free on the SD volume
at setup; not reserved. DOS free-space reporting caps at ~2 GiB. The current
bridge supports root-level 8.3 files only, not guest subdirectories. Existing
games retain their own C: drawer profiles.

User-provided DOS media inventoried/extracted under `.context/amiga/msdos622/`.
Four original 1.44 MiB installation/supplement images plus the supplied raw
boot floppy transferred with CRC verification to `PC/Disks/`. Installed
previously absent `DEVS:diskimage.device` 37.7 by Thore Boeckelmann; existing
`L:fat95` retained. App-local `Disks/DiskImageCtrl`, generated mountlists in
`Disks/Mounts`, and Mount-DOS-Disks / Eject-DOS-Disks scripts with IconX icons.
Units 0–4 map DOSD1, DOSD2, DOSD3, DOSSUP, DOSBOOT. All mount **read-only** and
appear in Workbench. UI eject, remount and repeated mount tested. No startup
changes; use Disks > Mount in Workbench after reboot. The ISO and DOS EXE
alternatives remain local; never executed the EXE on the Mac.

This does **not** install or boot MS-DOS 6.22. BIOS image boot, XMS/EMS,
CONFIG.SYS drivers, audio and joystick remain pending. CONFIG.SYS is explicitly
an editor/export reference, not active guest configuration. Host directory
mounting, BIOS image boot and CONFIG.SYS support must not be conflated.

Physical checks: configured EGA (1 MiB / 1000 cycles) and CGA (2 MiB / 6000)
fixtures passed with 15 frames/ACKs each. DOS wrote a test file to the shared
C: drawer and Workbench read its content. Live rawkey 'a' reached PC BIOS INT16
and printed PC keyboard PASS. Unlimited Keen stopped via F10 after 281 frames
/ 4,050 emulated ms; all frames acknowledged. Launcher button generated and
ran its default batch, opened the themed terminal, accepted `exit`, removed
its temporary batch/session and returned to MUI, with no illegal command.
All successful runs restored checked ARM registers/cache witness and released
owned memory; bridge and Roadie remained available.

Initial testing caught two host bugs: keys were polled before the bitmap was
unlocked, and stop during copy became a generic error. Polling now happens
between unlock and the next grant; F10 is distinguished from transport faults;
the ARM revokes the temporary mapping before cooperative stop. Full linked
ARM suite passed, including copy-time cancellation, both relocated images,
pixel formats, nonce rejection, file/DOS fixtures and invalid options. Native
ASan/UBSan profile/session/script tests passed. Seven UI tabs captured under
`.context/amiga/dosbox-launcher/`; these checks do not prove steady gameplay
speed, audio, disk boot or general game compatibility.

Installed runtime: 754,724 bytes; SHA256
`24b15c69a1e0587ed1c00ae08282411884582b34510e83eb05c02634d382d0f7`.
ARM ELF SHA256 `9ca462ae74e999648215934bde3416c2f19a38814289d5c429bd7a4ccf8c9a3a`.
Launcher: 94,408 bytes; SHA256
`b647fbdceb44222e5d344475a8f4e43a75dadac3a268ab036903088682625313`.
Previous runtime preserved as `PC/ZZDosBox-PreLauncher`. Core1 ownership and
32 MiB allocation/mapping/nonce/reset-before-free contracts unchanged.
Records: `amiga/records/2026-10-12/dosbox-launcher/`. No OS/game binaries in
records; no GitHub publication this turn.
