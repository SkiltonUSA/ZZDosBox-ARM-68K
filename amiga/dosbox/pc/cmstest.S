/* GPL-2.0-or-later. Guest I/O sound generation, checked at PCM output. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x221,%dx
 mov $28,%al
 out %al,%dx
 dec %dx
 mov $2,%al
 out %al,%dx
 mov $0x221,%dx
 mov $0,%al
 out %al,%dx
 dec %dx
 mov $255,%al
 out %al,%dx
 mov $0x221,%dx
 mov $8,%al
 out %al,%dx
 dec %dx
 mov $128,%al
 out %al,%dx
 mov $0x221,%dx
 mov $16,%al
 out %al,%dx
 dec %dx
 mov $68,%al
 out %al,%dx
 mov $0x221,%dx
 mov $20,%al
 out %al,%dx
 dec %dx
 mov $1,%al
 out %al,%dx
 mov $0x221,%dx
 mov $21,%al
 out %al,%dx
 dec %dx
 mov $0,%al
 out %al,%dx
 mov $0x221,%dx
 mov $28,%al
 out %al,%dx
 dec %dx
 mov $1,%al
 out %al,%dx

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
