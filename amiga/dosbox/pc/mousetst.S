/* Original DOS INT33 / BIOS PS2 integration test. Host mailbox at 0000:0500. */
.intel_syntax noprefix
.code16
.global _start
_start:
 sti
 mov ax,0x12
 int 0x10
 xor ax,ax
 int 0x33
 cmp ax,0xffff
 jne fail
 mov ax,7
 xor cx,cx
 mov dx,639
 int 0x33
 mov ax,8
 xor cx,cx
 mov dx,479
 int 0x33
 xor ax,ax
 mov es,ax
 mov byte ptr es:[0x500],1
one:
 mov ax,3
 int 0x33
 cmp bx,1
 jne one
 cmp cx,158
 jb fail
 cmp cx,160
 ja fail
 cmp dx,358
 jb fail
 cmp dx,360
 ja fail
 mov byte ptr es:[0x500],2
two:
 mov ax,3
 int 0x33
 cmp bx,6
 jne two
 mov ax,6
 xor bx,bx
 int 0x33
 cmp bx,1
 jne fail
 mov byte ptr es:[0x500],3
three:
 mov ax,3
 int 0x33
 test bx,bx
 jnz three
 /* BIOS callback protocol used by Windows standard PS/2 driver. */
 push cs
 pop es
 mov bx,offset ps2_handler
 mov ax,0xc207
 int 0x15
 jc fail
 mov ax,0xc200
 mov bh,1
 int 0x15
 jc fail
 xor ax,ax
 mov es,ax
 mov byte ptr es:[0x500],4
four:
 cmp byte ptr cs:[callback_seen],1
 jne four
 mov ax,0xc200
 xor bh,bh
 int 0x15
 jc fail
 mov byte ptr es:[0x500],5
 mov dx,offset pass
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
ps2_handler:
 push bp
 mov bp,sp
 test word ptr ss:[bp+12],1
 jz ps2_done
 mov byte ptr cs:[callback_seen],1
ps2_done:
 pop bp
 .byte 0xcb /* 16-bit far return; LLVM retf defaults to operand-size 32. */
fail:
 mov dx,offset bad
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
callback_seen: .byte 0
pass: .ascii "DOS mouse + PS2 IRQ12 PASS\r\n$"
bad: .ascii "DOS mouse FAIL\r\n$"
