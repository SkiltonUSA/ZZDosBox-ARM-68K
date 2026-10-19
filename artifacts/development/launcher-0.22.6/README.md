# Launcher 0.22.6-dev

Incremental update: close the launcher and player, back up the existing
`ZZDosBox-Launcher`, and replace that executable in the application drawer.
Retain the configuration and support files. Use the current Expanded runtime
for the new high-cycle choices; [Expanded 0.22.5-dev](../expanded-0.22.5) is the
tested companion. The launcher retains the MUI 3.8-or-later requirement.

CPU & memory offers 1,000 / 3,000 / 6,000 / 10,000 / 60,000 / 100,000 cycles.
Use Fixed cycle control to retain the selected budget. Existing profile indices
keep their values; the two new choices automatically select Expanded.
Global dosbox.conf overrides still take precedence. High-cycle choices retain
Expanded's restrictions on networking, renderer and developer configurations.
Older launchers cannot read profiles saved with the new choices.

Higher cycles can increase FPS while slowing emulated time and disrupting sound.
They do not alter the ARM clock. See the
[measured Duke results](../../../docs/amiga-dosbox-duke-arm-optimisation.md).
