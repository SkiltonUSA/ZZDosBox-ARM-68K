/* Original fixture: alter warmed code across a 4 KiB page via a segment alias. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 mov ax,cs
 inc ax
 mov es,ax
 /* Choose a target whose physical byte address ends in 0xfff, regardless
  * of the DOS loader's CS. Leave the program and text below the target. */
 mov ax,cs
 shl ax,4
 neg ax
 and ax,0x0fff
 add ax,0x1fff
 mov bx,ax
 mov byte ptr [bx],0xb8
 mov word ptr [bx+1],0x1234
 mov byte ptr [bx+3],0xc3
 mov cx,256
again:
 mov byte ptr es:[bx-15],0x34
 mov byte ptr es:[bx-14],0x12
 call bx
 cmp ax,0x1234
 jne fail
 mov byte ptr es:[bx-15],0x78
 mov byte ptr es:[bx-14],0x56
 call bx
 cmp ax,0x5678
 jne fail
 loop again
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
passed: .asciz "Page alias SMC PASS\r\n$"
failed: .asciz "Page alias SMC FAIL\r\n$"
.org 0x3000
.byte 0
