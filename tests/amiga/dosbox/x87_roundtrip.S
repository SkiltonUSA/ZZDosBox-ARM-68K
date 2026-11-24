/* Original fixture: all rounding policies, signed ties, saved CW and stack. */
.code16
.global _start
_start:
 push %cs
 pop %ds
 fninit
 mov $controls,%si
 mov $expected,%di
 mov $4,%bp
mode:
 fldcw (%si)
 mov $inputs,%bx
 mov $4,%cx
value:
 fldl (%bx)
 frndint
 fistpl result
 movl result,%eax
 cmpl (%di),%eax
 jne fail
 add $8,%bx
 add $4,%di
 loop value
 /* Save two values and the active rounding mode, disturb it, restore. */
 fld1
 fldl negative
 fnsave state
 fldcw controls
 frstor state
 fnstcw result
 mov (%si),%ax
 cmp result,%ax
 jne fail
 fistpl result
 /* Last table entry is the expected -3.5 result in this mode. */
 movl result,%eax
 cmpl -4(%di),%eax
 jne fail
 fistpl result
 cmpl $1,result
 jne fail
 add $2,%si
 dec %bp
 jnz mode
 fninit
 mov $passed,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c00,%ax
 int $0x21
fail:
 mov $failed,%dx
 mov $9,%ah
 int $0x21
 mov $0x4c01,%ax
 int $0x21
controls: .word 0x037f,0x077f,0x0b7f,0x0f7f
inputs: .double 2.5,3.5,-2.5
negative: .double -3.5
expected: .long 2,4,-2,-4,2,3,-3,-4,3,4,-2,-3,2,3,-2,-3
result: .long 0
state: .space 108
passed: .asciz "x87 rounding restore PASS\r\n$"
failed: .asciz "x87 rounding restore FAIL\r\n$"
