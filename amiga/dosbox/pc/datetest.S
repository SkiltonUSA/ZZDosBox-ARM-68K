/* GPL-2.0-or-later. INT21 open timestamp must match FindFirst metadata. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $name,%dx
 mov $0x3d00,%ax
 int $0x21
 jc fail
 mov %ax,%bx
 mov $0x5700,%ax
 int $0x21
 jc fail
 test %dx,%dx
 jz fail
 mov %dx,file_date
 mov %cx,file_time
 mov $0x3e,%ah
 int $0x21
 mov $dta,%dx
 mov $0x1a,%ah
 int $0x21
 mov $name,%dx
 xor %cx,%cx
 mov $0x4e,%ah
 int $0x21
 jc fail
 mov dta+22,%ax
 cmp file_time,%ax
 jne fail
 mov dta+24,%ax
 cmp file_date,%ax
 jne fail
 mov $passmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
fail:
 mov $failmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
name: .asciz "DATETEST.COM"
file_date: .word 0
file_time: .word 0
passmsg: .ascii "File date open/find match PASS\r\n$"
failmsg: .ascii "File date FAIL\r\n$"
dta: .space 64
