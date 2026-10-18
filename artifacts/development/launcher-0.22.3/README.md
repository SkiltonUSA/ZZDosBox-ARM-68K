# Launcher 0.22.3-dev — ZZ9000 temperature

Copy `ZZDosBox-Launcher` into an existing installation after closing the old
launcher. Keep the previous executable for rollback and retain the existing
library/configuration and support files.

The footer shows the ZZ9000 Zynq temperature next to the version. A read-only
register sample updates every two seconds while the launcher is active;
unavailable or invalid readings show `n/a`. Normal gameplay closes the launcher
window and suspends polling until it returns. This does not measure the 68K CPU.

Uses the same register and conversion as ZZTemperature and the NES/SNES
launchers: expansion manufacturer 0x6d6e, product 4/3, offset 0xe0, tenths Celsius.
No XACP command, Core1 job or register write is involved.

Built with warnings as errors. Physically verified on the A4000TX: footer
58.1 C matched the existing monitor; close/reopen passed and Launcher.cfg
remained byte-identical. Screenshot retained in the local verification record.
This updates only the launcher; runtime and game settings are unchanged.
