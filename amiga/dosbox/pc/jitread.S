/* Original COM regression: byte/word/dword reads at every page alignment,
 * partial registers, carry preservation, and the text-video device handler. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 push cs
 pop es
 cld
 mov di,0x3000
 mov eax,0xa55aa55a
 mov cx,2050
 rep stosd
 mov di,0x3000
 mov cx,4096
again:
 stc
 mov edx,dword ptr [di]
 jnc fail
 cmp edx,eax
 jne fail
 movzx edx,word ptr [di]
 cmp dx,ax
 jne fail
 shr edx,16
 jnz fail
 movzx edx,byte ptr [di]
 cmp dl,al
 jne fail
 shr edx,8
 jnz fail
 mov edx,0x12345678
 mov dl,byte ptr [di]
 shr edx,8
 cmp edx,0x123456
 jne fail
 ror eax,8
 inc di
 loop again
 mov ax,0xb800
 mov es,ax
 mov word ptr es:[100],0x1e41
 mov bx,word ptr es:[100]
 cmp bx,0x1e41
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
passed: .asciz "JIT checked reads PASS\r\n$"
failed: .asciz "JIT checked reads FAIL\r\n$"
