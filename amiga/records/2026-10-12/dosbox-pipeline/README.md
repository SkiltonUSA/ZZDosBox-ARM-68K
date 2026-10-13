# ZZDosBox copy/PAN overlap experiment

Physical A4000TX / ZZ9000 XX19c, 12 October 2026. Same executable with
serial/pipeline switch; unchanged ARM payload. 558 generated/acknowledged
frames and 8,000 emulated ms per game. No screenshots during timing.

Keen: 33.20 -> 33.32 FPS; Vette: 24.16 -> 24.19 FPS. Zero overlapping
copies in either game: no meaningful gain demonstrated. Serial remains
the source default; pipeline is available via `--pipeline-video` for future
work. The experimental binary in build.json defaulted to pipeline; serial
comparison explicitly used `--serial-video`.

Native event model passed 168,000 frames including wrap/ownership/timeout
cases. EGA/PIT and physical game runs passed restoration/release checks.
The physical overlapping-copy branch remains unexercised. ACK counts alone
do not establish tear-free scanout. Timings measure startup, not gameplay.

See the development document for design and interpretation. No game data
or executable payloads are included in these records.

Physical Ctrl-C during Keen startup also passed: 205 frames, 2,955 emulated
ms, timed_done=0, clean restore/release. No overlapping copy occurred.
