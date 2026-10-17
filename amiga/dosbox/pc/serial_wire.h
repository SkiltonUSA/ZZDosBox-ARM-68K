/* GPL-3.0-or-later. COM1 byte streams; each mutable index owns a cache line. */
#pragma once
#include "pc_wire.h"
#define PC_SERIAL_MODE 0x199000u /* 0 disabled, 1 TCP, 2 Hayes TCP modem, 3 physical device */
#define PC_SERIAL_TX_WRITE 0x199040u
#define PC_SERIAL_TX_READ 0x199080u
#define PC_SERIAL_RX_WRITE 0x1990c0u
#define PC_SERIAL_RX_READ 0x199100u
#define PC_SERIAL_STATUS 0x199140u /* host: bit 0 carrier, bit 1 service ready */
#define PC_SERIAL_DTR 0x199180u /* ARM: DTR state */
#define PC_SERIAL_CONFIG 0x1991c0u /* ARM: divisor | (LCR << 16), atomic word */
#define PC_SERIAL_BREAK 0x199200u /* ARM: break request generation */
#define PC_SERIAL_CTS 4u
#define PC_SERIAL_DSR 8u
#define PC_SERIAL_RI 16u
#define PC_SERIAL_TX_DATA 0x19a000u
#define PC_SERIAL_RX_DATA 0x19b000u
#define PC_SERIAL_SIZE 4096u
#if PC_SERIAL_RX_DATA+PC_SERIAL_SIZE > 0x1a0000u
#error Serial queues overlap audio
#endif
