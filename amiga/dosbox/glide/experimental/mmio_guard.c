#include "mmio_guard.h"

int grx_mmio_mapping_ok(uint32_t midr, uint32_t mpidr, uint32_t sctlr,
                        uint32_t ttbcr, uint32_t ttbr0, uint32_t table,
                        uint32_t section)
{
    /* Verified target A9 r3p0, Core0, short descriptors, identity-mapped
     * privileged RW Strongly-ordered PL section. Do not rewrite the live MMU. */
    return midr==0x413fc090u && (mpidr&255u)==0 &&
           (sctlr&0x72000001u)==1 && ttbcr==0 && !(table&0x3fffu) &&
           (ttbr0&0xffffc000u)==table && section==0x83c00c02u;
}

int grx_mmio_abort(uint32_t frame[6], uint32_t dfsr, uint32_t dfar, uint32_t spsr)
{
    uint32_t pc=frame[5]-8, mode=spsr&31u;
    uint32_t load=(uint32_t)(uintptr_t)&grx_mmio_load_insn;
    uint32_t store=(uint32_t)(uintptr_t)&grx_mmio_store_insn;
    /* Only synchronous external aborts at these two word accesses. All other
     * exceptions retain the BSP's original handler and original saved state. */
    if ((dfsr&0x40fu)!=8 || (spsr&0x20u) || (mode!=0x13 && mode!=0x1f) ||
        frame[0]<0x83c80000u || frame[0]>0x83c8001cu || (frame[0]&3u) ||
        dfar!=frame[0]) return 0;
    if (pc==load && !(dfsr&0x800u))
        frame[5]=(uint32_t)(uintptr_t)&grx_mmio_load_failed+8;
    else if (pc==store && (dfsr&0x800u))
        frame[5]=(uint32_t)(uintptr_t)&grx_mmio_store_failed+8;
    else return 0;
    return 1;
}
