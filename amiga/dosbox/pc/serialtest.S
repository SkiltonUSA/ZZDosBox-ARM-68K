/* GPL-2.0-or-later. 16550 divisor, scratch, loopback and transported bytes. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x40,%ax
 mov %ax,%es
 cmpw $0x3f8,%es:0
 jne fail
 mov $0x00e3,%ax
 xor %dx,%dx
 int $0x14
 mov $0x3ff,%dx
 mov $0xa5,%al
 out %al,%dx
 in %dx,%al
 cmp $0xa5,%al
 jne fail
 mov $0x3fc,%dx
 mov $0x1f,%al
 out %al,%dx
 mov $0x3f8,%dx
 mov $0x5a,%al
 out %al,%dx
 call receive
 cmp $0x5a,%al
 jne fail
 mov $0x3fc,%dx
 mov $0x0b,%al
 out %al,%dx
 mov $pattern,%si
 mov $16,%cx
next:
 push %cx
 mov $0x3fd,%dx
1: in %dx,%al
 test $0x20,%al
 jz 1b
 lodsb
 mov %al,%bl
 mov $0x3f8,%dx
 out %al,%dx
 call receive
 cmp %bl,%al
 jne fail
 pop %cx
 loop next
 mov $good,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
receive:
 mov $0x3fd,%dx
2: in %dx,%al
 test $1,%al
 jz 2b
 mov $0x3f8,%dx
 in %dx,%al
 ret
fail:
 mov $bad,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
pattern: .ascii "UART bridge test"
good: .ascii "COM1 UART PASS\r\n$"
bad: .ascii "COM1 UART FAIL\r\n$"
