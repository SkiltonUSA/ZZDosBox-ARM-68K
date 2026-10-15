# Network development build — 0.17.0-dev

Installed and tested on A4000TX/ZZ9000, XX19c/XACP 1.7 and Roadshow.
This is an incremental development update for an existing ZZDosBox installation.

- `ZZDosBox-Launcher`: MUI launcher with the Network tab.
- `ZZDosBox-Net`: separate interpreter runtime, used only for network-enabled profiles.
- `Compatibility.dat` and Markdown references: the launcher's offline report data.
- `dosbox.elf` and build manifests: symbols and provenance for the network runtime.

Place the launcher, network runtime and compatibility files alongside the
existing application. Preserve a copy of your launcher and settings before
upgrading: profiles migrate to version 12 with networking disabled. Keep the
normal DOS/Windows runtimes for non-network sessions.

Guest utilities are **not bundled here**. Stage the pinned NE2000/mTCP inputs
using `scripts/prepare_dosbox_network_tools.py`, then place its `Network/`
drawer beside the launcher. See the [Network guide](../../../docs/amiga-dosbox-network-ui.md)
for controls, input hashes, requirements and restrictions.

DHCP, manual IPv4, DNS, ping, normal launch and cooperative disconnect passed
bounded physical tests. Windows networking, IPX and sustained throughput are
not validated. This interpreter build does not replace the faster JIT runtime.
