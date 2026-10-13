/* GPL-2.0-or-later. Original MZ relocation and paragraph-allocation test. */
.intel_syntax noprefix
.code16
.section .text
.global _start,reloc_check
_start:
 push cs
 pop ds
 mov ax,word ptr [reloc_check]
 mov bx,cs
 cmp ax,bx
 jne fail
 // Release unneeded child memory before requesting a second block.
 mov ax,es
 mov bx,0x200
 mov ah,0x4a
 int 0x21
 jc fail
 mov bx,0x100
 mov ah,0x48
 int 0x21
 jc fail
 mov es,ax
 mov byte ptr es:[0],0x5a
 cmp byte ptr es:[0],0x5a
 jne fail
 mov ah,0x49
 int 0x21
 jc fail
 mov dx,offset message
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
fail:
 mov ax,0x4c02
 int 0x21
reloc_check:.word 0
message:.ascii "ZZDOS MZ relocation/memory PASS\r\n$"
