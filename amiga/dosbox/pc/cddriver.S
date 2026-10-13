/* GPL-2.0-or-later. Original DOS character driver for the ZZDosBox gateway. */
.code16
.global _start
_start:
 .long 0xffffffff
 .word 0xc800
 .word strategy
 .word interrupt
 .ascii "ZZDOSCD "
 .word 0
 .byte 0,1
request: .long 0
strategy:
 mov %bx,%cs:request
 mov %es,%cs:request+2
 lret
interrupt:
 push %ax
 push %bx
 push %cx
 push %dx
 push %ds
 push %es
 push %si
 push %di
 push %bp
 les %cs:request,%bx
 cmpb $0,%es:2(%bx)
 jne call_host
 movw $resident_end,%es:14(%bx)
 mov %cs,%ax
 mov %ax,%es:16(%bx)
call_host:
 mov %cs,%cx
 mov $0x5a43,%ax
 int $0x60
 pop %bp
 pop %di
 pop %si
 pop %es
 pop %ds
 pop %dx
 pop %cx
 pop %bx
 pop %ax
 lret
resident_end:
