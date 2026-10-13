/* Original DOS COM fixture: repeatedly rewrite already executed code. */
.intel_syntax noprefix
.code16
.global _start
_start:
 push cs
 pop ds
 mov cx,100
again:
 mov byte ptr [target+1],41
 call target
 cmp ax,41
 jne fail
 mov byte ptr [target+1],43
 call target
 cmp ax,43
 jne fail
 loop again
 mov dx,offset passed
 mov ah,9
 int 0x21
 mov ax,0x4c00
 int 0x21
target:
 mov ax,41
 ret
fail:
 mov dx,offset failed
 mov ah,9
 int 0x21
 mov ax,0x4c01
 int 0x21
passed: .asciz "JIT self-modifying guest code PASS\r\n$"
failed: .asciz "JIT self-modifying guest code FAIL\r\n$"
