/* Original protected-mode test: nested faults while resolving a missing page. */
.intel_syntax noprefix
.code16
.global _start
_start:
 cli
 xor ax,ax
 mov ds,ax
 mov ss,ax
 mov sp,0x7000
 mov al,2
 out 0x92,al
 lgdt [gdt_ptr]
 lidt [idt_ptr]
 mov eax,cr0
 or al,1
 mov cr0,eax
 .byte 0xea
 .word pm,8
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
 mov eax,3
 mov ecx,1024
pages:
 stosd
 add eax,4096
 loop pages
 mov dword ptr [0x180000],0xface1234
 mov dword ptr [0x181000],0x44556677
 mov dword ptr [0x11600],0x180002
 mov dword ptr [0x11604],0x181002
 mov eax,0x10000
 mov cr3,eax
 mov eax,cr0
 or eax,0x80000000
 mov cr0,eax
 mov eax,dword ptr [0x180000]
 cmp eax,0xface1234
 jne fail
 cmp dword ptr [0x504],2
 jne fail
 /* Leave the BIOS text aperture unmapped at stop, like a paged OS may.
  * Teardown must not invoke a guest fault after its escape frame unwinds. */
 mov dword ptr [0x112e0],0
 mov eax,cr3
 mov cr3,eax
 mov word ptr [0x500],0xfa17
 jmp idle
fail:
 mov word ptr [0x500],0xfa11
idle:
 hlt
 jmp idle
pf:
 push eax
 mov eax,cr2
 cmp eax,0x180000
 jne nested
 /* Resolving the first fault itself causes a second fault. */
 mov eax,dword ptr [0x181000]
 cmp eax,0x44556677
 jne fail
 mov dword ptr [0x11600],0x180003
 jmp resume
nested:
 cmp eax,0x181000
 jne fail
 mov dword ptr [0x11604],0x181003
resume:
 mov eax,cr3
 mov cr3,eax
 inc dword ptr [0x504]
 pop eax
 add esp,4
 iretd
.balign 8
gdt:
 .quad 0
 .quad 0x00cf9a000000ffff
 .quad 0x00cf92000000ffff
gdt_end:
gdt_ptr:
 .word gdt_end-gdt-1
 .long gdt
idt:
 .space 14*8
 .word pf,8
 .byte 0,0x8e
 .word 0
idt_end:
idt_ptr:
 .word idt_end-idt-1
 .long idt
.org 510
.word 0xaa55
