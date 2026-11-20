#ifndef GRX_COMPLETION_CLIENT_H
#define GRX_COMPLETION_CLIENT_H
#include <stdint.h>

/* Experimental firmware helper, not a released XACP interface.
 * The caller owns GPU submission and result consumption exclusively.
 * Transport callbacks return 1 only after a successful, ordered transaction;
 * errors/timeouts return 0. Plain unchecked volatile MMIO is not sufficient.
 * Offsets are relative to an already verified GRX1 endpoint. */
typedef struct {
    void *context;
    int (*read)(void *, unsigned offset, uint32_t *value);
    int (*write)(void *, unsigned offset, uint32_t value);
} grx_completion_io;

typedef enum {
    GRX_COMPLETION_PENDING,
    GRX_COMPLETION_RETIRED,
    GRX_COMPLETION_CANCELLED,
    GRX_COMPLETION_QUARANTINED
} grx_completion_state;

typedef struct {
    uint32_t epoch, sequence;
    grx_completion_state state;
} grx_completion_batch;

/* One bounded poll; no busy wait. Initialize a newly submitted batch with its
 * ordered token identity and PENDING. Terminal results are retained. Resources
 * may be released only for RETIRED/CANCELLED, after the transport's required
 * DMA visibility/cache operation. An outer deadline never authorizes release. */
grx_completion_state grx_completion_poll(const grx_completion_io *io,
                                        grx_completion_batch *batch);
#endif
