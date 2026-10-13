/* Original boot fixture: A20 aliasing, ROM protection and BIOS floppy I/O. */
.code16
.global _start
_start:
 xor %ax,%ax
 mov %ax,%ds
 mov %ax,%es
 /* Fast A20 must agree with actual address translation, including reset. */
 in $0x92,%al
 test $2,%al
 jnz fail
 movw $0x1357,0x0900
 mov $0xffff,%ax
 mov %ax,%es
 cmpw $0x1357,%es:0x0910
 jne fail
 mov $2,%al
 out %al,$0x92
 in $0x92,%al
 test $2,%al
 jz fail
 movw $0x2468,%es:0x0910
 cmpw $0x1357,0x0900
 jne fail
 xor %al,%al
 out %al,$0x92
 cmpw $0x1357,%es:0x0910
 jne fail
 mov $2,%al
 out %al,$0x92
 cmpw $0x2468,%es:0x0910
 jne fail
 xor %al,%al
 out %al,$0x92
 /* Writes must not damage the BIOS or VGA ROM. */
 mov $0xf000,%ax
 mov %ax,%es
 mov %es:0xfff0,%bx
 mov %bx,%ax
 not %ax
 mov %ax,%es:0xfff0
 cmp %bx,%es:0xfff0
 jne fail
 mov $0xc000,%ax
 mov %ax,%es
 mov %es:0,%bx
 mov %bx,%ax
 not %ax
 mov %ax,%es:0
 cmp %bx,%es:0
 jne fail
 xor %ax,%ax
 mov %ax,%es
 mov $0x0600,%bx
 mov $0x0201,%ax
 mov $0x0002,%cx
 xor %dx,%dx
 int $0x13
 jc fail
 cmpw $0x5a39,0x0600
 jne fail
 mov $0x0301,%ax
 int $0x13
 jnc fail
 cmp $3,%ah
 jne fail
 mov $0x0201,%ax
 mov $0x5001,%cx
 int $0x13
 jnc fail
 movw $0xb007,0x0500
 jmp idle
fail:
 movw $0xfa11,0x0500
idle:
 sti
 hlt
 jmp idle
.org 510
.word 0xaa55
