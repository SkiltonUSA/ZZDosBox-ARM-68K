/* GPL-2.0-or-later. Exercise >8 MiB XMS, VCPI allocation and UMB services. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x4310,%ax
 int $0x2f
 mov %bx,xms
 mov %es,xms+2
 movb $49,stage
 mov $20480,%dx
 mov $9,%ah
 lcall *xms
 cmp $1,%ax
 jne fail
 mov %dx,handle
 mov %dx,move_to+10
 mov %dx,move_from+4
 /* Conventional source/destination addresses for XMS move. */
 mov %ds,%ax
 mov %ax,move_to+8
 mov %ax,move_from+14
 movb $50,stage
 mov $move_to,%si
 mov $0xb,%ah
 lcall *xms
 cmp $1,%ax
 jne fail
 movb $51,stage
 mov $move_from,%si
 mov $0xb,%ah
 lcall *xms
 cmp $1,%ax
 jne fail
 movb $52,stage
 cmpl $0x5aa51234,destination
 jne fail
 movb $53,stage
 mov handle,%dx
 mov $0xa,%ah
 lcall *xms
 cmp $1,%ax
 jne fail
 /* Allocate and release an upper-memory block through XMS. */
 movb $54,stage
 mov $16,%dx
 mov $0x10,%ah
 lcall *xms
 cmp $1,%ax
 jne fail
 mov %bx,%es
 movw $0x7654,%es:0
 cmpw $0x7654,%es:0
 jne fail
 movb $55,stage
 mov %bx,%dx
 mov $0x11,%ah
 lcall *xms
 cmp $1,%ax
 jne fail
 /* VCPI installation / free pages / allocate / free. */
 movb $56,stage
 xor %cx,%cx
 mov $0x12,%di
 mov $0xde00,%ax
 int $0x67
 test %ah,%ah
 jnz fail
 cmp $0x100,%bx
 jb fail
 movb $57,stage
 mov $0xde03,%ax
 int $0x67
 test %ah,%ah
 jnz fail
 mov %edx,free_pages
 movb $58,stage
 mov $0xde04,%ax
 int $0x67
 test %ah,%ah
 jnz fail
 mov %edx,vcpi_page
 mov $0xde03,%ax
 int $0x67
 test %ah,%ah
 jnz fail
 inc %edx
 cmp free_pages,%edx
 jne fail
 movb $59,stage
 mov vcpi_page,%edx
 mov $0xde05,%ax
 int $0x67
 test %ah,%ah
 jnz fail
 mov $0xde03,%ax
 int $0x67
 cmp free_pages,%edx
 jne fail
 mov $good,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
fail:
 mov stage,%dl
 mov $2,%ah
 int $0x21
 mov $bad,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
stage: .byte 48
xms: .word 0,0
handle: .word 0
free_pages: .long 0
vcpi_page: .long 0
source: .long 0x5aa51234
destination: .long 0
/* length, source handle, source offset, destination handle, destination offset */
move_to: .long 4
 .word 0
 .word source,0
 .word 0
 .long 0x1000000
move_from: .long 4
 .word 0
 .long 0x1000000
 .word 0
 .word destination,0
good: .ascii "Extended RAM VCPI UMB PASS\r\n$"
bad: .ascii "Extended RAM VCPI UMB FAIL\r\n$"
