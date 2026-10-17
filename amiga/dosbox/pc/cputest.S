/* GPL-2.0-or-later. Guest-visible CPU flags and CPUID identity. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 pushfl
 pop %ebx
 mov %ebx,%eax
 xor $0x240000,%eax
 push %eax
 popfl
 pushfl
 pop %eax
 push %ebx
 popfl
 xor %ebx,%eax
 and $0x240000,%eax
 jz cpu386
 cmp $0x240000,%eax
 jne fail
 mov $0,%eax
 cpuid
 cmp $1,%eax
 jne fail
 cmp $0x756e6547,%ebx
 jne fail
 mov $1,%eax
 cpuid
 cmp $0x402,%eax
 je cpu486
 cmp $0x513,%eax
 jne fail
 test $0x10,%edx
 jz fail
 rdtsc
 mov $pentium,%dx
 jmp pass
cpu486:
 mov $msg486,%dx
 jmp pass
cpu386:
 mov $msg386,%dx
pass:
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
fail:
 mov $bad,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
msg386: .ascii "CPU 386 PASS\r\n$"
msg486: .ascii "CPU 486 PASS\r\n$"
pentium: .ascii "CPU Pentium PASS\r\n$"
bad: .ascii "CPU FAIL\r\n$"
