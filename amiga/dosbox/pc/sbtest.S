/* GPL-2.0-or-later. Actual SB DSP detection, DMA1 and IRQ7 guest test. */
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
 out %al,$0x0a
 xor %al,%al
 out %al,$0x0c
 xor %eax,%eax
 mov %cs,%ax
 shl $4,%eax
 add $samples,%eax
 out %al,$2
 mov %ah,%al
 out %al,$2
 shr $16,%eax
 out %al,$0x83
 mov $0xff,%al
 out %al,$3
 mov $7,%al
 out %al,$3
 mov $0x49,%al
 out %al,$0x0b
 mov $1,%al
 out %al,$0x0a
 mov $0xd1,%al
 call dsp
 mov $0x40,%al
 call dsp
 mov $211,%al
 call dsp
 mov $0x14,%al
 call dsp
 mov $0xff,%al
 call dsp
 mov $7,%al
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
 mov $0x22e,%dx
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
passmsg: .ascii "SBPro2 DMA1 IRQ7 PASS\r\n$"
failmsg: .ascii "SBPro2 FAIL\r\n$"
samples:
.rept 32
.fill 32,1,32
.fill 32,1,224
.endr
