/* GPL-2.0-or-later. Original adapter, banked VRAM and VBE LFB fixture. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 cld
#if TEST_ADAPTER == 3
 push cs
 pop es
 mov di,offset info
 mov dword ptr es:[di],0x32454256
 mov ax,0x4f00
 int 0x10
 cmp ax,0x004f
 jne fail
 cmp word ptr es:[di+18],16
 jne fail
 mov ax,0x4f01
 mov cx,TEST_MODE
 int 0x10
 cmp ax,0x004f
 jne fail
 mov eax,dword ptr es:[di+40]
 mov dword ptr [lfb],eax
 mov ax,0x4f02
 mov bx,TEST_MODE
 int 0x10
 cmp ax,0x004f
 jne fail
 /* Reject dimensions beyond capture and modes beyond 1 MiB VRAM. */
 mov ax,0x4f02
 mov bx,0x107
 int 0x10
 cmp ax,0x014f
 jne fail
 mov ax,0x4f01
 mov cx,0x117
 push cs
 pop es
 mov di,offset info
 int 0x10
 cmp ax,0x014f
 jne fail
#if TEST_BPP == 8
 mov dx,0x3c8
 mov al,1
 out dx,al
 inc dx
 mov al,63
 out dx,al
 out dx,al
 out dx,al
#endif
 xor bp,bp
bank:
 mov ax,0x4f05
 xor bx,bx
 mov dx,bp
 int 0x10
 cmp ax,0x004f
 jne fail
 mov ax,0xa000
 mov es,ax
 xor di,di
 mov cx,16384
#if TEST_BPP == 8
 mov eax,0x01010101
#elif TEST_BPP == 15
 mov eax,0x03e003e0
#elif TEST_BPP == 16
 mov eax,0x07e007e0
#else
 mov eax,0x00ffffff
#endif
 rep stosd
 inc bp
 cmp bp,16
 jb bank
 /* Return to bank zero, then check LFB aliases that same video memory. */
 mov ax,0x4f05
 xor bx,bx
 xor dx,dx
 int 0x10
 cli
 xor eax,eax
 mov ax,cs
 shl eax,4
 add eax,offset gdt
 mov dword ptr [gdtptr+2],eax
 lgdt [gdtptr]
 mov eax,cr0
 or al,1
 mov cr0,eax
 mov ax,8
 mov gs,ax
 mov eax,cr0
 and al,0xfe
 mov cr0,eax
 mov edi,dword ptr [lfb]
 mov eax,dword ptr es:[0]
 cmp dword ptr gs:[edi],eax
 jne fail
 not eax
 mov dword ptr gs:[edi],eax
 cmp dword ptr es:[0],eax
 jne fail
 not eax
 mov dword ptr gs:[edi],eax
 xor ax,ax
 mov gs,ax
 sti
#elif TEST_ADAPTER == 6
 mov dx,0x3bf
 mov al,3
 out dx,al
 mov dx,0x3b8
 xor al,al
 out dx,al
 mov si,offset crtc
 xor bx,bx
herc_regs:
 mov dx,0x3b4
 mov al,bl
 out dx,al
 inc dx
 lodsb
 out dx,al
 inc bl
 cmp bl,12
 jb herc_regs
 mov dx,0x3b8
 mov al,0x0a
 out dx,al
 mov ax,0xb000
 mov es,ax
 xor di,di
 mov cx,16384
 mov ax,0xffff
 rep stosw
#else
 mov ax,9
 int 0x10
#if TEST_ADAPTER == 5
 mov ax,0x1800 /* PCjr 32K graphics lives in reserved conventional RAM. */
#else
 mov ax,0xb800
#endif
 mov es,ax
 xor di,di
 mov cx,16384
 mov ax,0x5555
 rep stosw
#endif
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
fail:
 sti
 mov dx,offset failed
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
pass: .ascii "ADAPTER PASS\r\n$"
failed: .ascii "ADAPTER FAIL\r\n$"
crtc: .byte 0x35,0x2d,0x2e,7,0x5b,2,0x57,0x57,2,3,0,0
.balign 8
gdt: .quad 0,0x00cf92000000ffff
gdtptr: .word 15
 .long 0
lfb: .long 0
info: .space 512
