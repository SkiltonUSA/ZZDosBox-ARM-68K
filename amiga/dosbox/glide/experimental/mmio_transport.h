#ifndef GRX_MMIO_TRANSPORT_H
#define GRX_MMIO_TRANSPORT_H
#include <stdint.h>
typedef struct { uint32_t ready; } grx_mmio_transport;
/* Requires the BSP abort hook and this image's MMUTable/vector symbols. */
int grx_mmio_init(grx_mmio_transport *transport);
int grx_mmio_read(void *transport, unsigned offset, uint32_t *value);
int grx_mmio_write(void *transport, unsigned offset, uint32_t value);
#endif
