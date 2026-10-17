/* Guest UART settings only: no external serial bytes transmitted. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x3fb,%dx
 mov $0x80,%al
 out %al,%dx
 mov $0x3f8,%dx
 mov $6,%al
 out %al,%dx
 inc %dx
 xor %al,%al
 out %al,%dx
 mov $0x3fb,%dx
 mov $0x1a,%al
 out %al,%dx
 call settle
 mov $0x3fb,%dx
 mov $0x80,%al
 out %al,%dx
 mov $0x3f8,%dx
 mov $12,%al
 out %al,%dx
 inc %dx
 xor %al,%al
 out %al,%dx
 mov $0x3fb,%dx
 mov $3,%al
 out %al,%dx
 call settle
 mov $message,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
settle:
 mov $10,%bx
outer:
 xor %cx,%cx
inner:
 loop inner
 dec %bx
 jnz outer
 ret
message: .ascii "COM1 configuration fixture completed (no TX)\r\n$"
