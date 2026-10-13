/* Original x86 DOS fixture: status, no-echo, echo, buffered editing and output. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $0x0b,%ah
 int $0x21
 test %al,%al
 jnz fail
 mov $0x06,%ah
 mov $0xff,%dl
 int $0x21
 jnz fail
 mov $ready,%dx
 mov $9,%ah
 int $0x21
 mov $7,%ah
 int $0x21
 cmp $'Z',%al
 jne fail
 mov $0x0b,%ah
 int $0x21
 cmp $0xff,%al
 jne fail
 mov $1,%ah
 int $0x21
 cmp $'Q',%al
 jne fail
 mov $line,%dx
 mov $0x0a,%ah
 int $0x21
 cmpb $3,line+1
 jne fail
 cmpl $0x0d435841,line+2 /* AXC followed by CR, input AB BS XC CR */
 jne fail
 mov $0x0b,%ah
 int $0x21
 test %al,%al
 jnz fail
 /* Output exceeds the transport ring and old 160-byte diagnostic capture. */
 mov $5000,%cx
 mov $'~',%dl
output:
 mov $2,%ah
 int $0x21
 loop output
 mov $pass,%dx
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
ready: .ascii "READY\r\n$"
pass: .ascii "\r\nCONSOLE PASS\r\n$"
failmsg: .ascii "\r\nCONSOLE FAIL\r\n$"
line: .byte 16,0
 .space 16
