/* Original regression for bounded register reuse, partial registers and flags. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 mov ecx,2048
 xor edx,edx
again:
 mov eax,0x12345678
 mov ebx,0x89abcdef
 add edx,1
 mov esi,eax
 mov edi,ebx
 cmp esi,0x12345678
 jne fail
 cmp edi,0x89abcdef
 jne fail
 mov ah,0xa5
 cmp eax,0x1234a578
 jne fail
 mov bx,0x2468
 cmp ebx,0x89ab2468
 jne fail
 stc
 mov esi,eax
 adc esi,1
 cmp esi,0x1234a57a
 jne fail
 cmp eax,0x1234a578
 jne fail
 dec ecx
 jnz again
 cmp edx,2048
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
passed: .asciz "JIT bounded reuse PASS\r\n$"
failed: .asciz "JIT bounded reuse FAIL\r\n$"
