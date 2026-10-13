# ZZDosBox 0.5.0-dev: PC devices and direct display

Physical A4000TX / ZZ9000 XX19c records, 2026-10-12. The build manifest
identifies the pinned upstream and generated source hashes. Original COM
fixture source is in `amiga/dosbox/pc/egatest.S`; no games are included.

- `native.log`: native ASan COM/EXE and 16-colour EGA/PIT fixture.
- `keen-native.log`, `vette-native.log`: native timed game probes.
- `arm-model.log`: two relocated ARM instruction-model cases, both output
  formats, all 307,200 displayed pixels and revoked temporary mappings.
- `arm-model-error.log`: incorrect framebuffer nonce rejected, mapping revoked.
- `direct-plan.log`: 7,440 ASan/UBSan mapping/geometry cases.
- `build.log`, `build.json`, patches: final ARM/68k build and provenance.
- `fixture.log`, `keen.log`, `vette.log`: final physical runs; PC timing,
  file service, display ACKs, register/cache restoration and memory release.

`PC frames` counts generated frames. `emulated_ms` is PC virtual time;
`wall_ms` is the actual host run loop duration, excluding launcher startup.
It includes game loading, file service and display waits; screenshots can
also perturb timing. These are startup smoke tests, not steady gameplay
benchmarks. Timed game stop reports worker result 2 and launcher success 0.

The ARM image is identical to the initial physically tested image. The final
68k update adds wall time and requires a command for this fullscreen prototype.
Final executable: 692,312 bytes, SHA256
`4fddc62bf3ea5a5790fa8ca48a4c0f8916d292ced5f0fd3171b03b73ca052493`.

See `docs/amiga-dosbox-feasibility.md` for setup, limitations and next steps.
