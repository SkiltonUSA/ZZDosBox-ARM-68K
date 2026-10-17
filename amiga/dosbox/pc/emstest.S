/* GPL-2.0-or-later. Guest-level EMS remapping, execution and shared XMS pool. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x4300,%ax
 int $0x2f
 cmp $0x80,%al
 jne fail
 mov $0x4310,%ax
 int $0x2f
 mov %bx,xms_entry
 mov %es,xms_entry+2
 mov $0x41,%ah
 int $0x67
 test %ah,%ah
 jnz fail
 mov %bx,%es
 mov %bx,ems_entry+2
 mov $0x42,%ah
 int $0x67
 test %ah,%ah
 jnz fail
 mov %bx,free_pages
 mov $64,%dx
 mov $9,%ah
 lcall *xms_entry
 cmp $1,%ax
 jne fail
 mov %dx,xms_handle
 mov $0x42,%ah
 int $0x67
 add $4,%bx
 cmp free_pages,%bx
 jne fail
 mov $2,%bx
 mov $0x43,%ah
 int $0x67
 test %ah,%ah
 jnz fail
 mov %dx,ems_handle
 xor %bx,%bx
 call map
 movw $0x1111,%es:0
 movb $0xb8,%es:0x20
 movw $0x1111,%es:0x21
 movb $0xcb,%es:0x23
 mov $1,%bx
 call map
 movw $0x2222,%es:0
 movb $0xb8,%es:0x20
 movw $0x2222,%es:0x21
 movb $0xcb,%es:0x23
 mov $32,%cx
cycle:
 push %cx
 xor %bx,%bx
 call map
 cmpw $0x1111,%es:0
 jne fail
 lcall *ems_entry
 cmp $0x1111,%ax
 jne fail
 mov $1,%bx
 call map
 cmpw $0x2222,%es:0
 jne fail
 lcall *ems_entry
 cmp $0x2222,%ax
 jne fail
 pop %cx
 loop cycle
 mov ems_handle,%dx
 mov $0x45,%ah
 int $0x67
 test %ah,%ah
 jnz fail
 mov xms_handle,%dx
 mov $0x0a,%ah
 lcall *xms_entry
 cmp $1,%ax
 jne fail
 mov $0x42,%ah
 int $0x67
 cmp free_pages,%bx
 jne fail
 mov $passmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
map:
 mov ems_handle,%dx
 mov $0x4400,%ax
 int $0x67
 test %ah,%ah
 jnz fail
 ret
fail:
 mov $failmsg,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
xms_entry: .word 0,0
ems_entry: .word 0x20,0
ems_handle: .word 0
xms_handle: .word 0
free_pages: .word 0
passmsg: .ascii "EMS/XMS remap execution and shared allocation PASS\r\n$"
failmsg: .ascii "EMS/XMS FAIL\r\n$"
