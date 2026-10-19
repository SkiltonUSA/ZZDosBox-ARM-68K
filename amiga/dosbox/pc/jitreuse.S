/* Original regression: exercise resident-register load/store forwarding. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 mov eax,0x12345678
 mov ebx,eax
 mov ecx,ebx
 mov edx,ecx
 cmp edx,0x12345678
 jne fail
 mov dx,offset passed
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
fail:
 mov dx,offset failed
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
passed: .asciz "JIT register reuse PASS\r\n$"
failed: .asciz "JIT register reuse FAIL\r\n$"
