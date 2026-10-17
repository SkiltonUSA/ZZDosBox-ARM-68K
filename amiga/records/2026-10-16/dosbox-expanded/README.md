# Expanded hardware validation — 0.21.0-dev

The optional Expanded runtime was built and installed on the A4000TX with
XX19c/XACP, alongside the existing Compat runtime. It owns a 64 MiB allocation
and supports up to 32 MiB of emulated PC RAM. No firmware change was required.

## Build identification

| Component | SHA256 |
|---|---|
| Expanded runtime | `d40665496d5888d1981dc46dcb445a6592ea05df28ea4820e80bed3c44e7a145` |
| MUI launcher | `66c8ba89d46e59c892c85c4860ef8e3ec6eef9a79fdb87246697f78a2ecf3165` |

The linked ARM image occupies 957,412 bytes. Build flags retain the 4 MiB JIT
cache, NEON conversion, pipelined presentation, scanlines, graphics adapters,
PIC/VFP and Compat/x87, adding `--pc-expanded`. Allocation mapping checks
covered 16,384 possible alignment offsets.

## Results

- Native sanitizer fixtures passed CPU identification, extended RAM, VCPI/UMB,
  gameport, extra audio and UART tests. TCP ring wrap, modem commands and
  disconnect tests passed.
- Linked ARM fixtures passed relocation, device behavior, SIMD/FPSCR restore
  and denied-SIMD handling. Paged JIT passed ten alias/SMC, fetch, fault and
  privilege cases across two addresses. Default interpreter paging regressions
  also passed.
- Fifteen physical runs passed: 386, 486, Pentium, extended memory/VCPI/UMB,
  joystick library lifecycle, GUS, CMS, Tandy, UART/TCP, Auto, Max, x87, EMS,
  SB16 and paged-register JIT. Each restored registers and cache witnesses and
  released its allocation. [Machine-readable receipts](physical-results.json)
  include hashes of the retained private logs.
- Each extra sound device completed 18,432 AHI frames with error 0; SB16
  completed 2,048 frames with error 0.
- Physical paged JIT executed 27 JIT dispatches and 30 interpreter fallbacks,
  passed the alias/remapping/self-modification fixture and returned to real
  mode before clean shutdown.
- Auto and Max both adjusted from 6,000 to 2,008 cycles in an EGA fixture that
  lagged wall time. This proves feedback operation, not a game speed increase.

Paged JIT remains an opt-in register-only subset. Physical joystick movement,
audible sound quality, physical modem command behavior and broader VCPI client
compatibility remain to be checked. CPU model selection does not establish
equivalence to a physical 486 or Pentium. No new game FPS gain is claimed.

See the [capability guide](../../../../docs/amiga-dosbox-expanded-hardware.md)
for settings and limitations.
