# Shared ZZDosBox support

- `arm/`: Core1 worker transport, MMU/cache setup, bounded heap and memory routines.
- `libc/`: pinned PIC soft-float picolibc configuration and source checksums.
- `flip_plan.h`: display-slot ownership and acknowledgement planning.

This support is used by ZZDosBox. It contains no NES/SNES emulator core or
FFmpeg decoder. Existing internal worker symbol names are retained so archived
ARM images and instruction-model tests remain compatible. Component credits remain documented; recorded build hashes are unchanged.
See [the ARM execution contract](../../docs/amiga-arm-debugging.md).
