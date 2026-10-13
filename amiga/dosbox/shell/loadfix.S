/* Original DOS COM test helper. Reserve 64 KiB before EXEC for EXEPACK's
 * low-memory wraparound issue. Usage: LOADFIX program.exe (no arguments yet).
 * GPL-2.0-or-later. Not game data or an upstream LOADFIX binary. */
.intel_syntax noprefix
.code16
.global _start
_start:
 mov ax,cs
 mov ds,ax
 mov es,ax
 mov ss,ax
 mov sp,0x800
 mov bx,0x80
 mov ah,0x4a
 int 0x21
 jc fail
 xor cx,cx
 mov cl,byte ptr [0x80]
 mov si,0x81
 mov di,offset filename
skip:
 jcxz fail
 lodsb
 dec cx
 cmp al,' '
 je skip
copy:
 stosb
 jcxz copied
 lodsb
 dec cx
 cmp al,' '
 je copied
 jmp copy
copied:
 xor al,al
 stosb
 mov bx,0x1000
 mov ah,0x48
 int 0x21
 jc fail
 mov ax,cs
 mov ds,ax
 mov es,ax
 mov word ptr [params+4],ax
 mov dx,offset filename
 mov bx,offset params
 mov ax,0x4b00
 int 0x21
 jc fail
 mov ah,0x4d
 int 0x21
 mov ah,0x4c
 int 0x21
fail:
 mov ax,0x4c01
 int 0x21
params: .word 0,tail,0,0,0,0,0
tail: .byte 0,13
filename: .space 128
