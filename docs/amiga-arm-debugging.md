# ZZDosBox ARM execution contract

ZZDosBox uses Xanai's XACP application-owned Core1 model. Shared support for
that model lives in `amiga/common/arm/`; the 68K launch and return path lives
in `amiga/arm_debug/zz9000/`.

- Reserve the worker's 32 MiB block through AmigaOS Exec in ZZ9000 RAM.
- Verify the runtime ARM mapping with the launcher's mapping probe; never
  replace it with a remembered DDR address.
- Keep code, stack, page tables and shared buffers inside the owned allocation.
  The DOSBox build adapts the common MMU template to its allocation layout.
- Use the defined uncached control/mailbox regions and the existing cache
  synchronization and display-acknowledgement protocol.
- Preserve the firmware's register/MMU/cache state, stop cooperatively and
  reset Core1 idle before releasing the allocation.
- Keep one application owner of Core1. Close other XACP applications first.

The support code includes no console emulator core or video decoder.
Historical worker symbol names remain compatible with archived ARM images.
Native tests and instruction models do not establish physical cache coherence
or successful hardware teardown. See [validation and requirements](requirements.md)
and the versioned DOSBox records for the checks completed on hardware.

The separate [Amiga-MCP-Debugger](https://github.com/SkiltonUSA/Amiga-MCP-Debugger)
repository documents the debugging framework. XACP interfaces and firmware
belong to [Xanai's XACP-ZZ9000](https://github.com/Xanxi-Amiga/XACP-ZZ9000).
