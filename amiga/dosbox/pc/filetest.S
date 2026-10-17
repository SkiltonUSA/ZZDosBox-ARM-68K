/* GPL-2.0-or-later. Small reads, aliased writes, truncation, append and seek. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 xor %si,%si
fill:
 mov %si,%ax
 mov %al,buffer(%si)
 inc %si
 cmp $256,%si
 jne fill
 mov $name,%dx
 xor %cx,%cx
 mov $0x3c,%ah
 int $0x21
 jc fail
 mov %ax,%bx
 mov $32,%si
write_loop:
 mov $buffer,%dx
 mov $256,%cx
 mov $0x40,%ah
 int $0x21
 jc fail
 cmp $256,%ax
 jne fail
 dec %si
 jnz write_loop
 mov $0x3e,%ah
 int $0x21
 mov $name,%dx
 mov $0x3d00,%ax
 int $0x21
 jc fail
 mov %ax,%bx
 mov %ax,reader
 xor %si,%si
read_loop:
 mov $one,%dx
 mov $1,%cx
 mov $0x3f,%ah
 int $0x21
 jc fail
 cmp $1,%ax
 jne fail
 mov %si,%ax
 cmp one,%al
 jne fail
 inc %si
 cmp $8192,%si
 jne read_loop
 /* Refill a cache at the start, then mutate through another open handle. */
 xor %cx,%cx
 xor %dx,%dx
 mov $0x4200,%ax
 int $0x21
 jc fail
 call read_one
 mov $name,%dx
 mov $0x3d02,%ax
 int $0x21
 jc fail
 mov %ax,%bx
 mov %ax,writer
 xor %cx,%cx
 mov $1,%dx
 mov $0x4200,%ax
 int $0x21
 jc fail
 movb $0xaa,one
 mov $one,%dx
 mov $1,%cx
 mov $0x40,%ah
 int $0x21
 jc fail
 mov reader,%bx
 call read_one
 cmpb $0xaa,one
 jne fail
 /* Writer is at byte two: zero write truncates here. */
 mov writer,%bx
 xor %cx,%cx
 mov $0x40,%ah
 int $0x21
 jc fail
 mov reader,%bx
 mov $one,%dx
 mov $1,%cx
 mov $0x3f,%ah
 int $0x21
 jc fail
 test %ax,%ax
 jnz fail
 mov writer,%bx
 mov $buffer,%dx
 mov $3,%cx
 mov $0x40,%ah
 int $0x21
 jc fail
 mov reader,%bx
 call read_one
 cmpb $0,one
 jne fail
 /* Current position must be logical byte three, not read-ahead endpoint. */
 xor %cx,%cx
 xor %dx,%dx
 mov $0x4201,%ax
 int $0x21
 jc fail
 cmp $3,%ax
 jne fail
 test %dx,%dx
 jnz fail
 mov $0x3e,%ah
 int $0x21
 mov writer,%bx
 mov $0x3e,%ah
 int $0x21
 mov $name,%dx
 mov $0x41,%ah
 int $0x21
 jc fail
 mov $passmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
read_one:
 mov $one,%dx
 mov $1,%cx
 mov $0x3f,%ah
 int $0x21
 jc fail
 cmp $1,%ax
 jne fail
 ret
fail:
 mov $failmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
name: .asciz "READTEST.DAT"
reader: .word 0
writer: .word 0
one: .byte 0
passmsg: .ascii "Small reads alias truncate append seek PASS\r\n$"
failmsg: .ascii "Small reads FAIL\r\n$"
buffer: .space 256
