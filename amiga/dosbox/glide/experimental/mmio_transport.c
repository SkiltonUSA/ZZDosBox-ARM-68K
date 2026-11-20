#include "mmio_transport.h"
#include "mmio_guard.h"
extern uint32_t MMUTable[], _vector_table[];

int grx_mmio_init(grx_mmio_transport *transport)
{
    uint32_t midr,mpidr,sctlr,ttbcr,ttbr0,vbar;
    transport->ready=0;
    __asm__ volatile("mrc p15,0,%0,c0,c0,0":"=r"(midr));
    __asm__ volatile("mrc p15,0,%0,c0,c0,5":"=r"(mpidr));
    __asm__ volatile("mrc p15,0,%0,c1,c0,0":"=r"(sctlr));
    __asm__ volatile("mrc p15,0,%0,c2,c0,2":"=r"(ttbcr));
    __asm__ volatile("mrc p15,0,%0,c2,c0,0":"=r"(ttbr0));
    __asm__ volatile("mrc p15,0,%0,c12,c0,0":"=r"(vbar));
    if ((sctlr&0x2000u) || vbar!=(uint32_t)(uintptr_t)_vector_table) return 0;
    transport->ready=grx_mmio_mapping_ok(midr,mpidr,sctlr,ttbcr,ttbr0,
                         (uint32_t)(uintptr_t)MMUTable,MMUTable[0x83c]);
    return transport->ready!=0;
}

int grx_mmio_read(void *context, unsigned offset, uint32_t *value)
{
    grx_mmio_transport *transport=context;
    if (!transport->ready || offset>0x1c || (offset&3)) return 0;
    return grx_mmio_load(0x83c80000u+offset,value);
}

int grx_mmio_write(void *context, unsigned offset, uint32_t value)
{
    grx_mmio_transport *transport=context;
    if (!transport->ready || offset>0x1c || (offset&3)) return 0;
    return grx_mmio_store(0x83c80000u+offset,value);
}
