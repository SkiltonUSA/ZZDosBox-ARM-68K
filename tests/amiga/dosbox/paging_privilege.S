/* Original two-sector boot fixture: CPL3 instruction-fetch protection fault. */
.intel_syntax noprefix
.code16
.global _start
_start:
 cli
 xor ax,ax
 mov ds,ax
 mov es,ax
 mov ss,ax
 mov sp,0x7000
 mov bx,0x7e00
 mov ax,0x0201
 mov cx,2
 xor dx,dx
 int 0x13
 jc read_fail
 mov al,2
 out 0x92,al
 lgdt [gdt_ptr]
 lidt [idt_ptr]
 mov eax,cr0
 or al,1
 mov cr0,eax
 .byte 0xea
 .word pm,8
read_fail:
 mov word ptr [0x500],0xfa11
 hlt
 jmp read_fail
.balign 8
gdt:
 .quad 0
 .quad 0x00cf9a000000ffff
 .quad 0x00cf92000000ffff
 .quad 0x00cffa000000ffff
 .quad 0x00cff2000000ffff
 .quad 0x0000890120000067 /* TSS base 12000h, limit 67h */
gdt_end:
gdt_ptr: .word gdt_end-gdt-1
 .long gdt
idt:
 .space 14*8
 .word pf,8
 .byte 0,0x8e
 .word 0
idt_end:
idt_ptr: .word idt_end-idt-1
 .long idt
.org 510
.word 0xaa55
.code32
pm:
 mov ax,16
 mov ds,ax
 mov es,ax
 mov ss,ax
 mov esp,0x90000
 cld
 mov edi,0x10000
 xor eax,eax
 mov ecx,3072
 rep stosd
 mov dword ptr [0x10000],0x11003
 mov edi,0x11000
 mov eax,7
 mov ecx,1024
pages:
 stosd
 add eax,4096
 loop pages
 mov dword ptr [0x12004],0x90000
 mov word ptr [0x12008],16
 mov word ptr [0x12066],0x68
 mov ax,40
 ltr ax
 mov esi,offset user_code
 mov edi,0x180000
 mov ecx,offset user_size
 rep movsb
#ifdef TEST_486
 mov dword ptr [0x11600],0x180007 /* 486 faults on supervisor PDE alone. */
#else
 mov dword ptr [0x11600],0x180003 /* Pinned 386 requires both U/S bits clear. */
#endif
 mov eax,0x10000
 mov cr3,eax
 mov eax,cr0
 or eax,0x80000000
 mov cr0,eax
 mov ax,0x23
 mov ds,ax
 mov es,ax
 push 0x23
 push 0x80000
 push 2
 push 0x1b
 push 0x180000
 iretd
pf:
 push eax
 mov eax,cr2
 cmp eax,0x180000
 jne fail
 cmp dword ptr [esp+4],5
 jne fail
 inc dword ptr [0x504]
 or dword ptr [0x10000],4
 or dword ptr [0x11600],4
 mov dword ptr [0x112e0],0
 mov eax,cr3
 mov cr3,eax
 pop eax
 add esp,4
 iretd
fail:
 mov word ptr [0x500],0xfa11
 hlt
 jmp fail
user_code:
 mov eax,0x12345678
 cmp eax,0x12345678
 jne user_bad
 mov word ptr [0x500],0xfa17
user_wait:
 inc ebx
 jmp user_wait
user_bad:
 mov word ptr [0x500],0xfa11
 jmp user_wait
user_end:
.equ user_size,user_end-user_code
.org 1024
