/* Original x86 DOS fixture, GPL-2.0-or-later. No game assets. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 mov $banner,%dx
 mov $9,%ah
 int $0x21
loop:
 mov $prompt,%dx
 mov $9,%ah
 int $0x21
 mov $line,%dx
 mov $0x0a,%ah
 int $0x21
 mov $newline,%dx
 mov $9,%ah
 int $0x21
 cmpb $4,line+1
 jne print
 cmpl $0x74697865,line+2
 je done
print:
 mov $reply,%dx
 mov $9,%ah
 int $0x21
 xor %cx,%cx
 mov line+1,%cl
 mov $line+2,%dx
 mov $1,%bx
 mov $0x40,%ah
 int $0x21
 mov $newline,%dx
 mov $9,%ah
 int $0x21
 jmp loop
done:
 mov $bye,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
banner: .ascii "ZZDosBox console demo\r\n$"
prompt: .ascii "Type text (exit to quit): $"
reply: .ascii "You typed: $"
newline: .ascii "\r\n$"
bye: .ascii "Goodbye.\r\n$"
line: .byte 80,0
 .space 80
