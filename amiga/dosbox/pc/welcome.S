/* Original ZZDosBox welcome panel. DOS 5/6 compatible; no ANSI.SYS or COLOR. */
#include "../launcher/version.h"
.intel_syntax noprefix
.code16
.global _start
_start:
 mov ah,0x30
 int 0x21
 add al,'0'
 mov byte ptr [dosmajor],al
 mov al,ah
 xor ah,ah
 mov bl,10
 div bl
 add al,'0'
 add ah,'0'
 mov byte ptr [dosminor],al
 mov byte ptr [dosminor+1],ah
 mov ax,3
 int 0x10
 mov ah,0x0f
 int 0x10
 cmp al,7
 mov ax,0xb800
 jne color_text
 mov ax,0xb000
color_text:
 mov es,ax
 cld
 xor di,di
 mov ax,0x0720
 mov cx,2000
 rep stosw
 mov di,162
 mov dx,19
panel:
 mov ax,0x1f20
 mov cx,78
 rep stosw
 add di,4
 dec dx
 jnz panel
 mov di,162
 mov ax,0x1bc9
 stosw
 mov al,0xcd
 mov cx,76
 rep stosw
 mov al,0xbb
 stosw
 mov di,3042
 mov al,0xc8
 stosw
 mov al,0xcd
 mov cx,76
 rep stosw
 mov al,0xbc
 stosw
 mov di,322
 mov cx,17
sides:
 mov ax,0x1bba
 mov word ptr es:[di],ax
 mov word ptr es:[di+154],ax
 add di,160
 loop sides
 mov si,offset lines
line:
 lodsw
 cmp ax,0xffff
 je done
 mov di,ax
 mov ah,0x1f
 cmp di,8*160
 jae chars
 mov ah,0x1e
chars:
 lodsb
 test al,al
 jz line
 stosw
 jmp chars
done:
 mov ah,2
 xor bx,bx
 mov dx,0x1500
 int 0x10
 mov ax,0x4c00
 int 0x21
lines:
 .word 3*160+12
 .asciz "ZZZZ ZZZZ DDD   OOO   SSS  BBB   OOO  X   X"
 .word 4*160+12
 .asciz "  Z    Z  D  D O   O S     B  B O   O  X X"
 .word 5*160+12
 .asciz " Z    Z   D  D O   O  SSS  BBB  O   O   X"
 .word 6*160+12
 .asciz "Z    Z    D  D O   O     S B  B O   O  X X"
 .word 7*160+12
 .asciz "ZZZZ ZZZZ DDD   OOO  SSSS  BBB   OOO  X   X"
 .word 9*160+12
 .ascii "DosBox ZZ9000 project                         "
 .asciz ZZ_LAUNCHER_VERSION
 .word 10*160+12
 .asciz "AmigaOS / 32-bit Cortex-A9 / XACP"
 .word 11*160+12
 .ascii "DOS "
dosmajor: .ascii "0."
dosminor: .asciz "00"
 .word 14*160+12
 .asciz "Alt+Return       Window / fullscreen"
 .word 15*160+12
 .asciz "Ctrl+- / Ctrl++  CPU speed"
 .word 16*160+12
 .asciz "Ctrl+F5          Capture screenshot"
 .word 17*160+12
 .asciz "F10              Return to Workbench"
 .word 0xffff
