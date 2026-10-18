# Launcher 0.22.4-dev

Adds a saved 10,000-cycle CPU choice and retains the live ZZ9000 temperature
beside the version. Existing 1,000/3,000/6,000 choices retain their values.
Global dosbox.conf overrides still take precedence.

Close the launcher and replace ZZDosBox-Launcher in an existing installation,
keeping the old executable for rollback. Keep the current library and support
files. This launcher requires the existing runtime package; it is not a complete
installation. No runtime executable is replaced by this update.

Cross-built with warnings as errors; profile/session round-trip checks pass.
Installed executable and library verified on the A4000TX. Descent tuning and
its limits are documented in the [performance report](../../../docs/amiga-dosbox-descent-performance.md).

An older launcher cannot read a library containing the new 10,000-cycle choice.
To roll back, select 6,000 or less and save first, or restore the matching library
backup along with the old launcher.
