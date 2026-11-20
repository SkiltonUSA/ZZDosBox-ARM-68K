#ifndef GRX_MMIO_GUARD_H
#define GRX_MMIO_GUARD_H
#include <stdint.h>
/* BSP DataAbortHandler frame: r0-r3, r12, exception LR. */
int grx_mmio_abort(uint32_t frame[6], uint32_t dfsr, uint32_t dfar, uint32_t spsr);
int grx_mmio_mapping_ok(uint32_t midr, uint32_t mpidr, uint32_t sctlr,
                        uint32_t ttbcr, uint32_t ttbr0, uint32_t table,
                        uint32_t section);
/* Private primitives: caller must first verify the mapping and vector hook.
 * IRQ/FIQ masks are preserved. No promise of recovery from a nonresponding bus. */
int grx_mmio_load(uint32_t address, uint32_t *value);
int grx_mmio_store(uint32_t address, uint32_t value);
extern const uint32_t grx_mmio_load_insn, grx_mmio_store_insn;
extern const uint32_t grx_mmio_load_failed, grx_mmio_store_failed;
#endif
