/* GPL-2.0-or-later. PC gameport BIOS axes, buttons and timed discharge. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 /* Allow an emulated tick to import the host state. */
 xor %ah,%ah
 int $0x1a
 mov %dx,%bx
1: xor %ah,%ah
 int $0x1a
 cmp %bx,%dx
 je 1b
 mov $0x8401,%ax
 mov $1,%dx
 int $0x15
 jc fail
 cmp $255,%ax
 jne fail
 cmp $1,%bx
 jne fail
 mov $0x8400,%ax
 xor %dx,%dx
 int $0x15
 jc fail
 test $0x10,%al
 jnz fail
 test $0x20,%al
 jz fail
 mov $0x201,%dx
 xor %al,%al
 out %al,%dx
 in %dx,%al
 test $3,%al
 jz fail
 /* Both axes must discharge within one BIOS tick. */
 xor %ah,%ah
 int $0x1a
 mov %dx,%bx
2: xor %ah,%ah
 int $0x1a
 cmp %bx,%dx
 je 2b
 mov $0x201,%dx
 in %dx,%al
 test $3,%al
 jnz fail
 mov $good,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
fail:
 mov $bad,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
good: .ascii "Gameport PASS\r\n$"
bad: .ascii "Gameport FAIL\r\n$"
