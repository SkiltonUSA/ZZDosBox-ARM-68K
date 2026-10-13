# ZZDosBox interpreter compiler optimisation

Physical A4000TX / ZZ9000 XX19c, 12 October 2026. Only the normal x86
interpreter compilation changes from -Os to -O2. Other ARM sources retain
-Os. Serial display path, 1 ms host waits, 558 frames / 8,000 emulated ms
per game, no screenshots. No dynamic recompilation or frame skipping.

| Startup workload | Previous | New | Gain |
| --- | ---: | ---: | ---: |
| Keen 6 | 33.20 FPS | 35.75 FPS | 7.7% |
| Vette | 24.16 FPS | 25.18 FPS | 4.2% |

Baseline logs/build provenance are copied from the preceding pipeline
experiment's serial runs. Both new game runs pass restoration/release and
complete all 558 display acknowledgements, with zero file writes. These
are single-run startup measurements; gameplay controls/audio are pending.

ARM-model log covers both relocation bases/formats, exact pixels, arithmetic,
DOS file/MZ fixtures, bad nonce and mapping revocation. EGA/PIT and input-wait
cancellation also pass physically. No game binaries are included here.

Final default build is byte-identical to the explicit -O2 candidate.
Executable SHA256:
`927b5dd8738de7e0b44e6a08dc289a215dc7741fd2a673e0cecca4ec8dfb6379`.

Active-game Ctrl-C passed (216 frames, timed_done=0). Installed as the normal
prototype executable, preserving the scaler build as ZZDosBox-PreCPU.
Promotion events record this operation. No GitHub publication performed.

Installed-path EGA/PIT passed: 18 frames/18 ACKs. Final Workbench and bridge
are available, with Core1 ownership released.
