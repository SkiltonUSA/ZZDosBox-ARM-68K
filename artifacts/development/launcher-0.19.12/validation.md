# 0.19.12-dev validation

## Completed

- m68k launcher and all seven host variants compile with warnings as errors.
- Every variant retains byte-identical ARM image, mapping probe and relocation
  table from its previous runtime. No ARM performance change is claimed.
- Host review tests pass: bounded console synchronization, receive queue handling,
  presentation/flip ownership and latency schedules. Developer/profile tests pass.
- Native DOSBox executes the rebuilt DOS welcome program. The ASCII cells use
  yellow-on-blue attribute 0x1e; all four shortcut labels render in bright red
  while descriptions remain white. Ctrl+F5 is present and the GitHub URL is absent.
  DOS version, coloured border and return to prompt pass. The PNG is that native
  execution, not an Amiga screenshot.
- Welcome file tests pass: known-version upgrade, original backup, repeat launch,
  unknown-file preservation and failed-rename recovery.
- Library preview layout and Ctrl+F5 loading instructions were displayed on the
  physical Amiga. Screenshot runtimes were transferred and checksum verified;
  library/configuration files were byte-identical across installation.
- Earlier library-control checks passed: Rename preserves settings, mounted images
  enter OS without duplication, and No/protected/shared/OS deletion cases retain
  files. Yes removes a disposable private game folder, nested save and entry.
  All disposable fixtures were removed; the original library was restored.

## Pending physical checks

The target bridge became unresponsive during the launch check. Physical capture,
hover and runtime lifecycle checks remain pending.

- Ctrl+F5 file creation and preview refresh on both fullscreen and windowed paths.
- Footer hover behaviour, including leaving the Delete button.
- Clean player exit and Core1 release for these relinked runtimes.
- Final launcher/welcome installation. The currently transferred launcher is an
  earlier diagnostic build, not the final binary in this directory.

Do not treat these binaries as a fully hardware-validated runtime replacement.
No new benchmark result or FPS improvement is claimed.
