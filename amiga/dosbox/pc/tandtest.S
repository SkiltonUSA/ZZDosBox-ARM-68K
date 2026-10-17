/* GPL-2.0-or-later. Guest I/O sound generation, checked at PCM output. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x80,%al
 out %al,$0xc0
 mov $0x10,%al
 out %al,$0xc0
 mov $0x90,%al
 out %al,$0xc0

 xor %ah,%ah
 int $0x1a
 mov %dx,%bx
wait:
 xor %ah,%ah
 int $0x1a
 sub %bx,%dx
 cmp $10,%dx
 jb wait
 mov $message,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
message: .ascii "Extra sound PASS\r\n$"
