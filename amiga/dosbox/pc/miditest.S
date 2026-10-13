/* Original MPU-401 fixture. Run against a capture output, not an instrument. */
.intel_syntax noprefix
.code16
.global _start
_start:
 sti
 mov dx,0x331
 mov al,0xac
 out dx,al
 call read_byte
 cmp al,0xfe
 jne fail
 call read_byte
 cmp al,0x15
 jne fail
 mov dx,0x331
 mov al,0x3f
 out dx,al
 call read_byte
 cmp al,0xfe
 jne fail
 mov cx,5000
 mov dx,0x330
 mov al,0xf8
clock:
 out dx,al
 loop clock
 mov si,offset data
 mov cx,15
send:
 lodsb
 out dx,al
 loop send
 mov dx,offset pass
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
read_byte:
 mov dx,0x331
 mov cx,0xffff
wait:
 in al,dx
 test al,0x80
 jz ready
 loop wait
 jmp fail
ready:
 dec dx
 in al,dx
 ret
fail:
 mov dx,offset failed
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
data: .byte 0x91,60,64,62,65,0xf8,0x81,60,0,0xf0,0x7d,1,2,3,0xf7
data_end:
pass: .ascii "MPU401 intelligent/UART PASS\r\n$"
failed: .ascii "MPU401 FAIL\r\n$"
