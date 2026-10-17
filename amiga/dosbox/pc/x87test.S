/* GPL-2.0-or-later. Guest x87 BIOS, arithmetic, rounding and save/restore. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 int $0x11
 test $2,%ax
 jz fail
 fninit
 fnstcw result
 cmpw $0x37f,result
 jne fail
 fnstsw %ax
 test %ax,%ax
 jnz fail
 fildl nine
 fsqrt
 fildl seven
 fmulp
 fistpl result
 cmpl $21,result
 jne fail
 fldl fraction
 fistpl result
 cmpl $4,result
 jne fail
 fldcw down
 fldl fraction
 fistpl result
 cmpl $3,result
 jne fail
 fldcw up
 fldl negative
 fistpl result
 cmpl $-3,result
 jne fail
 fldcw trunc
 fldl negative
 fistpl result
 cmpl $-3,result
 jne fail
 fninit
 fld1
 fldpi
 fnsave state
 fnstsw %ax
 test %ax,%ax
 jnz fail
 frstor state
 fistpl result
 cmpl $3,result
 jne fail
 fistpl result
 cmpl $1,result
 jne fail
 /* Extended-real memory round trip and transcendental helper linkage. */
 fildl nine
 fstpt extended
 fldt extended
 fistpl result
 cmpl $9,result
 jne fail
 fldz
 fsin
 fistpl result
 cmpl $0,result
 jne fail
 fldz
 fcos
 fistpl result
 cmpl $1,result
 jne fail
 fldpi
 fidivs two
 fsin
 fistpl result
 cmpl $1,result
 jne fail
 fld1
 fld1
 fpatan
 fptan
 fstp %st(0)
 fistpl result
 cmpl $1,result
 jne fail
 fld1
 fildl eight
 fyl2x
 fistpl result
 cmpl $3,result
 jne fail
 fldl half
 f2xm1
 fld1
 faddp
 fmull hundred
 fistpl result
 cmpl $141,result
 jne fail
 fninit
 mov $pass,%dx
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
.balign 8
fraction: .double 3.75
negative: .double -3.75
half: .double 0.5
hundred: .double 100.0
eight: .long 8
two: .word 2
nine: .long 9
seven: .long 7
result: .long 0
down: .word 0x77f
up: .word 0xb7f
trunc: .word 0xf7f
.balign 8
state: .space 108
extended: .space 10
pass: .asciz "x87 arithmetic rounding state PASS\r\n$"
bad: .asciz "x87 FAIL\r\n$"
