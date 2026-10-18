/* GPL-2.0-or-later. Unpacking may replace invalid bytes before execution.
 * A translator must not abort merely by looking ahead into those bytes. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 movw $0x9090,target
target:
 .byte 0xfe,0xd0
 mov $passed,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
passed: .ascii "JIT speculative invalid bytes PASS\r\n$"
