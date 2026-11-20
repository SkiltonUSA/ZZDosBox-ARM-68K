.syntax unified
.arm
.text
.align 2
.global grx_mmio_load, grx_mmio_store
.global grx_mmio_load_insn, grx_mmio_store_insn
.global grx_mmio_load_failed, grx_mmio_store_failed
.type grx_mmio_load,%function
grx_mmio_load:
    push {r4,lr}
    mrs r4,cpsr
    cpsid if
    dsb sy
grx_mmio_load_insn:
    ldr r2,[r0]
    dsb sy
    str r2,[r1]
    mov r0,#1
    b .Lload_done
grx_mmio_load_failed:
    mov r0,#0
.Lload_done:
    msr cpsr_c,r4
    pop {r4,pc}
.size grx_mmio_load,.-grx_mmio_load

.type grx_mmio_store,%function
grx_mmio_store:
    push {r4,lr}
    mrs r4,cpsr
    cpsid if
    dsb sy
grx_mmio_store_insn:
    str r1,[r0]
    dsb sy
    mov r0,#1
    b .Lstore_done
grx_mmio_store_failed:
    mov r0,#0
.Lstore_done:
    msr cpsr_c,r4
    pop {r4,pc}
.size grx_mmio_store,.-grx_mmio_store
.section .note.GNU-stack,"",%progbits
