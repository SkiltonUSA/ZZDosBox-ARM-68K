/* GPL-2.0-or-later. Original DOS file/handle/memory diagnostic. */
.intel_syntax noprefix
.code16
.section .text
.global _start
_start:
 cld
 mov ah,0x30
 int 0x21
 cmp al,5
 jne fail
 // Open the external input, read it, seek and check EOF.
 mov dx,offset input
 mov ax,0x3d00
 int 0x21
 jc fail
 mov bx,ax
 mov dx,offset buffer
 mov cx,256
 mov ah,0x3f
 int 0x21
 jc fail
 cmp ax,256
 jne fail
 mov si,offset buffer
 mov cx,256
 mov dl,7
check:
 lodsb
 cmp al,dl
 jne fail
 add dl,13
 loop check
 xor cx,cx
 xor dx,dx
 mov ax,0x4202
 int 0x21
 jc fail
 cmp ax,256
 jne fail
 mov dx,offset buffer
 mov cx,1
 mov ah,0x3f
 int 0x21
 jc fail
 test ax,ax
 jnz fail
 mov ah,0x3e
 int 0x21
 jc fail
 // Create a file, write the bytes, close, reopen and compare.
 mov dx,offset output
 xor cx,cx
 mov ah,0x3c
 int 0x21
 jc fail
 mov bx,ax
 mov dx,offset buffer
 mov cx,256
 mov ah,0x40
 int 0x21
 jc fail
 cmp ax,256
 jne fail
 mov ah,0x3e
 int 0x21
 jc fail
 mov dx,offset output
 mov ax,0x3d00
 int 0x21
 jc fail
 mov bx,ax
 mov dx,offset buffer2
 mov cx,256
 mov ah,0x3f
 int 0x21
 jc fail
 cmp ax,256
 jne fail
 mov ah,0x3e
 int 0x21
 jc fail
 push ds
 pop es
 mov si,offset buffer
 mov di,offset buffer2
 mov cx,256
 repe cmpsb
 jne fail
 // Missing files must report carry + error 2.
 mov dx,offset missing
 mov ax,0x3d00
 int 0x21
 jnc fail
 cmp ax,2
 jne fail
 mov dx,offset message
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
fail:
 mov ax,0x4c01
 int 0x21
input:.asciz "INPUT.TXT"
output:.asciz "OUTPUT.TXT"
missing:.asciz "MISSING.TXT"
message:.ascii "ZZDOS files/handles/seek PASS\r\n$"
buffer:.space 256
buffer2:.space 256
