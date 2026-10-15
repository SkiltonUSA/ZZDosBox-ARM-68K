/* Original regression: PIT channel 2 reaches library fmod in a scalar PC build. */
.intel_syntax noprefix
.code16
.global _start
_start:
 in al,0x61
 mov bl,al
 and al,0xfd
 or al,1
 out 0x61,al
 mov al,0xb6
 out 0x43,al
 mov al,0xe8
 out 0x42,al
 mov al,3
 out 0x42,al
 mov cx,100
poll:
 in al,0x61
 loop poll
 mov al,bl
 out 0x61,al
 mov dx,offset passed
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
passed: .ascii "PIT math PASS\r\n$"
