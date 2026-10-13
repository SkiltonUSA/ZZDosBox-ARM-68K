/* Original INT 13h HDD fixture: geometry, cross-track writes and end bounds. */
.code16
.global _start
_start:
 xor %ax,%ax
 mov %ax,%ds
 mov %ax,%es
 mov $0x80,%dl
 mov $0x0800,%ax
 int $0x13
 jc fail
 cmp $127,%ch
 jne fail
 cmp $32,%cl
 jne fail
 cmp $15,%dh
 jne fail
 movw $0x5a39,0x0800
 movw $0x2718,0x0a00
 mov $0x0800,%bx
 mov $0x0080,%dx
 mov $32,%cx
 mov $0x0302,%ax
 int $0x13
 jc fail
 movw $0,0x0800
 movw $0,0x0a00
 mov $0x0202,%ax
 int $0x13
 jc fail
 cmpw $0x5a39,0x0800
 jne fail
 cmpw $0x2718,0x0a00
 jne fail
 mov $0x0f80,%dx
 mov $0x7f20,%cx
 mov $0x0301,%ax
 int $0x13
 jc fail
 mov $0x0201,%ax
 int $0x13
 jc fail
 mov $0x8001,%cx
 mov $0x0301,%ax
 int $0x13
 jnc fail
 cmp $4,%ah
 jne fail
 mov $0x0201,%ax
 int $0x13
 jnc fail
 movw $0xd007,0x0500
 jmp idle
fail:
 movw $0xfa11,0x0500
idle:
 sti
 hlt
 jmp idle
.org 510
.word 0xaa55
