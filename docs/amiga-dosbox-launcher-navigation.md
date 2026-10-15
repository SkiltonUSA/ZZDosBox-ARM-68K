# ZZDosBox launcher navigation

Launcher **0.19.3-dev** replaces the main row of tabs with a persistent left
sidebar. Select a section with the mouse, or focus the list with Tab and use
Up/Down. The highlight and page heading identify the current section. The
selected program stays visible above its settings.

| Section | Contents |
| --- | --- |
| Load program | All programs, Games, Applications and OS; add, edit and launch |
| DOS terminal | DOS selection, terminal colours, commands, AUTOEXEC.BAT, CONFIG.SYS and CD-ROM |
| CPU & memory | Guest RAM, CPU cycles and session duration |
| Display | Window/fullscreen, window size, video adapter, monochrome colour, renderer, presentation, scanlines and FPS |
| Audio | MIDI output and CAMD/AHI/ZZ9000AX driver detection |
| Network | TCP/IP or IPX configuration, connection status and diagnostics |
| Disks | Boot images and Workbench mounting/ejection |
| Developer | Diagnostics, memory tools and dosbox.conf |
| Compatibility | Offline DOSBox community reports |
| About | Version and credits |

**Save settings** stays below the sidebar; operation messages stay along the
bottom. Navigation preserves in-progress edits. Category Edit returns to the
program editor and keeps the sidebar in sync. Small secondary tab groups are
retained only within their own section. No separately installed custom MUI classes or MUI 4 upgrade are required;
the launcher retains its MUI 3.8 requirement.

During an active network/developer session, the launcher selects the relevant
section and temporarily disables the sidebar. Its live controls remain
available, including cooperative Disconnect/Stop. Navigation returns when the
session ends. This change does not alter emulator execution or profile formats.

## Program-list keyboard controls — 0.19.10-dev

In **Load program**, click the right-hand program list or use Tab to focus it.
The same controls work in **All**, **Games**, **Applications** and **OS**:

- **Up / Down:** select the previous or next entry and scroll it into view.
- **A–Z:** jump to the first visible-list title starting with that letter;
  for example, **G** selects the first G title. Matching ignores case.
- **0–9:** jump to the first title starting with that number.

If nothing matches, the selection stays unchanged. Arrow keys stop at the
first/last entry. A letter selects the first match each time; it does not
cycle through matches or launch a program. Text fields and the left sidebar
keep their normal keyboard behaviour. Switching program categories focuses
their program list. Use **Launch** to start the selected program.

The keyboard handler is built into the launcher and uses AmigaOS's keymap.
It adds no external MUI class or emulator runtime dependency.

The ZZDosBox logo sits above the navigation column, embedded in the launcher
so no separate artwork file is needed.

## About and version history

About includes a scrollable changelog beneath the credits, newest version
first. It contains every recorded numbered milestone from 0.1.0 onwards and
is generated from the project changelog during the build.

## Startup

The embedded ZZDosBox logo and launcher version appear for two seconds before
the main window opens. Click the panel to continue immediately. The panel
uses native MUI classes and needs no external image file.

Launching a game, application or DOS session shows a four-second **LOADING**
card before handing control to the runtime. Click to skip. It lists Alt+Return
for window/fullscreen, F10 to return, Ctrl+= (the plus/equal key) for faster
cycles, and Ctrl+- for slower cycles. This is a timed introduction, not a
progress meter for disk reads or guest startup.

## Design basis

The supplied MUI Preferences screenshots inspired the sidebar and selected
page layout. The design uses visible, left-aligned labels, a persistent location
indicator, grouped settings and a stable save action. Developer tools remain
separate from common launch/display tasks. The application inherits the user's
MUI theme rather than imposing colours or copying artwork from the references.

References: [NN/g: vertical navigation](https://www.nngroup.com/articles/vertical-nav/),
[NN/g: progressive disclosure](https://www.nngroup.com/articles/progressive-disclosure/),
and [MUI Group documentation](https://github.com/amiga-mui/muidev/wiki/MUI_Group).

Version 0.19.1 adds native menu icons in a separate column so all captions
align, framed page headings and consistent
button/panel spacing. Screen and Rendering share panel and label widths; the
FPS checkbox aligns with their controls. App-local Helvetica (13 px body,
18 px heading) and Courier (13 px editor) fall back to MUI fonts if unavailable.
Workbench font and colour preferences are not modified.

## Validation

The m68k launcher builds with warnings treated as errors. Existing native
profile/session/developer checks pass. On the A4000TX, all ten sections were
reviewed, and Tab followed by Up/Down operated the sidebar. A fresh copy of
all 21 profiles remained byte-identical after a full navigation-and-save pass.
The IPX terminal selected Network, disabled navigation while active, then
Disconnected cooperatively and restored navigation. Its host log confirms
zero network drops/errors, restored SIMD/MMU/cache witnesses and released
owned memory. No emulator runtime changes were needed.

Screenshots and checks are recorded in `.context/amiga/dosbox-sidebar/`.
Physical mouse resizing remains unverified. A bridge-injected size gesture
stalled remote UI control during the initial review; the bridge subsequently
recovered, and no emulator/Core1 session was active at the time. Do not inject
Workbench drag or size gestures.

Installed on the A4000TX with `.pre019` launcher/settings backups. Installed
profiles and dosbox.conf remained byte-identical; emulator runtimes were unchanged.

Presentation update 0.19.1 was reviewed across all ten sections and the nested
Memory, dosbox.conf and AUTOEXEC.BAT pages. Screen/Rendering alignment and
Audio caption alignment were verified on the A4000TX. The staged save retained
configuration checksums; installation preserved both configuration files
byte-for-byte. Previous launcher/settings have `.pre0191` backups. Records:
`amiga/records/2026-10-15/dosbox-polish/`.
