# Shareware starter builds

Keep **Doom 1.9**, **Duke Nukem 3D 1.3D** and **Descent shareware** with the
personal ZZDosBox build. Only Doom and Duke are included by the public packaging
mode. These are game data distributions under their publishers' terms, not GPL
components of ZZDosBox.

## Contents and settings

| Game | Guest settings | Observed status |
| --- | --- | --- |
| Doom 1.9 shareware | 8 MB, 386, normal runtime, sound off | Attract demo rendered |
| Duke Nukem 3D 1.3D shareware | 8 MB, 486, EMS, SB16, Expanded runtime | Menu and rendered scene reached |
| Descent shareware (personal build only) | 16 MB, 386, EMS, SB Pro 2, Expanded runtime | Pilot selection reached; later gameplay remains unverified |

All three profiles start fullscreen at 6,000 cycles. LOADFIX, VCPI and paging JIT
are off. Duke's original archive has no saved configuration: run its SETUP.EXE
to select 320x200 and SB16 at 220/IRQ 7/DMA 1 and 5. Original game payloads and
notices are retained unchanged. Original installer files are also included.
The new profiles contain no personal paths, pilots or saves.

Extract a starter ZIP into a **new writable drawer** and open
`PC/ZZDosBox-Launcher`. Do not overwrite an existing `Launcher.cfg` with the
starter library. The usual AmigaOS, MUI, ZZ9000/XACP and RTG requirements apply.
Advanced disk/network/developer support remains part of the full installation;
the starter drawer includes the launcher and the three runtimes used here.

## Rebuild

Supply these pinned input files in a media directory:

- `doom19s.zip`: original id Software 1.9 installer; the two DEICE volumes are
  concatenated and their complete embedded ZIP is unpacked without executing DOS.
- `DN3DSW13.SHR`: the original Duke 1.3D self-extracting ZIP payload.
- Personal build only: the supplied `DOSBOX_DESCENT.ZIP`, plus
  `DESCENT-LICENCE.TXT` extracted unchanged from the original `desc14sw.zip`
  installer. The original installer archive's MD5 is
  `9771e9a03e81fd8a7e32e3d02708328c`.

The script pins SHA-256 for every input and rejects different editions. Provide
a runtime directory containing the tested `ZZDosBox-Compat` and
`ZZDosBox-Expanded` binaries with the DOS temporary-file fix. Other starter
components come from the versioned development artifacts.

```sh
python3 scripts/package_dosbox_shareware.py \
  --media /path/to/shareware-inputs \
  --runtime-dir /path/to/tested-runtimes \
  --output .context/releases

# Personal use only: adds Descent. Do not upload this archive.
python3 scripts/package_dosbox_shareware.py \
  --media /path/to/shareware-inputs \
  --runtime-dir /path/to/tested-runtimes \
  --output .context/releases --personal-descent
```

The library is generated with the production C serializer and read back for
validation. ZIP members are CRC checked and compared with a per-file SHA-256
manifest. Packaging verification is not a new gameplay or audio test. The
startup tests and remaining Descent limitation are recorded in the
[shooter review](amiga-dosbox-shooter-startup-review.md).

## Distribution notices

- Duke's included `LICENSE.TXT`, section 3, permits free online distribution
  with all original files and notices unmodified. Its complete 17-file payload
  is preserved; no preconfigured replacement CFG is inserted.
- Doom's complete installer and 20-file payload are preserved. The id Software
  licence and clarification are recorded by
  [Debian](https://sources.debian.org/src/doom-wad-shareware/1.9.fixed-4/debian/copyright).
- Descent's README permits free unmodified sharing, but the **original 1.4
  installer's LICENCE.TXT explicitly restricts bundling with other products**.
  That stricter notice is retained. Descent stays in the personal package and
  requires redistribution permission before inclusion in a public release.
  [Original installer mirror](https://www.classicdosgames.com/files/games/parallax/desc14sw.zip).

Personal game media and generated personal archives stay outside Git. Keep the
local media directory with private build inputs so a later rebuild retains the
same editions. Public mode does not read or package either Descent input.
No commercial editions, Microsoft software or Quake data are bundled.
