/* Original boot-sector fixture: BIOS read, media bounds and write protection. */
.code16
.global _start
_start:
 xor %ax,%ax
 mov %ax,%ds
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
