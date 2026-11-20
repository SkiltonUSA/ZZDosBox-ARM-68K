#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "amiga/dosbox/glide/experimental/mmio_guard.h"

/* Link labels for the native policy tests; ARM execution uses real assembly. */
const uint32_t grx_mmio_load_insn=0, grx_mmio_store_insn=0;
const uint32_t grx_mmio_load_failed=0, grx_mmio_store_failed=0;
static uint32_t addr(const uint32_t *p) { return (uint32_t)(uintptr_t)p; }
static void attempt(unsigned write, uint32_t dfsr, uint32_t dfar,
                    uint32_t spsr, uint32_t pc, int expected)
{
    uint32_t frame[6]={0x83c80014,2,3,4,12,pc+8}, before[6];
    memcpy(before,frame,sizeof(frame));
    assert(grx_mmio_abort(frame,dfsr,dfar,spsr)==expected);
    if(expected) {
        assert(frame[5]==addr(write?&grx_mmio_store_failed:&grx_mmio_load_failed)+8);
        assert(!memcmp(frame,before,5*sizeof(uint32_t)));
    } else assert(!memcmp(frame,before,sizeof(frame)));
}
int main(void)
{
    uint32_t load=addr(&grx_mmio_load_insn), store=addr(&grx_mmio_store_insn);
    attempt(0,0x1008,0x83c80014,0x13,load,1); /* precise SLVERR read */
    attempt(1,0x1808,0x83c80014,0x1f,store,1); /* precise SLVERR write */
    attempt(0,0x0008,0x83c80014,0x13,load,1); /* precise DECERR */
    attempt(0,0x1406,0x83c80014,0x13,load,0); /* asynchronous abort */
    attempt(0,0x1005,0x83c80014,0x13,load,0); /* translation fault */
    attempt(0,0x1008,0x83c80010,0x13,load,0); /* wrong address */
    attempt(0,0x1808,0x83c80014,0x13,load,0); /* wrong direction */
    attempt(0,0x1008,0x83c80014,0x33,load,0); /* Thumb exception */
    attempt(0,0x1008,0x83c80014,0x12,load,0); /* IRQ mode */
    attempt(0,0x1008,0x83c80014,0x13,load+4,0); /* foreign instruction */
    assert(grx_mmio_mapping_ok(0x413fc090,0,1,0,0x10004000,0x10004000,0x83c00c02));
    assert(!grx_mmio_mapping_ok(0x413fc090,1,1,0,0x10004000,0x10004000,0x83c00c02));
    assert(!grx_mmio_mapping_ok(0x413fc090,0,0,0,0x10004000,0x10004000,0x83c00c02));
    assert(!grx_mmio_mapping_ok(0x413fc090,0,1,1,0x10004000,0x10004000,0x83c00c02));
    assert(!grx_mmio_mapping_ok(0x413fc090,0,1,0,0x10008000,0x10004000,0x83c00c02));
    assert(!grx_mmio_mapping_ok(0x413fc090,0,1,0,0x10004000,0x10004000,0x83c00c06));
    assert(!grx_mmio_mapping_ok(0x413fc090,0,0x10000001,0,0x10004000,0x10004000,0x83c00c02));
    assert(!grx_mmio_mapping_ok(0x413fc090,0,0x40000001,0,0x10004000,0x10004000,0x83c00c02));
    puts("PASS guarded MMIO: precise faults recover; unrelated faults unchanged; strongly ordered identity mapping required");
}
