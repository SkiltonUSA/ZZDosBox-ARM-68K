/* GPL-3.0-or-later. Two bounded SPSC Ethernet queues inside owned uncached DDR.
 * Every index has one writer and a separate cache line; no firmware allocation. */
#pragma once
#include "pc_wire.h"
#define NET_CONTROL 0x18c000u
#define NET_MAGIC 0x4e455431u
#define NET_PROTOCOL (NET_CONTROL+4u) /* host: 0 Ethernet/NAT, 1 IPX/UDP */
#define NET_SERVER (NET_CONTROL+8u) /* host: IPv4 in canonical integer order */
#define NET_PORT (NET_CONTROL+12u) /* host: UDP port */
#define NET_IPX_CONNECTED (NET_CONTROL+320u) /* ARM only, separate cache line */
#define NET_ENABLED (NET_CONTROL+0u)
#define NET_TX_WRITE (NET_CONTROL+64u) /* ARM */
#define NET_TX_READ (NET_CONTROL+128u) /* host */
#define NET_RX_WRITE (NET_CONTROL+192u) /* host */
#define NET_RX_READ (NET_CONTROL+256u) /* ARM */
#define NET_TX_DATA (NET_CONTROL+512u)
#define NET_COUNT 4u
#define NET_STRIDE 1600u
#define NET_MTU 1518u
#define NET_RX_DATA (NET_TX_DATA+NET_COUNT*NET_STRIDE)
#if NET_RX_DATA+NET_COUNT*NET_STRIDE > DB_PROGRAM
#error Network queues overlap guest executable storage
#endif
static inline unsigned net_slot(unsigned data,unsigned seq){return data+(seq&(NET_COUNT-1))*NET_STRIDE;}
static inline int net_pending(unsigned write,unsigned read){unsigned n=write-read;return n>NET_COUNT?-1:(int)n;}

#ifdef DB_COMPAT
#include "audio_wire.h"
#if NET_RX_DATA+NET_COUNT*NET_STRIDE > PC_AUDIO_CONTROL
#error Network queues overlap audio control
#endif
#endif
