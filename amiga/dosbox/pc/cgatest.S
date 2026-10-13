/* Original CGA packed-pixel and BIOS timer fixture. */
.intel_syntax noprefix
.code16
.global _start
_start:
 mov ax,4
 int 0x10
 mov ax,0xb800
 mov es,ax
 xor di,di
 mov cx,8192
 mov ax,0x5555
 rep stosw
 sti
 xor ah,ah
 int 0x1a
 mov bx,dx
wait:
 xor ah,ah
 int 0x1a
 sub dx,bx
 cmp dx,5
 jb wait
 mov dx,offset pass
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
pass: .ascii "CGA/PIT PASS\r\n$"
