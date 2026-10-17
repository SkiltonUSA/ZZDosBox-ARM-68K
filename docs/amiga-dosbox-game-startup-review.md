# Game startup review

The 0.20.1 development runtime includes selectable SBPro2/SB16 and PC speaker,
plus consistent host file dates. Installing files or reaching an introduction
does not establish full-game compatibility.

| Supplied title | Finding and action | Evidence |
| --- | --- | --- |
| Pinball Dreams | Original copy lacks INSTALL.SYS. Included installer requires A: or B: and installation media; an empty file is insufficient. No placeholder installed. | Still unresolved; files retained. |
| Pinball Dreams II | Saved sound.cfg selects SB16.SDR. Select SB16 in the launcher. | Native title animation; physical gameplay pending. |
| Pinball Dreams Deluxe | Nested DELUXE/DREAMS assets are required. Existing ZZSTART.BAT enters DELUXE. Enable Sound Blaster. | Native table-selection menu. |
| Pinball Fantasies | Earlier runtime lacks its configured Sound Blaster device. Use Compat with SBPro2, LOADFIX off. | Native Digital Illusions introduction; physical Pinball Fantasies title animation and clean F10 return. |
| Pinball Illusions | Requires command-line ILLUSION.CFG and Sound Blaster; initial loading is slow. | Physical table-selection menu in prior check; full table play pending. |
| Eye of the Beholder I | Direct engine invocation requires `EOB.EXE 3 1 1 homie`. | Native dungeon view; physical title/menu with 8 MB and clean F10 return. |
| Eye of the Beholder II | Missing EOSPREFS.DAT triggers setup, not a game crash. | On hardware saved VGA, Adlib/Sound Blaster and mouse preferences. |
| Eye of the Beholder III | Zero open/stat file dates made compiled resources appear stale and triggered missing source-text errors. Return consistent dates. | Native main menu and physical animated introduction after fix. |
| Unreal (1991) | UNREAL.EXE remains black in extended native tests. The separate DIGDREAM.EXE displays a loader introduction; that does not prove the game works. | Still unresolved; retain original game command. |

The native game harness now copies subdirectories and isolates each run. Earlier
checks that omitted nested assets were invalid and have been superseded. Native
runs use the interpreter, whereas physical tests exercise the ARM dynamic core.

The original game executables were not patched. Launcher commands/settings were
adjusted, and EOB II's supplied setup program wrote its own preferences. Supplied
game data and private installation records remain outside the source repository.
