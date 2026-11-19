# What is required for a hardware FPS result

There is no accelerated GTA FPS measurement yet. The diagnostic overlay reports
no board, and no GTA triangle has reached RasterIX. The 79 passing component
scenarios establish particular protocol behaviours, not a complete renderer or
game performance.

## Remaining prerequisites

1. **Complete GPU integration and a build environment.** The proposed fence,
   identity receiver, error observer and completion registers need integration
   with the complete GPU and board design. The published design uses Vivado
   2025.2, which is not available in the current development environment.
   AMD lists supported [x86-64 Windows/Linux operating systems](https://docs.amd.com/r/2025.2-English/ug973-vivado-release-notes-install-license/Supported-Operating-Systems).
   An ARM Mac VM is not that supported build environment. The pinned project
   must pass block-design validation, synthesis, implementation and timing
   closure before any new image can be considered for installation.
2. **Verified firmware and hardware completion.** The existing submit validator
   rejects the proposed fence/echo sequence. A capability-aware interface,
   matching FPGA image and bounded physical retirement/error/cleanup tests are
   needed. Previous readback discrepancies and freezes remain unresolved.
   Successful TCP connectivity does not establish GPU correctness.
3. **Guest renderer integration.** The overlay's 146 exports are diagnostic
   stubs, not 146 implemented Glide functions. Drawing dispatch, texture
   allocation/upload, guest state, required depth/LFB behaviour and presentation
   still need implementation and validation. Changing the hardware query to
   return true would not make the game renderer work.

## Latest environment check

The local bridge services were restored and the Amiga answered ping and public
port queries. No emulator held the Core1 ownership port. The reservation was
released without launching a game or GPU probe. No clock, firmware or installed
application was changed.

The container VM reported a successful start but then reported stopped, with its
build connection refusing connections. No Vivado executable or installation was
found in the checked local locations. An existing supported build machine has
been requested; none has been identified in this session.

The full pinned RasterIX and Float sources were downloaded for the next simulator
stage. An exploratory Icarus elaboration of the complete RasterIX top failed
on unused constant output destinations and declarations referenced before their
definitions. That run used a preliminary one-TMU/64-bit configuration, not all
board parameters. It is neither a complete simulation result nor evidence that
Vivado rejects the design. No source workaround or hardware image was promoted.

## Measurement criteria

Once a real guest renderer passes the prerequisites, compare the same GTA build,
scene, resolution, CPU-cycle setting, audio configuration and ARM clock. Record
the identities of the player, overlay, firmware and FPGA image.

Measure completed game frame presentations over an explicitly timed interval,
after warm-up. Report actual frame count and elapsed time alongside FPS, and
repeat the same run. Count frames at the guest render/present boundary: VGA
refreshes, menu refreshes and repeated framebuffer scans are not gameplay frames.

Keep CPU submission time, GPU completion time and total frame time separate.
Do not convert native-Mac runtime, RTL simulation speed, a 100 MHz nominal clock
or the number of passing tests into an A9 gameplay FPS claim. Any improvement
needs a matched baseline and clean shutdown; isolated renderer throughput must
be labelled separately from game FPS.
