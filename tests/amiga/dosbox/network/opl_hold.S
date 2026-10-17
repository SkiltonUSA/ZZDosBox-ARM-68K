/* GPL-2.0-or-later. Keep FM active after exit, while the following network workload runs. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $notes,%si
 mov $12,%cx
next:
 lodsw
 push %cx
 call regwrite
 pop %cx
 loop next
 mov $message,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
regwrite:
 mov $0x388,%dx
 out %al,%dx
 mov $6,%cx
pause1:
 in %dx,%al
 loop pause1
 mov %ah,%al
 inc %dx
 out %al,%dx
 dec %dx
 mov $35,%cx
pause2:
 in %dx,%al
 loop pause2
 ret
notes:
.word 0x2001,0x0120,0x0123,0x1040,0x0043,0xf060,0xf063,0x7780,0x7783,0x01c0,0x98a0,0x31b0
message: .ascii "OPL held\r\n$"
