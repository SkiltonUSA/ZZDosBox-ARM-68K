# ZZDosBox launcher 0.23.0-dev

[Download ZZDosBox-Launcher](ZZDosBox-Launcher).

Launcher-only update for an existing ZZDosBox installation. Requires AmigaOS 3.x
and MUI 3.8 or later; uses the existing emulator runtimes and support files.

Close ZZDosBox, keep a copy of your previous launcher, then replace
`ZZDosBox-Launcher` in the application's `PC` drawer with this executable.
Set its executable protection bit if your transfer method removes it:

```text
Protect ZZDosBox-Launcher +e
```

Profiles, DOS configuration and game files stay in place. The complete beta
installers remain available on the [Releases page](https://github.com/SkiltonUSA/ZZDosBox-ARM-68K/releases).

This update adds library categories, compact settings tabs, an **[i] About**
button and scrolling status history. Settings use the full page and retain
scrolling when needed on smaller screens.

[Navigation guide](../../../docs/amiga-dosbox-launcher-navigation.md).
`build.json` records the executable and source hashes; `SHA256SUMS` verifies
the download.

Validated on the A4000TX with 640×480 and 1024×768 window budgets: all twelve
pages, resizing, scrolling and bounded status history passed. No Workbench
screen-mode change was made. [Layout reports](../../../docs/validation/launcher-0.23.0-ui).
