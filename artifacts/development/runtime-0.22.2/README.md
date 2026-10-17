# ZZDosBox 0.22.2-dev update

Incremental update for an existing ZZDosBox installation. Close the player and
launcher, then copy the four executables and `Network.guide` into the application
drawer. Retain the existing configuration, icons, media and support files.

| File | Purpose |
| --- | --- |
| ZZDosBox-Launcher | MUI launcher, COM1 settings, joystick tester and current changelog |
| ZZDosBox-Compat | ARM JIT, EMS/XMS, Sound Blaster, experimental x87 and file read-ahead |
| ZZDosBox-Expanded | Additional CPU/memory/device choices, COM1 and joystick |
| ZZDosBox-Net-JIT | NE2000/IPX networking with JIT, EMS and Sound Blaster |
| Network.guide | Offline networking help |

The launcher selects the appropriate runtime. Existing interpreter-network,
Windows and developer runtimes remain separate. Physical COM1 is experimental;
COM1/joystick and network modes cannot currently be combined. Paging JIT remains
off by default. See [requirements](../../../docs/requirements.md) and
[I/O specifications](../../../docs/amiga-dosbox-io.md).

Compat and Net-JIT use a 32 MiB owned ZZ9000 allocation; Expanded uses 64 MiB.
The TF4060 identifies the test machine, not a required accelerator model.

The original small-read/coherence fixture improved from 18,671 ms to 336 ms on
SD storage. This is a diagnostic latency result, not a game-FPS multiplier.
[Measurements and validation](../../../docs/amiga-dosbox-file-read-optimisation.md).

`builds/` retains the exact ARM ELF, build metadata and integration patches.
`sha256.json` records executable/build hashes. Source, tests, dependency pins and
licence notices are in the repository. This update contains no games or guest OS
media. It is a development backup, not a new tagged release.
