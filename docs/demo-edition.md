# ZZDosBox release editions

The [0.22.7 Beta release](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/releases/tag/v0.22.7-beta) provides **Standard** and
**With Demos** AmiPkg `.lha` installers. See the [installer comparison](amipkg-installers.md)
for requirements and upgrade behaviour.

The ZIP files below are build inputs for those installers. Both use the same
launcher, runtimes and support files:

| Package | Included games |
| --- | --- |
| `ZZDosBox-0.22.7-beta-standard.zip` | None; an empty library for your own programs |
| `ZZDosBox-0.22.7-beta-demos.zip` | Corridor 7, Doom shareware and King's Quest VII Demo |

Both are fresh-install packages. The Standard edition needs no game media to
build and contains no demo payloads or original game installers.

The demo edition includes **Corridor 7**, **Doom 1.9 shareware**, and
**King's Quest VII's promotional demo**, ready to select in the Games library.

Extract into a new writable drawer and open `PC/ZZDosBox-Launcher`.
Keep an existing installation's `Launcher.cfg`; the included library is for a
fresh installation. Paths are relative to the application drawer.

| Demo | Included profile | Hardware check |
| --- | --- | --- |
| Corridor 7 | VGA, 8 MB, 6,000 cycles, EMS, SB Pro 2, window | Mission, movement and firing |
| Doom 1.9 shareware | VGA, 8 MB, 6,000 cycles, sound off, fullscreen | Attract sequence |
| King's Quest VII | S3/VESA, 8 MB, 3,000 cycles, SB Pro 2, window | Title and illustrated demo sequence |

King's Quest VII is **non-interactive**, rather than a playable adventure.
Press Return at **Include scrolling?**. Startup and scene transitions can leave
the screen black temporarily. Keep SB Pro 2 enabled: disabling the sound card
while retaining Sierra's original sound drivers prevented the demo rendering
in our tests. The fix is in its launcher profile; game files are unchanged.

F10 exits, Alt+Return switches window/fullscreen, and Ctrl+F5 captures a screenshot.
[Amiga requirements](requirements.md) apply. System libraries, firmware and
Microsoft operating systems are supplied separately. Optional guest networking
tools and LOADFIX are not included or required by these demo profiles.

## Package contents

- Launcher 0.22.7-dev, the tested Compat runtime, Expanded 0.22.5-dev and the
  existing display/developer runtime variants.
- Disk utilities, compatibility database, network guide, component notices and
  a fresh three-entry launcher library.
- Complete pinned demo payloads, including their original notices and installers.
- Per-file SHA-256 manifest and archive checksum.

The demo edition does not replace the separate Doom/Duke shareware starter or
the private Descent package. It contains no commercial editions or personal saves.

## Rebuild

```sh
# Without demos; no game inputs required.
python3 scripts/package_dosbox_demos.py \
  --edition standard --output .context/releases

# With the three demos.
python3 scripts/package_dosbox_demos.py \
  --edition demos \
  --media /path/to/demo-archives \
  --kq7-dir /path/to/original-extracted-kq7-demo \
  --output .context/releases
```

Supply `corr7.zip` and `doom19s.zip` in the media directory. Their SHA-256 values
are pinned in the script. `scripts/kq7-demo-files.json` pins every file in the
original extracted Sierra demo, including its four installer volumes. Different
editions, modified configuration and extra files are rejected before packaging.
Game media stays outside the source repository.

Profiles use the production launcher serializer and are read back to verify
cycles, runtime selection and relative paths. ZIP CRCs and every output hash are
checked. These packaging checks do not claim new gameplay or audio benchmarks.
The physical Corridor and King's Quest checks exited cleanly; audio delivery
completed without reported errors, but sound quality was not assessed by ear.

## Provenance and notices

Corridor 7 is Capstone's demo (`.DMO` data and original `FILE_ID.DIZ` retained).
Doom is id Software's complete 1.9 shareware installer and extracted payload.
King's Quest VII is Sierra's English DOS/Windows non-interactive demo: resource
sizes and detection hashes match [ScummVM's detection table](https://github.com/scummvm/scummvm/blob/master/engines/sci/detection_tables.h).

These games retain their publishers' copyrights and terms. ZZDosBox's GPL
applies to the emulator, not the bundled game data. No new licence is asserted
for a demo merely because it is freely downloadable. The supplied Corridor and
Sierra files contain no standalone redistribution licence; public bundling
permission has not been established by this packaging work.
