# XACP 1.8 regression and fullscreen foreground handling

Tests ran on XX19d-RIX/XACP 1.8 with the installed 4 MiB JIT-cache players.
No firmware, global AHI configuration or user game profile was changed.

## Passing checks

- Physical x87 arithmetic, rounding and state fixture.
- Physical EMS/XMS mapping and shared allocation fixture.
- Physical SB16 DMA5/IRQ7 fixture: AHI delivery completed with no device error.
- Physical expanded-player paging JIT aliases and self-modifying-code fixture.
- Native console synchronization, oversized network packets, presentation
  ownership, audio batching, serial lifecycle, read-ahead coherence and launcher
  layout suites. These are not live modem/network throughput measurements.
- Windowed Doom: 20,000 emulated milliseconds, 1,399 VGA updates and all
  640,000 queued PCM frames delivered; normal register/MMU/cache restoration
  and memory release. VGA updates are not Doom gameplay FPS.

## Fullscreen finding

Two release-player fullscreen attempts failed safely. A diagnostic host using
that same archived ARM payload reproduced one cause: `full_front()` was false
at the next display grant, and the host treated it as a fatal transport error.
Its log records `VIDEO_FAIL line=110` and `HOST_SERVICE_FAIL display`.

The source now waits before granting another frame when the custom screen is
not foremost. It continues the host service loop, including cancellation; it
does not grant a new framebuffer copy while the screen is behind Workbench.
The existing locked-copy/PAN protocol, stop deadline and
allocation/nonce/teardown checks remain. `DISPLAY background_waits` records
how many host service visits waited for the foreground.

The first private patched-host run completed 20,000 emulated milliseconds,
1,400 VGA updates and all 640,000 PCM frames. Its register/MMU/cache witnesses
and memory release passed. This build retains the release ARM payload and
uses current host support sources; it is not a matched performance comparison.

The second normal patched run also passed (1,399 VGA updates), but recorded
`background_waits=0`; it did not exercise the changed branch. A separate private
fault-injection build forces `full_front()` false on five service visits after
100 completed frames. It then uses the real foreground predicate again. This
6,000-emulated-millisecond run passed with 419 updates, exactly five waits,
191,488 queued/completed PCM frames, and clean restoration/release. This tests
wait/resume through the physical host service loop, not an actual Intuition
screen switch. The injection exists only in the private test build.

`foreground-fault-build.json` records the payload and executable identities.
`build-host-diagnostic.py` records the private build procedure; it needs the
archived release artifacts in the development workspace. `DOS_FRONT_FAULT=1`
enables injection; an ordinary build does not include it. The host-review suite
also passed again after the source change (console spans, 168,000 scheduled
frames, 200,000 slot handoffs and oversized network-packet handling).

A different fullscreen failure reported `Framebuffer handoff error 7` (nonce
mismatch). The foreground fix does not establish that this second failure is
resolved. No challenge was bypassed. Device gaps also remain: a completed
queue is not proof of uninterrupted or perceptually clean audio.

Installed players, defaults and release version remain unchanged. Game data
are private inputs, not included here. The first script incorrectly labeled a
fullscreen attempt as windowed; its retained log is named
`dos-doom-full-playdemo.log`. The successful windowed log explicitly reports
`DISPLAY_CONFIG mode=window`.

Screen ordering follows the [AmigaOS ScreenToFront interface](https://developer.amigaos3.net/autodocs/intuition.library/ScreenToFront.html).
The diagnostic evidence establishes loss of foreground; it does not identify
which application or Intuition operation changed the screen order.

## Follow-up: actual screen switching and fault rejection

[Display handoff follow-up](handoff-followup/README.md) records actual Intuition
screen-depth changes, six window/fullscreen transitions, deliberate bad-marker
rejection and normal cancellation while behind Workbench. The original
intermittent nonce failure is still unresolved. Installed releases are unchanged.
