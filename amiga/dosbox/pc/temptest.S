/* GPL-2.0-or-later. DOS temporary-file error isolation regression. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 /* DOS/4GW uses INT 21h/5Ah for its swap file. A missing-name probe
    must not poison a successful temporary-file creation with error 2. */
 mov $tempname,%dx
 xor %cx,%cx
 mov $0x5a,%ah
 int $0x21
 jc fail
 mov %ax,%bx
 cmpb $0,tempname+1
 je fail
 mov $0x3e,%ah
 int $0x21
 jc fail
 mov $tempname,%dx
 mov $0x41,%ah
 int $0x21
 jc fail
 /* A missing directory must still fail and report path not found. */
 mov $badtemp,%dx
 xor %cx,%cx
 mov $0x5a,%ah
 int $0x21
 jnc fail
 cmp $3,%ax
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
tempname: .asciz "\\"
 .space 128
badtemp: .asciz "NO_DIR\\"
 .space 128
passmsg: .ascii "DOS temporary file creation and missing directory PASS\r\n$"
failmsg: .ascii "DOS temporary file FAIL\r\n$"
