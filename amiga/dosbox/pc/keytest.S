/* Original keyboard IRQ/BIOS integration fixture. */
.intel_syntax noprefix
.code16
.global _start
_start:
 sti
 xor ah,ah
 int 0x16
 cmp al,'a'
 jne fail
 mov dx,offset pass
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
fail:
 mov ax,0x4c01
 int 0x21
pass: .ascii "PC keyboard PASS\r\n$"
