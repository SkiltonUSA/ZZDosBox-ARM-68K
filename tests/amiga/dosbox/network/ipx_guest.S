/* Original real-mode IPX API test: socket, ECB receive/ESR, broadcast and reply. */
.intel_syntax noprefix
.code16
.section .text
.global _start
_start:
 push cs
 pop ds
 mov ax,0x7a00
 int 0x2f
 cmp al,0xff
 jne fail
 push ds
 pop es
 mov ax,ds
 mov word ptr [receive+38],ax
 mov word ptr [sendecb+38],ax
 mov word ptr [receive+6],ax
 mov dx,0x2345
 xor bx,bx
 xor ax,ax
 int 0x7a
 test al,al
 jne fail
 mov si,offset address
 mov bx,9
 int 0x7a
 mov si,offset receive
 mov bx,4
 int 0x7a
 test al,al
 jne fail
 mov si,offset sendecb
 mov bx,3
 int 0x7a
 test al,al
 jne fail
 cmp byte ptr [sendecb+9],0
 jne fail
 mov cx,5000
waiting:
 sti
 hlt
 cmp byte ptr [receive+8],0
 jne idle
 cmp byte ptr [esr_seen],1
 je received
idle:
 push cx
 mov bx,0x0a
 int 0x7a
 pop cx
 loop waiting
 jmp fail
received:
 cmp byte ptr [receive+9],0
 jne fail
 mov si,offset recvdata+30
 mov di,offset expected
 mov cx,10
 repe cmpsb
 jne fail
 mov bx,1
 mov dx,0x2345
 int 0x7a
 mov dx,offset ok
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
fail:
 mov dx,offset bad
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
esr:
 mov byte ptr cs:[esr_seen],1
 .byte 0xcb /* 16-bit RETF; clang Intel mnemonic emits operand-size override. */
esr_seen:.byte 0
address:.space 10
receive:
 .long 0
 .word esr,0
 .byte 0,0
 .word 0x2345
 .space 16
 .space 6
 .word 1
 .word recvdata,0,128
sendecb:
 .long 0
 .long 0
 .byte 0,0
 .word 0x2345
 .space 16
 .space 6
 .word 1
 .word packet,0,40
packet:
 .byte 255,255,0,40,0,0
 .long 0
 .byte 255,255,255,255,255,255,0x45,0x23
 .space 12
 .ascii "ZZIPXHELLO"
recvdata:.space 128
expected:.ascii "ZZIPXREPLY"
ok:.asciz "PASS IPX socket, broadcast, receive and ESR\r\n$"
bad:.asciz "FAIL IPX guest fixture\r\n$"
