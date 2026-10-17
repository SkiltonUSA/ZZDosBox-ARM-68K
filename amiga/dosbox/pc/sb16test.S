/* GPL-2.0-or-later. Actual SB16 signed PCM on DMA5 with IRQ7 acknowledgement. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x226,%dx
 mov $1,%al
 out %al,%dx
 xor %al,%al
 out %al,%dx
 mov $0xffff,%cx
reset_wait:
 mov $0x22e,%dx
 in %dx,%al
 test $0x80,%al
 jnz reset_read
 loop reset_wait
 jmp fail
reset_read:
 mov $0x22a,%dx
 in %dx,%al
 cmp $0xaa,%al
 jne fail
 mov $0x350f,%ax
 int $0x21
 mov %bx,old_irq
 mov %es,old_irq+2
 mov $irq,%dx
 mov $0x250f,%ax
 int $0x21
 in $0x21,%al
 mov %al,old_mask
 and $0x7f,%al
 out %al,$0x21
 mov $5,%al
 mov $0xd4,%dx
 out %al,%dx
 xor %al,%al
 mov $0xd8,%dx
 out %al,%dx
 xor %eax,%eax
 mov %cs,%ax
 shl $4,%eax
 add $samples,%eax
 mov %eax,%ebx
 shr $1,%eax
 mov $0xc4,%dx
 out %al,%dx
 mov %ah,%al
 out %al,%dx
 mov %ebx,%eax
 shr $16,%eax
 out %al,$0x8b
 mov $0xc6,%dx
 mov $0xff,%al
 out %al,%dx
 mov $3,%al
 out %al,%dx
 mov $0x49,%al
 mov $0xd6,%dx
 out %al,%dx
 mov $1,%al
 mov $0xd4,%dx
 out %al,%dx
 mov $0xd1,%al
 call dsp
 mov $0x41,%al
 call dsp
 mov $0x56,%al
 call dsp
 mov $0x22,%al
 call dsp
 mov $0xb0,%al
 call dsp
 mov $0x10,%al
 call dsp
 mov $0xff,%al
 call dsp
 mov $3,%al
 call dsp
 mov $0,%ah
 int $0x1a
 mov %dx,%bx
wait:
 cmpb $0,irq_count
 jne success
 mov $0,%ah
 int $0x1a
 sub %bx,%dx
 cmp $36,%dx
 jb wait
 call restore
 jmp fail
success:
 call restore
 mov $passmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
restore:
 mov old_mask,%al
 out %al,$0x21
 push %ds
 lds old_irq,%dx
 mov $0x250f,%ax
 int $0x21
 pop %ds
 ret
dsp:
 push %ax
 mov $0x22c,%dx
 mov $0xffff,%cx
ready:
 in %dx,%al
 test $0x80,%al
 jz send
 loop ready
 pop %ax
 jmp fail
send:
 pop %ax
 out %al,%dx
 ret
irq:
 push %ax
 push %dx
 incb %cs:irq_count
 mov $0x22f,%dx
 in %dx,%al
 mov $0x20,%al
 out %al,$0x20
 pop %dx
 pop %ax
 iret
fail:
 mov $failmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
old_irq: .word 0,0
old_mask: .byte 0
irq_count: .byte 0
passmsg: .ascii "SB16 DMA5 IRQ7 PASS\r\n$"
failmsg: .ascii "SB16 FAIL\r\n$"
.balign 2
samples:
.rept 16
.fill 32,2,12000
.fill 32,2,-12000
.endr
