/* Original COM: all RAM page offsets, partial writes, flags, video handler. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 push cs
 pop es
 mov di,0x3000
 mov cx,4096
again:
 mov eax,0x1234a55a
 stc
 mov dword ptr [di],eax
 jnc fail
 cmp dword ptr [di],eax
 jne fail
 mov byte ptr [di],0x69
 cmp dword ptr [di],0x1234a569
 jne fail
 mov word ptr [di],0x7654
 cmp dword ptr [di],0x12347654
 jne fail
 inc di
 loop again
 mov ax,0xb800
 mov es,ax
 mov word ptr es:[100],0x1e41
 cmp word ptr es:[100],0x1e41
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
passed: .asciz "JIT checked writes PASS\r\n$"
failed: .asciz "JIT checked writes FAIL\r\n$"
