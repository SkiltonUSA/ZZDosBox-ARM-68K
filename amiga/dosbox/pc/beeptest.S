/* GPL-2.0-or-later. PIT channel 2 + port 61 PC speaker tone. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 in $0x61,%al
 mov %al,original
 mov $0xb6,%al
 out %al,$0x43
 mov $1193,%ax
 out %al,$0x42
 mov %ah,%al
 out %al,$0x42
 in $0x61,%al
 or $3,%al
 out %al,$0x61
 xor %ah,%ah
 int $0x1a
 mov %dx,%bx
wait:
 xor %ah,%ah
 int $0x1a
 sub %bx,%dx
 cmp $10,%dx
 jb wait
 mov original,%al
 out %al,$0x61
 mov $message,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
original: .byte 0
message: .ascii "PC speaker tone complete\r\n$"
