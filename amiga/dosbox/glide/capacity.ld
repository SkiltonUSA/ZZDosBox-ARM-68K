ENTRY(_start)
PHDRS { image PT_LOAD FLAGS(7); dynamic PT_DYNAMIC FLAGS(6); }
SECTIONS {
  . = 0;
  .entry : { KEEP(*(.text.entry)) } :image
  /* The Exec allocation is 64-byte aligned. Keep code off its partial page. */
  . = ALIGN(4096);
  .text : { *(.text*) } :image
  .rodata : { *(.rodata*) } :image
  .data : { *(.data*) *(.got*) } :image
  .dynamic : { *(.dynamic) } :image :dynamic
  .dynsym : { *(.dynsym) } :image
  .dynstr : { *(.dynstr) } :image
  .hash : { *(.hash) } :image
  .gnu.hash : { *(.gnu.hash) } :image
  .rel.dyn : { *(.rel.dyn) } :image
  .bss : { *(.bss*) *(COMMON) } :image
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment) }
  ASSERT(. < 0x40000, "Encoder benchmark overlaps private page tables")
}
