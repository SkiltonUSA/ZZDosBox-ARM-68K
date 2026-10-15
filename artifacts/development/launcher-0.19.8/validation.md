# Validation

- m68k launcher build passed with warnings treated as errors.
- Generated AmigaGuide: seven nodes; all internal links resolve; ASCII output.
- Installed launcher and guide verified by CRC32 after transfer and rename.
- Prior launcher retained as a rollback copy; saved profiles and dosbox.conf
  were byte-identical before and after installation.
- Physical Network guide click opened MultiView. The TCP/IP contents link
  opened the matching guide section; closing it returned to the launcher.
- All ten main sidebar pages opened and were captured on the Amiga.
- No guest/Core1 workload was started. Opening help during an active network
  session is implemented but was not exercised in this UI check.

Launcher: 586,132 bytes, SHA256
`3d5d63fd0d9c168f7dfd94914c6d98bc26dc914b2c7f2685e873c06c04463288`.
Guide: 6,774 bytes, CRC32 `A88EC33B`.

[Screenshots](../../../docs/screenshots/launcher/).
