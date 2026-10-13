/* GPL-2.0-or-later. Original EGA/PIT integration fixture. */
.intel_syntax noprefix
.code16
.global _start
_start:
 mov ax,0x000d
 int 0x10
 mov ax,0xa000
 mov es,ax
 mov si,offset planes
 mov bx,1
plane:
 mov dx,0x3c4
 mov al,2
 out dx,al
 inc dx
 mov al,bl
 out dx,al
 xor di,di
 mov cx,8000
 rep movsb
 shl bl,1
 cmp bl,16
 jne plane
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
pass: .ascii "EGA/PIT PASS\r\n$"
planes: .incbin "ega-planes.bin"
