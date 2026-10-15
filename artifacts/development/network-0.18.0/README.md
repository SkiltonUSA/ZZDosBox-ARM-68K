# TCP/IP and IPX development runtime

`ZZDosBox-Net` is the interpreter runtime used for the physical TCP/IP/IPX
transport tests. Use it with the current launcher. Networking is optional and
disabled by default; Roadshow and bsdsocket.library supply host connectivity.
TCP/IP needs separately staged NE2000/mTCP tools; IPX needs a compatible UDP relay.
Multiplayer game compatibility is untested.

The milestone folder refers to launcher 0.18.0; the embedded runtime version
retains its earlier `0.10.1-dev-network-test` label. This tested binary predates
the later oversized-receive queue cleanup now present in source.

See [network settings](../../../docs/amiga-dosbox-network-ui.md),
[physical evidence](../../../amiga/records/2026-10-15/dosbox-ipx/) and
`runtime-build.json` for provenance.
